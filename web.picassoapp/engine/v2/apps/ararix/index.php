<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/../../worker/db.php";
require_once __DIR__ . "/../../worker/uuid.php";

abstract class ArarixAuth extends Db
{
    protected $client;
    protected $clientId;

    public function __construct()
    {
        parent::__construct();
    }

    public function auth()
    {
        global $bearerToken;
        if (empty($bearerToken)) {
            return false;
        }

        $sql = <<<EOD
        SELECT c.*
        FROM ararix_clients c
        INNER JOIN ararix_sessions s ON s.f_client_id = c.f_id
        WHERE s.f_token = ?
          AND c.f_status = 1
          AND (s.f_expires_at IS NULL OR s.f_expires_at > NOW())
        EOD;

        $this->client = $this->select($sql, "s", [$bearerToken])->fetch_assoc();
        if (empty($this->client)) {
            return false;
        }
        $this->clientId = (int)$this->client["f_id"];
        return true;
    }

    protected function clientPayload(?array $row = null): array
    {
        $row = $row ?? $this->client;
        return [
            "id" => (int)$row["f_id"],
            "country_code" => $row["f_country_code"],
            "phone" => $row["f_phone"],
            "name" => $row["f_name"],
            "email" => $row["f_email"],
            "locale" => $row["f_locale"],
            "avatar_url" => $row["f_avatar_url"],
            "status" => (int)$row["f_status"],
        ];
    }

    protected function normalizePhone(string $countryCode, string $phone): array
    {
        $countryCode = trim($countryCode);
        if ($countryCode === "" || $countryCode[0] !== "+") {
            $countryCode = "+" . ltrim($countryCode, "+");
        }
        $phone = preg_replace('/\D+/', '', $phone) ?? "";
        return [$countryCode, $phone];
    }

    protected function createOtp(string $countryCode, string $phone, string $channel, string $purpose = "login"): string
    {
        $code = str_pad((string)random_int(0, 9999), 4, "0", STR_PAD_LEFT);
        $this->select(
            "UPDATE ararix_otp SET f_used = 1 WHERE f_country_code = ? AND f_phone = ? AND f_purpose = ? AND f_used = 0",
            "sss",
            [$countryCode, $phone, $purpose],
            true
        );
        $this->insert("ararix_otp", [
            "f_country_code" => $countryCode,
            "f_phone" => $phone,
            "f_code" => $code,
            "f_channel" => $channel,
            "f_purpose" => $purpose,
            "f_expires_at" => date("Y-m-d H:i:s", time() + 300),
            "f_used" => 0,
        ]);
        return $code;
    }

    /**
     * Send OTP via Nikita SMS broker (same as worker/user-login.php).
     * WhatsApp channel is not wired yet — SMS is used for both.
     */
    protected function sendOtpSms(string $countryCode, string $phone, string $code): void
    {
        global $otp_login, $otp_pass, $otp_url, $otp_ordinator;

        $recipient = $countryCode . $phone;
        if (substr($recipient, 0, 4) !== "+374") {
            dieWithCode("Only +374 numbers are supported for SMS", 400);
        }
        if (empty($otp_url) || empty($otp_login) || empty($otp_pass)) {
            dieWithCode("SMS gateway is not configured", 500);
        }

        $data = [
            "messages" => [
                [
                    "recipient" => $recipient,
                    "priority" => "2",
                    "sms" => [
                        "originator" => $otp_ordinator ?: "Ararix",
                        "content" => [
                            "text" => "Ararix code: $code",
                        ],
                    ],
                    "message-id" => "ararix-" . time() . "-" . $phone,
                ],
            ],
        ];

        $ch = curl_init($otp_url);
        curl_setopt($ch, CURLOPT_HTTPHEADER, [
            "Content-Type: application/json; charset=utf-8",
        ]);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_USERPWD, "$otp_login:$otp_pass");
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
        curl_setopt($ch, CURLOPT_TIMEOUT, 20);

        $response = curl_exec($ch);
        $httpCode = (int)curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $curlError = curl_error($ch);
        curl_close($ch);

        if ($response === false || $httpCode < 200 || $httpCode >= 300) {
            dieWithCode(
                "SMS send failed" . ($curlError !== "" ? ": $curlError" : " (HTTP $httpCode)"),
                502
            );
        }
    }

    protected function createAndSendOtp(string $countryCode, string $phone, string $channel, string $purpose = "login"): string
    {
        $code = $this->createOtp($countryCode, $phone, $channel, $purpose);
        $this->sendOtpSms($countryCode, $phone, $code);
        return $code;
    }

    protected function verifyOtpCode(string $countryCode, string $phone, string $code, string $purpose = "login"): bool
    {
        $row = $this->select(
            "SELECT * FROM ararix_otp WHERE f_country_code = ? AND f_phone = ? AND f_code = ? AND f_purpose = ? AND f_used = 0 AND f_expires_at > NOW() ORDER BY f_id DESC LIMIT 1",
            "ssss",
            [$countryCode, $phone, $code, $purpose]
        )->fetch_assoc();

        if (empty($row)) {
            return false;
        }

        $this->update("ararix_otp", ["f_used" => 1], (int)$row["f_id"]);
        return true;
    }

    protected function issueSession(int $clientId): string
    {
        $token = uuid_v4();
        $this->insert("ararix_sessions", [
            "f_client_id" => $clientId,
            "f_token" => $token,
            "f_expires_at" => date("Y-m-d H:i:s", time() + 60 * 60 * 24 * 90),
        ]);
        return $token;
    }

    protected function findOrCreateClient(string $countryCode, string $phone): array
    {
        $row = $this->select(
            "SELECT * FROM ararix_clients WHERE f_country_code = ? AND f_phone = ? LIMIT 1",
            "ss",
            [$countryCode, $phone]
        )->fetch_assoc();

        if (!empty($row)) {
            return $row;
        }

        $id = $this->insert("ararix_clients", [
            "f_country_code" => $countryCode,
            "f_phone" => $phone,
            "f_locale" => Translator::$locale ?: "en",
            "f_status" => 1,
        ]);

        return $this->select("SELECT * FROM ararix_clients WHERE f_id = ?", "i", [$id])->fetch_assoc();
    }

    protected function absoluteMediaUrl(?string $url): ?string
    {
        $url = trim((string)$url);
        if ($url === '') {
            return null;
        }
        if (preg_match('#^https?://#i', $url)) {
            return $url;
        }
        $scheme = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') ? 'https' : 'http';
        $host = (string)($_SERVER['HTTP_HOST'] ?? '');
        if ($host === '') {
            return $url;
        }
        return $scheme . '://' . $host . (str_starts_with($url, '/') ? $url : '/' . $url);
    }

    protected function haversineMeters(float $lat1, float $lng1, float $lat2, float $lng2): float
    {
        $earth = 6371000.0;
        $dLat = deg2rad($lat2 - $lat1);
        $dLng = deg2rad($lng2 - $lng1);
        $a = sin($dLat / 2) ** 2
            + cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * sin($dLng / 2) ** 2;
        return 2 * $earth * asin(min(1.0, sqrt($a)));
    }

    protected function clientAddressStub(): array
    {
        return [
            "label" => "Komitas Avenue, 8",
            "lat" => 40.1872,
            "lng" => 44.5121,
        ];
    }
}
