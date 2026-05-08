<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-27 12:29:24
# Last Modified: 2026-01-27 12:29:27
require_once __DIR__ . "/index.php";


class Documents extends Auth {
    public function SaveStoreDocument($params) {
       
$session = $sessionId;
$err = null;

try {
    // 1️⃣ Call procedure
    $stmt = $pdo->prepare("CALL sf_create_store_document(:json)");
    if (!$stmt->execute([':json' => $json])) {
        $err = $stmt->errorInfo()[2];
        return;
    }

    // MySQL требует закрыть result set после CALL
    while ($stmt->nextRowset()) {}

    // 2️⃣ Get result
    $stmt = $pdo->prepare("SELECT f_result FROM a_result WHERE f_session = :session");
    $stmt->execute([':session' => $session]);

    $row = $stmt->fetch(PDO::FETCH_ASSOC);
    if (!$row) {
        $err = "No result row for session $session";
        return;
    }

    // 3️⃣ Parse JSON
    $js = json_decode($row['f_result'], true);
    if (!$js) {
        $err = "Invalid JSON: " . json_last_error_msg();
        return;
    }

    // 4️⃣ Check status
    if (($js['status'] ?? 0) == 0) {
        $err = $js['message'] ?? 'Unknown error';
        return;
    }

} catch (PDOException $e) {
    $err = $e->getMessage();
}

    }
}