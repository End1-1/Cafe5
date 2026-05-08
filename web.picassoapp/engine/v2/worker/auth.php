<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 18:37:09
# Last Modified: 2025-12-31 21:50:06

require_once __DIR__ . "/db.php";
require_once __DIR__ . "/permissions.php";

abstract class Auth extends Db
{
    protected $user;
    protected $userid;
    protected $permissions;
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
        select u.* from s_user u 
        left join s_login_session s on s.f_user=u.f_id 
        where f_session=?
        EOD;
        $this->user = $this->select($sql, "s", [$bearerToken])->fetch_assoc();
        if (empty($this->user)) {
            return false;
        }
        $this->userid = $this->user["f_id"];
        $rows = $this->select("select f_key, f_value from s_user_access where f_group=? and f_value=1", "i", [$this->user["f_group"]])->fetch_all(MYSQLI_NUM);
        $prm = array_column($rows, 0);
        $this->permissions = new Permissions($prm);
        return true;
    }

    public function get_client_ip()
    {
        if (array_key_exists('HTTP_X_FORWARDED_FOR', $_SERVER)) {
            return  $_SERVER['HTTP_X_FORWARDED_FOR'];
        } else if (array_key_exists('REMOTE_ADDR', $_SERVER)) {
            return $_SERVER['REMOTE_ADDR'];
        } else if (array_key_exists('HTTP_CLIENT_IP', $_SERVER)) {
            return $_SERVER['HTTP_CLIENT_IP'];
        }

        return '';
    }

    protected function fullName()
    {
        return $this->user["f_last"] . " " . $this->user["f_first"];
    }

    protected function fullShort()
    {
        return $this->user["f_last"] . " " .  mb_substr($this->user["f_first"], 0, 1) . ".";
    }
}
