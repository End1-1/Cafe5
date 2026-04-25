<?php
defined('APP') or die('Die, vampire!');

function exitError($error, $code = 500) {
    http_response_code($code);
    header('Content-Type: application/json');
    echo json_encode(['error' => $error]);
    exit;
}

function uuidv4()
{
  $data = random_bytes(16);

  $data[6] = chr(ord($data[6]) & 0x0f | 0x40); // set version to 0100
  $data[8] = chr(ord($data[8]) & 0x3f | 0x80); // set bits 6-7 to 10
    
  return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
}

function printResult($code, $data) {
  echo json_encode(["status"=>$code, "data"=>$data], JSON_UNESCAPED_UNICODE);
}

function isValidEAN13($ean) {
  $ean = preg_replace('/\D/', '', $ean);
  if (strlen($ean) != 13) {
      return false;
  }
  $sum = 0;
  for ($i = 0; $i < 12; $i++) {
      $sum += $ean[$i] * (($i % 2 == 0) ? 1 : 3);
  }
  $checksum = (10 - ($sum % 10)) % 10;
  return $checksum == $ean[12];
}

function get_client_ip() {
  if (array_key_exists('HTTP_X_FORWARDED_FOR', $_SERVER)) {
    return  $_SERVER['HTTP_X_FORWARDED_FOR'];
  } else if (array_key_exists('REMOTE_ADDR', $_SERVER)) {
    return $_SERVER['REMOTE_ADDR'];
  } else if (array_key_exists('HTTP_CLIENT_IP', $_SERVER)) {
    return $_SERVER['HTTP_CLIENT_IP'];
  }

  return '';
}