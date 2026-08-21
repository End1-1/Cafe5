<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Profile extends ArarixAuth
{
    public function Get($params)
    {
        $this->result["client"] = $this->clientPayload();
        $this->echoResult();
    }

    public function Update($params)
    {
        $data = [];
        if (property_exists($params, "name")) {
            $data["f_name"] = trim((string)$params->name);
        }
        if (property_exists($params, "email")) {
            $email = trim((string)$params->email);
            if ($email !== "" && !filter_var($email, FILTER_VALIDATE_EMAIL)) {
                dieWithCode("Invalid email", 400);
            }
            $data["f_email"] = $email === "" ? null : $email;
        }
        if (property_exists($params, "locale")) {
            $locale = strtolower(trim((string)$params->locale));
            if (!in_array($locale, ["en", "ru", "hy"], true)) {
                dieWithCode("Unsupported locale", 400);
            }
            $data["f_locale"] = $locale;
        }
        if (empty($data)) {
            dieWithCode("Nothing to update", 400);
        }

        $this->update("ararix_clients", $data, $this->clientId);
        $this->client = $this->select("SELECT * FROM ararix_clients WHERE f_id = ?", "i", [$this->clientId])->fetch_assoc();
        $this->result["client"] = $this->clientPayload();
        $this->echoResult();
    }

    public function ChangePhoneRequest($params)
    {
        [$countryCode, $phone] = $this->normalizePhone(
            (string)($params->country_code ?? "+374"),
            (string)($params->phone ?? "")
        );
        $channel = strtolower((string)($params->channel ?? "whatsapp"));
        if (!in_array($channel, ["sms", "whatsapp"], true)) {
            $channel = "whatsapp";
        }
        if ($phone === "" || strlen($phone) < 6) {
            dieWithCode("Invalid phone number", 400);
        }

        $exists = $this->select(
            "SELECT f_id FROM ararix_clients WHERE f_country_code = ? AND f_phone = ? AND f_id <> ? LIMIT 1",
            "ssi",
            [$countryCode, $phone, $this->clientId]
        )->fetch_assoc();
        if (!empty($exists)) {
            dieWithCode("Phone already in use", 400);
        }

        $this->createAndSendOtp($countryCode, $phone, $channel, "change_phone");
        $this->result["expires_in"] = 60;
        $this->result["channel"] = $channel;
        $this->echoResult();
    }

    public function ChangePhoneVerify($params)
    {
        [$countryCode, $phone] = $this->normalizePhone(
            (string)($params->country_code ?? "+374"),
            (string)($params->phone ?? "")
        );
        $code = trim((string)($params->code ?? ""));
        if ($phone === "" || $code === "") {
            dieWithCode("Phone and code are required", 400);
        }
        if (!$this->verifyOtpCode($countryCode, $phone, $code, "change_phone")) {
            dieWithCode("Invalid or expired code", 400);
        }

        $exists = $this->select(
            "SELECT f_id FROM ararix_clients WHERE f_country_code = ? AND f_phone = ? AND f_id <> ? LIMIT 1",
            "ssi",
            [$countryCode, $phone, $this->clientId]
        )->fetch_assoc();
        if (!empty($exists)) {
            dieWithCode("Phone already in use", 400);
        }

        $this->update("ararix_clients", [
            "f_country_code" => $countryCode,
            "f_phone" => $phone,
        ], $this->clientId);

        $this->client = $this->select("SELECT * FROM ararix_clients WHERE f_id = ?", "i", [$this->clientId])->fetch_assoc();
        $this->result["client"] = $this->clientPayload();
        $this->echoResult();
    }
}
