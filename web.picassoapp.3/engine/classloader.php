<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-30 11:22:18
# Last modified - 2025-03-30 11:22:20
require_once __DIR__ . "/app.php";

if (empty($params->class)) {
    exitError("O, drop it like it hot!");
}

$fileName = $_SERVER['DOCUMENT_ROOT'] . "/engine/" . CLASS_SUBDIR . "/" . $params->class . ".php";
if (!file_exists($fileName)) {
    exitError("O, drop it like it not hot!");
}
if (!in_array($params->class, $CLASSES)) {
    exitError("Forbidden class {$params->class}");
}
require_once $fileName;
$c = new $params->class();
if (!method_exists($c, $params->method)) {
    exitError("No such method {$params->method}");
}
$c->{$params->method}();
