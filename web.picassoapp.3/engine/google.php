<?php

define("APP", 1);
require_once __DIR__ . "/functions/common.php";

$google_api_key = "????" ; // get it from cnf.php
$baseURL = "https://maps.googleapis.com/maps/api";

if (isset($_REQUEST["api"])) {
    $url = "$baseURL/js?key={$google_api_key}";
    header("Content-Type: application/javascript");
}

if (isset($_REQUEST["geocode"])) {
    $url = "$baseURL/geocode/json?latlng={$_GET["latlng"]}&key=$google_api_key";
}

if (isset($_REQUEST["pointofaddress"])) {
   $address = urlencode($_GET["address"]);
   $url="$baseURL/geocode/json?address=$address&key=$google_api_key";
}

if (isset($_REQUEST["autocomplete"])) {
    $radius = 20000;
    $components = "country:AM";
    $location = '40.1792,44.4991';
    $sessionToken = $_GET["sessionToken"];
    $input = $_GET["input"];
    $url = "$baseURL/place/autocomplete/json?input=" . urlencode($input) . "&key=$google_api_key&sessiontoken=$sessionToken"
        . "&location=$location"
        . "&radius=$radius"
        . "&components=$components"
        . "&types=address";
}

if (empty($url)) {
    http_response_code(500);
    echo $url;
    exit("request type missing");
}

$response = file_get_contents($url);
if ($response !== false) {
    header('Content-Type: application/json');
    echo $response;
} else {
    http_response_code(500);
    echo json_encode(['error' => 'Failed to fetch data from Google API']);
}