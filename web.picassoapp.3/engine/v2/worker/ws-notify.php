<?php
# (C) 2026 Kudryashov Vasili 
# Created - 2025-03-29 01:06:49
# Last modified - 2026-04-19 19:35:42
// Основная функция-обертка
require_once __DIR__ . "/../../cnf.php";
require_once __DIR__ . "/../../functions/ws.php";
$ws_worker = new class {
    private function getWS()
    {
        global $websocketaddress, $websocketport, $websocketuser, $websocketpass, $websocketkey;
        $ws = new WSClient($websocketaddress, $websocketport);
        if (!$ws->connect()) return null;
        $ws->handshake();
        $ws->sendJson([
            "command"  => "register_socket",
            "key"      => $websocketkey,
            "username" => $websocketuser,
            "password" => $websocketpass
        ]);
        $ack = $ws->receiveJson();
        if (!$ack || ($ack["status"] ?? 0) != 1) {
            $ws->close();
            return null;
        }
        return $ws;
    }

    // Твоя старая функция (оставлена как есть)
    public function notify(string $dict, int $id, bool $isnew)
    {
        $ws = $this->getWS();
        if (!$ws) return false;
        $ws->sendJson([
            "command" => "dict_event",
            "entity"  => $dict,
            "op"      => $isnew ? "i" : "u",
            "id"      => $id,
            "ts"      => time()
        ]);
        $ws->close();
        return true;
    }

    // Новая функция специально для цен (принимает строку данных)
    public function updatePrices($items)
    {
        $ws = $this->getWS();
        if (!$ws) return false;
        $ws->sendJson([
            "command" => "update_goods_last_input_prices",
            "data"    => json_encode($items, JSON_UNESCAPED_UNICODE),
            "ts"      => time()
        ]);
        $ws->close();
        return true;
    }
};

// Возвращаем объект, чтобы можно было вызывать методы
return $ws_worker;
