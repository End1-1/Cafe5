<?php
// Shared backend proxy for the PHP deployment (Apache).
// Mirrors server.js / the Qt kiosk: login + waiter/menu/get against Picasso.

const SB_APP_NAME = 'selfboard';
const SB_LOGIN_ROUTE = '/engine/v2/worker/user-login/login';
const SB_MENU_ROUTE = '/engine/v2/waiter/menu/get';

function sb_config(): array
{
    $defaults = [
        'host' => '',
        'https' => true,
        'rejectUnauthorized' => true,
        'username' => '',
        'password' => '',
        'locale' => 'en',
        'appVersion' => '0.1.0.76',
    ];

    $cfg = $defaults;

    // config.json lives in the project root (web.selfboard/), one level above
    // the web docroot — readable by PHP but not web-accessible.
    foreach ([__DIR__ . '/../../config.json', __DIR__ . '/../config.json'] as $path) {
        if (is_file($path)) {
            $j = json_decode((string)file_get_contents($path), true);
            if (is_array($j)) {
                $cfg = array_merge($cfg, $j);
            }
            break;
        }
    }

    $env = getenv();
    if (!empty($env['SELFBOARD_HOST'])) $cfg['host'] = $env['SELFBOARD_HOST'];
    if (isset($env['SELFBOARD_HTTPS'])) $cfg['https'] = $env['SELFBOARD_HTTPS'] !== 'false';
    if (isset($env['SELFBOARD_INSECURE'])) $cfg['rejectUnauthorized'] = $env['SELFBOARD_INSECURE'] !== 'true';
    if (!empty($env['SELFBOARD_USER'])) $cfg['username'] = $env['SELFBOARD_USER'];
    if (!empty($env['SELFBOARD_PASSWORD'])) $cfg['password'] = $env['SELFBOARD_PASSWORD'];
    if (!empty($env['SELFBOARD_LOCALE'])) $cfg['locale'] = $env['SELFBOARD_LOCALE'];

    return $cfg;
}

function sb_normalize_host(string $raw): string
{
    $host = preg_replace('#^https?://#i', '', trim($raw));
    return rtrim($host, '/');
}

/**
 * POST a JSON body to the backend, replicating NDataProvider::getData headers
 * and the envelope fields (sessionkey, app, appversion, hostinfo).
 *
 * @return array{0:int,1:string} [httpCode, rawBody]
 */
function sb_post(array $cfg, string $route, array $params, string $token): array
{
    $host = sb_normalize_host((string)$cfg['host']);
    if ($host === '') {
        throw new RuntimeException('Backend host is not configured (set config.json or SELFBOARD_HOST).');
    }

    $useHttps = ($cfg['https'] ?? true) !== false;
    $scheme = $useHttps ? 'https' : 'http';
    $url = "$scheme://$host$route";

    $hostInfo = strtolower(gethostname() ?: 'web');

    $body = array_merge([
        'sessionkey' => $token,
        'hostinfo' => $hostInfo,
        'app' => SB_APP_NAME,
        'appversion' => $cfg['appVersion'],
    ], $params);
    $payload = json_encode($body, JSON_UNESCAPED_UNICODE);

    $ch = curl_init($url);
    curl_setopt_array($ch, [
        CURLOPT_POST => true,
        CURLOPT_POSTFIELDS => $payload,
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_TIMEOUT => 120,
        CURLOPT_HTTPHEADER => [
            'Content-Type: application/json',
            'Authorization: Bearer ' . $token,
            'X-Application-Name: ' . SB_APP_NAME,
            'X-Application-Version: ' . $cfg['appVersion'],
            'X-Application-Host: ' . $hostInfo,
        ],
    ]);

    if ($useHttps && ($cfg['rejectUnauthorized'] ?? true) === false) {
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    }

    $resp = curl_exec($ch);
    $httpCode = (int)curl_getinfo($ch, CURLINFO_HTTP_CODE);
    $err = curl_error($ch);
    curl_close($ch);

    if ($resp === false) {
        throw new RuntimeException($err !== '' ? $err : 'Backend request failed');
    }
    return [$httpCode, (string)$resp];
}

function sb_login(array $cfg): string
{
    if (empty($cfg['username']) || empty($cfg['password'])) {
        throw new RuntimeException('Backend username/password are not configured.');
    }
    [, $resp] = sb_post($cfg, SB_LOGIN_ROUTE, [
        'username' => $cfg['username'],
        'password' => $cfg['password'],
        'nootp' => true,
    ], '');

    $j = json_decode($resp, true);
    if (!is_array($j) || (int)($j['status'] ?? 0) !== 1) {
        $msg = is_array($j) ? trim((string)($j['message'] ?? '')) : trim($resp);
        throw new RuntimeException($msg !== '' ? $msg : 'Login failed');
    }
    $token = $j['token'] ?? ($j['sessionkey'] ?? '');
    if ($token === '') {
        throw new RuntimeException('Login succeeded but token is empty');
    }
    return (string)$token;
}

function sb_fetch_menu(array $cfg, string $locale): array
{
    $token = sb_login($cfg);
    [$code, $resp] = sb_post($cfg, SB_MENU_ROUTE, ['locale' => $locale], $token);

    $j = json_decode($resp, true);
    if (!is_array($j)) {
        throw new RuntimeException("Invalid JSON from backend (HTTP $code)");
    }
    if ((int)($j['status'] ?? 0) !== 1) {
        throw new RuntimeException($j['message'] ?? 'Menu request failed');
    }
    return $j;
}

function sb_send_json(array $data): void
{
    header('Content-Type: application/json; charset=utf-8');
    echo json_encode($data, JSON_UNESCAPED_UNICODE);
}
