<?php
require_once(__DIR__ . "/../app.php");

if(empty($params->config_id)) {
    exitError("No config id specified");
}
$config = stmtall("select f_config from sys_json_config where f_name=?", "s", [$params->config_id])->fetch_assoc();
if (empty($config)) {
    exitError("Settings id $params->config_id not exists");
} else {
    $config = json_decode($config["f_config"]);
}
