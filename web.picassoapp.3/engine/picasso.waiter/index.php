<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-27 17:21:20
# Last modified - 2025-03-27 17:21:23

require_once __DIR__ . "/../app.php";

if (empty($params->class)) {
    exitError("O, drop it like it hot!");
}

$fileName = __DIR__ . "/" . $params->class . ".php";
if (!file_exists($fileName)) {
    exitError("O, drop it like it not hot!");
}
require_once $fileName;
$c = new $params->class();
if (!method_exists($c, $params->method)) {
    exitError("No such method {$params->method}");
}
$c->{$params->method}($params);