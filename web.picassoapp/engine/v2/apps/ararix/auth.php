<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

/**
 * Public auth endpoints (no Bearer required).
 * Method names Login / CheckOtp match the global whitelist in v2/index.php,
 * same pattern as worker/user-login.php.
 */
class Auth extends ArarixAuth
{
    /** All actions on this controller are unauthenticated. */
    public function auth()
    {
        return true;
    }

    /** Request OTP — analogous to UserLogin::login (sends code). */
    public function Login($params)
    {
        [$countryCode, $phone] = $this->normalizePhone(
            (string)($params->country_code ?? "+374"),
            (string)($params->phone ?? "")
        );
        $channel = strtolower((string)($params->channel ?? "sms"));
        if (!in_array($channel, ["sms", "whatsapp"], true)) {
            $channel = "sms";
        }
        if ($phone === "" || strlen($phone) < 6) {
            dieWithCode("Invalid phone number", 400);
        }

        $this->createAndSendOtp($countryCode, $phone, $channel, "login");
        $this->result["expires_in"] = 60;
        $this->result["channel"] = $channel;
        $this->echoResult();
    }

    /** Verify OTP — analogous to UserLogin::checkOtp. */
    public function CheckOtp($params)
    {
        [$countryCode, $phone] = $this->normalizePhone(
            (string)($params->country_code ?? "+374"),
            (string)($params->phone ?? "")
        );
        $code = trim((string)($params->code ?? ""));
        if ($phone === "" || $code === "") {
            dieWithCode("Phone and code are required", 400);
        }
        if (!$this->verifyOtpCode($countryCode, $phone, $code, "login")) {
            dieWithCode("Invalid or expired code", 400);
        }

        $client = $this->findOrCreateClient($countryCode, $phone);
        $token = $this->issueSession((int)$client["f_id"]);
        $this->result["token"] = $token;
        $this->result["client"] = $this->clientPayload($client);
        $this->echoResult();
    }

    /** Aliases for older clients / hot-reload leftovers. */
    public function RequestOtp($params)
    {
        $this->Login($params);
    }

    public function ResendOtp($params)
    {
        $this->Login($params);
    }

    public function VerifyOtp($params)
    {
        $this->CheckOtp($params);
    }
}
