<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-09-22 12:06:28
# Last Modified: 2026-03-28 11:40:57
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class UserLogin extends Auth
{
    public function auth()
    {
        global $bearerToken;
        $sql = <<<EOD
        select * from s_login_session where f_session=?
        EOD;
        $data = $this->select($sql, "s", [$bearerToken])->fetch_assoc();
        if (empty($data)) {
            dieWithCode(Translator::t("Need authorization"), 401);
        }
        return true;
    }

    public function login($params)
    {
        global $bearerToken;
        $sql = <<<EOD
        select f_first, f_last, f_id, f_phone, f_id as f_user, f_config, f_group
        from s_user 
        where f_login=? and f_password=md5(?)
        EOD;
        $userdata = $this->select($sql, "ss", [$params->username, $params->password])->fetch_assoc();
        if (empty($userdata)) {
            dieWithCode(Translator::t("Access denied"), 401);
        }
        $bearerToken = uuid_v4();
        if ($params->nootp ?? false) {
            $this->result["nootp"] = true;
            $this->result["userdata"] = $userdata;
            $this->writeSession($userdata);
            $this->setUserData($userdata);
            return;
        }
        $params->user = $userdata["f_id"];
        $params->phone = $userdata["f_phone"];
        $this->sendOTP($params);
        $this->result["userdata"] = $userdata;
        $this->result["token"] = $bearerToken;
        $this->echoResult();
    }

    public function PinLogin($params)
    {
        global $bearerToken;
        $sql = <<<EOD
        select f_first, f_last, f_id, f_phone, f_id as f_user, f_config, f_group
        from s_user 
        where f_altpassword=md5(?)
        EOD;
        $userdata = $this->select($sql, "s", [$params->pin])->fetch_assoc();
        if (empty($userdata)) {
            dieWithCode(Translator::t("Access denied"), 401);
        }
        $bearerToken = uuid_v4();
        if ($params->nootp ?? false) {
            $this->result["nootp"] = true;
            $this->result["userdata"] = $userdata;
            $this->writeSession($userdata);
            $this->setUserData($userdata);
            return;
        }
        $params->user = $userdata["f_id"];
        $params->phone = $userdata["f_phone"];
        $this->sendOTP($params);
        $this->result["userdata"] = $userdata;
        $this->result["token"] = $bearerToken;
        $this->echoResult();
    }

    public function HashLogin($params)
    {
        global $bearerToken;
        $bearerToken = $params->token;
        $sql = <<<EOD
        select f_first, f_last, u.f_id, f_phone, u.f_id as f_user, f_config, f_group
        from s_user u
        inner join s_login_session ls on ls.f_user=u.f_id and ls.f_session=?
        EOD;
        $userdata = $this->select($sql, "s", [$params->token])->fetch_assoc();
        if (empty($userdata)) {
            dieWithCode(Translator::t("Access denied"), 401);
        }

        $this->result["userdata"] = $userdata;
        $this->result["token"] = $bearerToken;
        $this->echoResult();
    }

    public function sendOTP($params)
    {

        global $bearerToken;
        global $otp_login;
        global $otp_pass;
        global $otp_url;
        global $otp_ordinator;
        if (substr($params->phone, 0, 4) != "+374") {
            dieWithCode("Are you international hacker? With" . $params->phone . " phone");
        }

        //update previouse tryes, set it to invalid
        $this->select(
            "update s_activation set f_state=3 where f_state=1 and f_phone=?",
            "s",
            [$params->phone],
            true
        );

        $confirmationcode = random_int(1000, 9999);
        $bind["f_ip"] = $this->get_client_ip();
        $bind["f_state"] = 1;
        $bind["f_session"] = $bearerToken;
        $bind["f_created"] = date("Y-m-d H:i:s");
        $bind["f_code"] = $confirmationcode;
        $bind["f_phone"] = $params->phone;
        $bind["f_user"] = $params->user;
        $this->insert("s_activation", $bind);

        $login = $otp_login;
        $password = $otp_pass;

        $data = [
            "messages" => [
                [
                    "recipient" => $params->phone,
                    "priority" => "2",
                    "sms" => [
                        "originator" => $otp_ordinator,
                        "content" => [
                            "text" => "<#> Your OTP code: \n $confirmationcode \n99Yxa876Gxa"
                        ]
                    ],
                    "message-id" => "201902280917"
                ]
            ]
        ];

        $ch = curl_init($otp_url);

        curl_setopt($ch, CURLOPT_HTTPHEADER, [
            "Content-Type: application/json; charset=utf-8"
        ]);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_USERPWD, "$login:$password");
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));

        $response = curl_exec($ch);
        $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $this->result["ottp_response_code"] = $http_code;
        $this->result["otp_address"] = $otp_url;
        $this->result["otp"] = $response;
        unset($ch);
    }

    public function checkOtp($params)
    {
        global $bearerToken;

        $sql = <<<EOD
        select a.f_user from s_activation a where f_session=? and f_state=1 and f_code=?
        EOD;

        $userdata = $this->select($sql, "ss", [$bearerToken, $params->code])->fetch_assoc();
        if (empty($userdata)) {
            dieWithCode(Translator::t("Invalid confirmation code"), 401);
        }
        $this->select(
            "update s_activation set f_state=2 where f_state=1 and f_session=? ",
            "s",
            [$bearerToken],
            true
        );
        $this->writeSession($userdata);
    }

    public function writeSession($userdata)
    {
        global $bearerToken;
        $v["f_session"] = $bearerToken;
        $v["f_user"] = $userdata["f_user"];
        $v["f_datestart"] = date("Y-m-d");
        $v["f_timestart"] = date("H:i:s");
        $v["f_iplogin"] = json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]);
        $this->insert("s_login_session", $v);
        $this->result["token"] = $bearerToken;
        $this->echoResult();
    }

    private function setUserData($main)
    {
        $permissions = $this->select("select f_key from s_user_access where f_group=? and f_value=1", "i", [$main["f_group"]])->fetch_all(MYSQLI_NUM);
        $permissions = array_column($permissions, 0);
        $settings = $this->select("select f_key, f_value from s_settings_values where f_settings=?", "i", [$main["f_config"]])->fetch_all(MYSQLI_ASSOC);
        $jsonconfig = $this->select("select f_config from sys_json_config where f_id=?", "i", [$main["f_config"]])->fetch_assoc();
        $jsonconfig = ["f_config" => (array) json_decode($jsonconfig["f_config"] ?? "{}")];
        $this->result["permissions"] = $permissions;
        $this->result["settings"] = $settings;
        $this->result["jsonconfig"] = $jsonconfig;
    }
}
