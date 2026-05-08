<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 19:20:40
# Last Modified: 2025-05-25 19:20:43

function dieWithCode($message, $code = 500)
{
    http_response_code($code);
    header("Content-Type:  text/plain; charset=utf-8");
    die($message);
}