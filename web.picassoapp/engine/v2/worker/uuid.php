<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 19:20:40
# Last Modified: 2025-05-25 19:20:43

function uuid_v4()
{
    $data = random_bytes(16);
    $data[6] = chr(ord($data[6]) & 0x0f | 0x40);
    $data[8] = chr(ord($data[8]) & 0x3f | 0x80);
    return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
}
