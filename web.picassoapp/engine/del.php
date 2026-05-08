
<?php
$serverKey = 'AIzaSyAf4_GEcJzwdZXE1ISZ1-sFS0t_FVx4MEw';
$deviceToken = 'ctzaijFbSF2fHd5FVlY3bA:APA91bFfmGpTOMCbgbTI5zDyq_mWoiV4wLAwIsx0f9eBq9VQ2A3kYw-YXl1jStb9jKiI2-ph_1wD6XWGDz4ify7Jc-ulsIuPYIApjjPCr1gZH7WyPhORTnQ'; 
$pid = "wagon-a7dd0";
$data = [
    "to" => $deviceToken,
    "data" => [
        "title" => "Новое уведомление",
        "body" => "Выберите действие",
        "action1" => "accept",
        "action2" => "decline"
    ],
];

$headers = [
    'Authorization: key=' . $serverKey,
    'Content-Type: application/json'
];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, 'https://fcm.googleapis.com/v1/projects/$pid/messages:send');
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));

$result = curl_exec($ch);
if (curl_errno($ch)) {
    echo 'CURL Error: ' . curl_error($ch);
}
curl_close($ch);

echo $result;
?>
