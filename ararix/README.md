# Ararix

Flutter client app for Ararix (phone OTP auth, profile, FAQ, language).

## Run

```bash
cd ararix
flutter pub get
flutter run --dart-define=API_BASE=http://YOUR_HOST/engine/v2
```

Android emulator default API base: `http://10.0.2.2/engine/v2`

Headers sent on every request:
- `X-Application-Name: ararix`
- `X-Application-Version: 1.0.0`
- `Authorization: Bearer <token>` after login

OTP is sent via Nikita SMS (`cnf.php`). Only `+374` numbers are supported. WhatsApp button currently also sends SMS until WhatsApp is integrated.

## Dev backend switcher

On the welcome screen type `1981` in the phone field and tap **SMS** — a backend picker opens:

1. `ararix.local`
2. `development.picassocloud.com`
3. Production — placeholder (disabled until URL is known)

Selection is stored in SharedPreferences and used for subsequent API calls.
