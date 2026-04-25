<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 13:31:11
# Last Modified: 2026-03-28 11:47:17

$allowedOrigins = ['http://localhost:5173'];

$origin = $_SERVER['HTTP_ORIGIN'] ?? '';
if (in_array($origin, $allowedOrigins)) {
    header('Access-Control-Allow-Origin: ' . $origin);
    header('Access-Control-Allow-Credentials: true');
    header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
    header('Access-Control-Allow-Headers: Content-Type, Authorization');
} else {
    //  die("ORIGIN '$origin' NOT ALLOWED!!!");
}

const DEBUG = true;
define("LANG", "hy");

error_reporting(E_ALL);
ini_set("display_errors", "On");
ini_set("memory_limit", "4512M");
date_default_timezone_set("Asia/Yerevan");

require_once __DIR__ . "/check-app.php";
require_once __DIR__ . "/../cnf.php";
require_once __DIR__ . "/worker/die-with-code.php";
require_once __DIR__ . "/worker/validate.php";
require_once __DIR__ . "/worker/translator.php";

# Get bearer token from Authorization header
$bearerToken = "";
if (isset($_SERVER['HTTP_AUTHORIZATION'])) {

    $authHeader = $_SERVER['HTTP_AUTHORIZATION'];
} elseif (function_exists('getallheaders')) {

    $headers = getallheaders();
    $authHeader = $headers['Authorization'] ?? $headers['authorization'] ?? null;
}

if (isset($authHeader) && preg_match('/Bearer\s(\S+)/i', $authHeader, $matches)) {
    $bearerToken = $matches[1];
}

# Set all warnings to be treated as errors
function stacktrace_error_handler($errno, $message, $file, $line)
{
    if ($errno === E_WARNING) {
        debug_print_backtrace();
    }
    return false;
}
set_error_handler("stacktrace_error_handler");
# Correct error generation
register_shutdown_function(function () {
    $error = error_get_last();
    if ($error !== null && in_array($error['type'], [E_ERROR, E_PARSE, E_CORE_ERROR, E_COMPILE_ERROR])) {
        if (!headers_sent()) {
            http_response_code(500);
            header('Content-Type: text/plain'); // или application/json
        }
        echo "Fatal error: {$error['message']} in {$error['file']} on line {$error['line']}";
    }
});

function toPascalCase($str)
{
    return str_replace(' ', '', ucwords(str_replace(['-', '_'], ' ', $str)));
}

$requestUri = $_SERVER['REQUEST_URI'];
$scriptName = $_SERVER['SCRIPT_NAME'];
$basePath = dirname($scriptName);
$path = substr($requestUri, strlen($basePath));
$pathArray = explode("/", $path);
array_shift($pathArray);


if (count($pathArray) < 3) {
    dieWithCode("Invalid request path. Expected format: /app/controller/action");
}

$includeFileName = __DIR__ . "/apps/" . $pathArray[0] . "/" . $pathArray[1] . ".php";
if (!file_exists($includeFileName)) {
    dieWithCode("File not found: apps/{$pathArray[0]}/{$pathArray[1]}", 404);
}

$remote_host = $_SERVER['HTTP_X_APPLICATION_HOST'] ?? "host-undefined";
require_once $includeFileName;
$controllerName = toPascalCase($pathArray[1]);

if (!class_exists($controllerName)) {
    dieWithCode("Class not found: {$controllerName}");
}

$controller = new $controllerName();
$methodName = toPascalCase($pathArray[2]);
if (!method_exists($controller, $methodName)) {
    print_r($pathArray);
    dieWithCode("Method not found: {$methodName}");
}

if ($methodName != "Login" && $methodName != "CheckOtp" && $methodName != "PinLogin" && $methodName != "HashLogin") {
    if (!$controller->auth()) {
        dieWithCode("Unauthorized", 401);
    }
}


if (!empty($_FILES) || isset($_POST['data'])) {
    $jsonString = $_POST['data'] ?? '{}';
    $jsonParams = json_decode($jsonString);

    if (json_last_error() !== JSON_ERROR_NONE) {
        dieWithCode("Invalid JSON in multipart 'data' field: $jsonString", 400);
    }

    $jsonParams->_files = $_FILES;
} else {

    $requestString = file_get_contents("php://input");
    $jsonParams = json_decode($requestString);

    if (json_last_error() !== JSON_ERROR_NONE) {
        dieWithCode("Invalid JSON in request body $requestString", 400);
    }
}

Translator::$locale = empty($jsonParams->locale) ? LANG : $jsonParams->locale;
$controller->{$methodName}($jsonParams);
