<?php
$secretCode = "123456"; // твой код

$msg = "";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $code = $_POST['code'] ?? '';

    if ($code === $secretCode) {
        exec("sudo /sbin/shutdown -P now > /dev/null 2>&1 &");
        $msg = "Wait...";
    } else {
        $msg = "Invalid code";
    }
}
?>

<!DOCTYPE html>
<html>

<head>
    <meta charset="utf-8">
    <title>Shutdown</title>
</head>

<body>
    <h2>Մուտք</h2>

    <form method="post">
        <input type="password" name="code" placeholder="Կոդ">
        <button type="submit">Выключить</button>
    </form>

    <p><?= htmlspecialchars($msg) ?></p>

</body>

</html>