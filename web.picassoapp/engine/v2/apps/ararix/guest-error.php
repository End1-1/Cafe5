<?php
# Machine-readable guest API errors for booklet / QRNeto clients.

function guest_fail(int $http, string $code, string $message, array $details = []): void
{
    http_response_code($http);
    header("Content-Type: application/json; charset=utf-8");
    $payload = [
        "error" => [
            "code" => $code,
            "message" => $message,
        ],
    ];
    if ($details !== []) {
        $payload["error"]["details"] = $details;
    }
    echo json_encode($payload, JSON_UNESCAPED_UNICODE);
    exit;
}
