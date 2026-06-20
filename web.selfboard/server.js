"use strict";

// Web SelfBoard — tiny zero-dependency Node server.
//
// It serves the kiosk frontend (public/) and proxies two calls to the Picasso
// backend, mirroring the Qt kiosk (ServerConfig::login + MenuCache::preload):
//   POST /engine/v2/worker/user-login/login   { username, password, nootp }
//   POST /engine/v2/waiter/menu/get           { locale }
//
// The backend session token is kept server-side so credentials never reach the
// browser, and CORS/self-signed-cert issues are avoided.

const fs = require("fs");
const path = require("path");
const http = require("http");
const https = require("https");
const os = require("os");
const { URL } = require("url");

const ROOT = __dirname;
const PUBLIC_DIR = path.join(ROOT, "public");

const LOGIN_ROUTE = "/engine/v2/worker/user-login/login";
const MENU_ROUTE = "/engine/v2/waiter/menu/get";

function loadConfig() {
  const defaults = {
    host: "",
    https: true,
    rejectUnauthorized: true,
    username: "",
    password: "",
    locale: "en",
    port: 8080,
    appVersion: "0.1.0.76",
  };

  let fileCfg = {};
  const cfgPath = path.join(ROOT, "config.json");
  if (fs.existsSync(cfgPath)) {
    try {
      fileCfg = JSON.parse(fs.readFileSync(cfgPath, "utf8"));
    } catch (e) {
      console.error(`Failed to parse config.json: ${e.message}`);
    }
  }

  const env = process.env;
  const envCfg = {};
  if (env.SELFBOARD_HOST) envCfg.host = env.SELFBOARD_HOST;
  if (env.SELFBOARD_HTTPS) envCfg.https = env.SELFBOARD_HTTPS !== "false";
  if (env.SELFBOARD_INSECURE) envCfg.rejectUnauthorized = env.SELFBOARD_INSECURE !== "true";
  if (env.SELFBOARD_USER) envCfg.username = env.SELFBOARD_USER;
  if (env.SELFBOARD_PASSWORD) envCfg.password = env.SELFBOARD_PASSWORD;
  if (env.SELFBOARD_LOCALE) envCfg.locale = env.SELFBOARD_LOCALE;
  if (env.PORT) envCfg.port = parseInt(env.PORT, 10);

  return Object.assign(defaults, fileCfg, envCfg);
}

const config = loadConfig();

function normalizeHost(raw) {
  let host = String(raw || "").trim();
  if (/^https?:\/\//i.test(host)) {
    host = host.replace(/^https?:\/\//i, "");
  }
  return host.replace(/\/+$/, "");
}

const APP_NAME = "selfboard";
const HOST_INFO = os.hostname().toLowerCase();

// Server-side session cache, like NDataProvider::sessionKey.
let sessionKey = "";

/**
 * POST a JSON body to the backend, replicating NDataProvider::getData headers
 * and the envelope fields (sessionkey, app, appversion, hostinfo).
 */
function backendPost(route, params) {
  return new Promise((resolve, reject) => {
    const host = normalizeHost(config.host);
    if (!host) {
      reject(new Error("Backend host is not configured (set config.json or SELFBOARD_HOST)."));
      return;
    }

    const useHttps = config.https !== false;
    const target = new URL(`${useHttps ? "https" : "http"}://${host}${route}`);

    const body = Object.assign(
      {
        sessionkey: sessionKey,
        hostinfo: HOST_INFO,
        app: APP_NAME,
        appversion: config.appVersion,
      },
      params || {}
    );
    const payload = Buffer.from(JSON.stringify(body), "utf8");

    const options = {
      method: "POST",
      hostname: target.hostname,
      port: target.port || (useHttps ? 443 : 80),
      path: target.pathname + target.search,
      headers: {
        "Content-Type": "application/json",
        "Content-Length": payload.length,
        Authorization: `Bearer ${sessionKey}`,
        "X-Application-Name": APP_NAME,
        "X-Application-Version": config.appVersion,
        "X-Application-Host": HOST_INFO,
      },
    };

    const transport = useHttps ? https : http;
    if (useHttps && config.rejectUnauthorized === false) {
      options.rejectUnauthorized = false;
    }

    const req = transport.request(options, (res) => {
      const chunks = [];
      res.on("data", (c) => chunks.push(c));
      res.on("end", () => {
        const raw = Buffer.concat(chunks).toString("utf8");
        if (res.statusCode === 401) {
          reject(Object.assign(new Error("Authorization error"), { httpCode: 401 }));
          return;
        }
        let json;
        try {
          json = raw ? JSON.parse(raw) : {};
        } catch (e) {
          reject(new Error(`Invalid JSON from backend (${res.statusCode}): ${raw.slice(0, 200)}`));
          return;
        }
        resolve({ httpCode: res.statusCode, json });
      });
    });

    req.on("error", (err) => reject(err));
    req.setTimeout(120000, () => req.destroy(new Error("Backend request timed out")));
    req.write(payload);
    req.end();
  });
}

async function login() {
  if (!config.username || !config.password) {
    throw new Error("Backend username/password are not configured.");
  }
  sessionKey = "";
  const { json } = await backendPost(LOGIN_ROUTE, {
    username: config.username,
    password: config.password,
    nootp: true,
  });
  if (json.status !== 1) {
    throw new Error(json.message || "Login failed");
  }
  const token = json.token || json.sessionkey;
  if (!token) {
    throw new Error("Login succeeded but token is empty");
  }
  sessionKey = token;
  return token;
}

async function fetchMenu(locale) {
  const doFetch = () => backendPost(MENU_ROUTE, { locale });

  if (!sessionKey) {
    await login();
  }

  let resp;
  try {
    resp = await doFetch();
  } catch (e) {
    if (e.httpCode === 401) {
      await login();
      resp = await doFetch();
    } else {
      throw e;
    }
  }

  // Stale session can return 401 (handled above) or status!=1 — re-login once.
  if (resp.httpCode === 401 || (resp.json && resp.json.status !== 1)) {
    await login();
    resp = await doFetch();
  }

  if (resp.json.status !== 1) {
    throw new Error(resp.json.message || "Menu request failed");
  }
  return resp.json;
}

// ---------------------------------------------------------------------------
// Static file serving
// ---------------------------------------------------------------------------

const MIME = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".jpeg": "image/jpeg",
  ".gif": "image/gif",
  ".svg": "image/svg+xml",
  ".ico": "image/x-icon",
  ".webp": "image/webp",
};

function sendJson(res, code, obj) {
  const body = JSON.stringify(obj);
  res.writeHead(code, { "Content-Type": "application/json; charset=utf-8" });
  res.end(body);
}

function serveStatic(req, res) {
  let urlPath = decodeURIComponent(new URL(req.url, "http://localhost").pathname);
  if (urlPath === "/") urlPath = "/index.html";

  const filePath = path.normalize(path.join(PUBLIC_DIR, urlPath));
  if (!filePath.startsWith(PUBLIC_DIR)) {
    res.writeHead(403);
    res.end("Forbidden");
    return;
  }

  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
      res.end("Not found");
      return;
    }
    const ext = path.extname(filePath).toLowerCase();
    res.writeHead(200, { "Content-Type": MIME[ext] || "application/octet-stream" });
    res.end(data);
  });
}

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, "http://localhost");

  if (url.pathname === "/api/config" || url.pathname === "/api/config.php") {
    sendJson(res, 200, {
      appVersion: config.appVersion,
      defaultLocale: config.locale || "en",
      locales: ["ru", "en", "hy"],
      configured: Boolean(normalizeHost(config.host) && config.username && config.password),
    });
    return;
  }

  if (url.pathname === "/api/menu" || url.pathname === "/api/menu.php") {
    const locale = url.searchParams.get("locale") || config.locale || "en";
    try {
      const menu = await fetchMenu(locale);
      sendJson(res, 200, {
        status: 1,
        groups: menu.groups || [],
        dishes: menu.dishes || [],
      });
    } catch (e) {
      console.error(`/api/menu failed: ${e.message}`);
      sendJson(res, 200, { status: 0, message: e.message });
    }
    return;
  }

  if (req.method !== "GET" && req.method !== "HEAD") {
    res.writeHead(405);
    res.end("Method not allowed");
    return;
  }

  serveStatic(req, res);
});

const port = config.port || 8080;
server.listen(port, () => {
  const host = normalizeHost(config.host);
  console.log(`Web SelfBoard listening on http://localhost:${port}`);
  if (!host || !config.username || !config.password) {
    console.warn(
      "WARNING: backend is not fully configured. Copy config.example.json to " +
        "config.json and fill host/username/password (or set SELFBOARD_* env vars)."
    );
  } else {
    console.log(`Backend: ${config.https !== false ? "https" : "http"}://${host}`);
  }
});
