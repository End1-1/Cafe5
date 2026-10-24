<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-29 01:06:49
# Last modified - 2025-03-29 01:06:53
class WSClient
{
    private $host;
    private $port;
    private $socket;

    public function __construct($host = '127.0.0.1', $port = 10002)
    {
        $this->host = $host;
        $this->port = $port;
    }

    public function connect()
    {
        $this->socket = @fsockopen($this->host, $this->port, $errno, $errstr, 5);
        if (!$this->socket) {
            $this->socket = null;
            return false;
        }
        stream_set_timeout($this->socket, 10);
        stream_set_blocking($this->socket, true);
        return true;
    }


    public function handshake()
    {
        $key = base64_encode(random_bytes(16));

        $headers = [
            "GET / HTTP/1.1",
            "Host: {$this->host}:{$this->port}",
            "Upgrade: websocket",
            "Connection: Upgrade",
            "Sec-WebSocket-Key: $key",
            "Sec-WebSocket-Version: 13"
        ];

        $request = implode("\r\n", $headers) . "\r\n\r\n";
        fwrite($this->socket, $request);

        $response = fread($this->socket, 2000);

        // 1) Проверка 101
        if (strpos($response, "101") === false) {
            throw new Exception("Handshake failed: $response");
        }

        // 2) Проверка Sec-WebSocket-Accept
        $expectedAccept = base64_encode(
            sha1($key . "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", true)
        );

        if (!preg_match("/Sec-WebSocket-Accept:\s*(.*)\r/i", $response, $m)) {
            throw new Exception("No Sec-WebSocket-Accept in response");
        }

        $serverAccept = trim($m[1]);

        if ($serverAccept !== $expectedAccept) {
            throw new Exception("Bad WS Accept key: $serverAccept != $expectedAccept");
        }
    }


    public function sendMessage($message)
    {
        if (!$this->isOpen()) {
            return;
        }
        try {
            @fwrite($this->socket, $this->frameMessage($message));
        } catch (Throwable $e) {
            $this->dropSocket();
        }
    }

    public function sendJson($arr)
    {
        $this->sendMessage(json_encode($arr, JSON_UNESCAPED_UNICODE));
    }

    public function receiveJson()
    {
        $msg = $this->receiveMessage();
        return $msg ? json_decode($msg, true) : null;
    }


    public function receiveMessage()
    {
        $data = fread($this->socket, 2048);
        if (!$data) return null;

        $opcode = ord($data[0]) & 0x0F;

        // PING
        if ($opcode == 0x9) {
            fwrite($this->socket, chr(0x8A) . chr(0x00));
            return null;
        }

        // CLOSE — peer already shutting down; just drop local handle.
        if ($opcode == 0x8) {
            $this->dropSocket();
            return null;
        }

        // Only text frames
        if ($opcode != 0x1) return null;

        $length = ord($data[1]) & 127;
        $offset = 2;

        if ($length == 126) {
            $length = unpack('n', substr($data, 2, 2))[1];
            $offset = 4;
        } elseif ($length == 127) {
            $arr = unpack('N2', substr($data, 2, 8));
            $length = ($arr[1] << 32) | $arr[2];
            $offset = 10;
        }

        $masked = (ord($data[1]) & 0x80) != 0;
        if ($masked) {
            $mask = substr($data, $offset, 4);
            $offset += 4;
        }

        $payload = substr($data, $offset);
        $remaining = $length - strlen($payload);

        if ($remaining > 0) {
            $payload .= $this->readExact($remaining);
        }

        if ($masked) {
            $decoded = '';
            for ($i = 0; $i < $length; $i++) {
                $decoded .= $payload[$i] ^ $mask[$i % 4];
            }
            return $decoded;
        }

        return $payload;
    }



    public function ping()
    {
        if (!$this->isOpen()) {
            return;
        }
        try {
            @fwrite($this->socket, chr(0x89) . chr(0x00));
        } catch (Throwable $e) {
            $this->dropSocket();
        }
    }

    public function close()
    {
        if (!$this->isOpen()) {
            $this->socket = null;
            return;
        }
        try {
            // Opcode 0x8 close frame — may fail if peer already closed.
            @fwrite($this->socket, "\x88\x00");
        } catch (Throwable $e) {
            // ignore broken pipe / invalid stream
        }
        $this->dropSocket();
    }

    private function isOpen(): bool
    {
        if ($this->socket === null) {
            return false;
        }
        // PHP 8: closed streams are still typed but is_resource() becomes false
        return is_resource($this->socket);
    }

    private function dropSocket(): void
    {
        if ($this->socket !== null && is_resource($this->socket)) {
            try {
                @fclose($this->socket);
            } catch (Throwable $e) {
                // ignore
            }
        }
        $this->socket = null;
    }

    private function readExact($len)
    {
        $data = '';
        while (strlen($data) < $len) {
            $chunk = fread($this->socket, $len - strlen($data));
            if ($chunk === false || $chunk === '') break;
            $data .= $chunk;
        }
        return $data;
    }


    private function frameMessage($payload)
    {
        $length = strlen($payload);
        $mask = random_bytes(4);
        $masked = '';

        for ($i = 0; $i < $length; $i++) {
            $masked .= $payload[$i] ^ $mask[$i % 4];
        }

        $frame = chr(0x81); // FIN + TEXT

        if ($length <= 125) {
            $frame .= chr(0x80 | $length);
        } elseif ($length <= 65535) {
            $frame .= chr(0x80 | 126) . pack('n', $length);
        } else {
            $frame .= chr(0x80 | 127) . pack('NN', 0, $length);
        }

        return $frame . $mask . $masked;
    }
}
