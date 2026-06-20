# Web SelfBoard

Web version of the **SelfBoard** self-service ordering kiosk (the Qt app in
[`../selfboard`](../selfboard)). It reproduces the kiosk screens in the browser:

- **Start screen** — language picker (RU / EN / HY) and *Take away* / *Dine in*.
- **Menu screen** — group sidebar, group chips, searchable dish grid, dietary
  badges, prep time, price, and a live cart summary bar.
- **Dish details** — type / size / nutrition (BJU) popup.
- **Package picker** — `f_type = 5` packages with Type/Size attribute selection,
  combination validation, quantity, and computed line price.
- **Cart** — line items with quantity controls and total.

It talks to the same Picasso backend as the kiosk, via a tiny Node proxy that
mirrors `ServerConfig::login` and `MenuCache::preload`:

| Kiosk call | Backend route |
| --- | --- |
| login | `POST /engine/v2/worker/user-login/login` `{ username, password, nootp }` |
| menu  | `POST /engine/v2/waiter/menu/get` `{ locale }` |

The backend session token stays on the server, so credentials never reach the
browser and there are no CORS / self-signed-certificate problems.

## Requirements

- Node.js 18+ (uses only built-in modules — **no `npm install` needed**).

## Configure

Copy the example config and fill in your server / kiosk login:

```bash
cp config.example.json config.json
```

```json
{
  "host": "picassoapp.local",
  "https": false,
  "rejectUnauthorized": false,
  "username": "kiosk-user",
  "password": "kiosk-pass",
  "locale": "en",
  "port": 8080
}
```

Use the same login/password as in the Qt SelfBoard server settings (F3). For a local
Apache vhost, `https` is usually `false` (`picassoapp.local` is typically HTTP only).

`config.json` is git-ignored. Alternatively use environment variables:
`SELFBOARD_HOST`, `SELFBOARD_HTTPS` (`false` for http), `SELFBOARD_INSECURE`
(`true` to accept self-signed certs), `SELFBOARD_USER`, `SELFBOARD_PASSWORD`,
`SELFBOARD_LOCALE`, `PORT`.

## Run

You can deploy either way — both serve the same frontend and the same two API
endpoints (`api/config.php`, `api/menu.php`).

### A) Apache + PHP (e.g. `kiosk.local`)

Point the vhost docroot at `web.selfboard/public`. PHP (with the `curl`
extension) handles `api/config.php` and `api/menu.php` directly — no Node
needed. `config.json` is read from the project root (`web.selfboard/`), one
level above the docroot, so it is not web-accessible.

Then open <http://kiosk.local/>.

### B) Node (zero dependencies, for local dev)

```bash
node server.js
# or: npm start
```

Open <http://localhost:8080> (the Node server also answers the `.php` endpoint
URLs). Note: port 8080 may be taken by Apache — change `port` in `config.json`
or set `PORT`.

## Layout

```
server.js            zero-dependency proxy + static host
config.example.json  copy to config.json
public/
  index.html         screens + modals
  styles.css         design ported from selfboard/res/selfboard.css
  app.js             screen logic, i18n (ru/en/hy), cart
  menu.js            data parsing ported from menuhelpers.cpp
  api/
    config.php       client config (PHP/Apache deployment)
    menu.php         login + menu proxy (PHP/Apache deployment)
    _lib.php         shared backend helpers
  res/               images copied from selfboard/res
```

## Notes

- Menu item/group names are localized server-side, so switching language in the
  menu screen re-fetches `/api/menu`.
- Dietary/allergen badges are rendered as labelled pills (the kiosk used PNG
  icons from a shared resource bundle).
