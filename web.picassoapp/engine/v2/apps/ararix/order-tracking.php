<?php
# Server-to-server order tracking for QRNeto.
# Guest QR page keeps /ararix/order-status/by-token.
# Kitchen line status is the same scale as Waiter DlgKitchenInProgressFine:
#   1 ACCEPTED, 2 COOKING, 3 READY, 4 SERVED.
# Order status is the minimum among live lines (the kitchen card color).

require_once __DIR__ . "/../../worker/auth.php";

class OrderTracking extends Auth
{
    private const PARTNER = "qrneto";
    private const TOKEN_TTL_SECONDS = 86400;
    private const POLL_INTERVAL_SECONDS = 5;
    private const TZ = "Asia/Yerevan";

    /**
     * GET /engine/v2/ararix/order-tracking/by-token?token=
     * Authorization: Bearer <partner API token>
     */
    public function ByToken($params)
    {
        global $bearerToken;
        $this->assertPartner($bearerToken ?? "");

        $token = trim((string)($params->token ?? ""));
        if ($token === "" || !preg_match('/^[a-f0-9]{32,64}$/i', $token)) {
            $this->fail(404, "ORDER_NOT_FOUND", "Order not found");
        }

        $header = $this->select(
            <<<EOD
            SELECT oh.f_id, oh.f_state, oh.f_prefix, oh.f_amounttotal, oh.f_data,
                   oh.f_public_token, oh.f_datecash, oh.f_timeclose
            FROM o_header oh
            WHERE oh.f_public_token = ?
            LIMIT 1
            EOD,
            "s",
            [$token]
        )->fetch_assoc();

        if (!$header) {
            $this->fail(404, "ORDER_NOT_FOUND", "Order not found");
        }

        $odata = json_decode($header["f_data"] ?? "{}", true);
        if (!is_array($odata)) {
            $odata = [];
        }

        $closedAt = $this->dateTimeFromParts(
            (string)($odata["f_date_close"] ?? $header["f_datecash"] ?? ""),
            (string)($odata["f_time_close"] ?? $header["f_timeclose"] ?? "")
        );
        if ($closedAt instanceof DateTime && ($closedAt->getTimestamp() + self::TOKEN_TTL_SECONDS) < time()) {
            $this->fail(410, "TRACKING_EXPIRED", "Tracking expired");
        }

        $this->assertPollInterval($token);

        $rows = $this->select(
            <<<EOD
            SELECT og.f_id, og.f_goods, og.f_qty, og.f_price, og.f_total, og.f_state,
                   og.f_parent, og.f_row, og.f_data,
                   g.f_name, g.f_scancode,
                   COALESCE(ogp.f_status, 1) AS f_process_status,
                   ogp.f_data AS f_process_data
            FROM o_goods og
            LEFT JOIN c_goods g ON g.f_id = og.f_goods
            LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
            WHERE og.f_header = ?
            ORDER BY og.f_row
            EOD,
            "s",
            [$header["f_id"]]
        )->fetch_all(MYSQLI_ASSOC);

        $payload = $this->buildPayload($header, $odata, $rows, $closedAt);
        http_response_code(200);
        header("Content-Type: application/json; charset=utf-8");
        echo json_encode($payload, JSON_UNESCAPED_UNICODE);
        exit;
    }

    /**
     * Logged-in ArariX user rotates the QRNeto key. Plain token is returned once.
     * POST /engine/v2/ararix/order-tracking/issue-key
     */
    public function IssueKey($params)
    {
        unset($params);
        $token = bin2hex(random_bytes(32));
        $hash = hash("sha256", $token);
        $existing = $this->select(
            "SELECT f_id FROM external_api_key WHERE f_partner = ? LIMIT 1",
            "s",
            [self::PARTNER]
        )->fetch_assoc();
        if ($existing) {
            $this->update("external_api_key", [
                "f_token_hash" => $hash,
                "f_enabled" => 1,
            ], (int)$existing["f_id"]);
        } else {
            $this->insert("external_api_key", [
                "f_partner" => self::PARTNER,
                "f_token_hash" => $hash,
                "f_enabled" => 1,
            ]);
        }
        $this->result = [
            "partner" => self::PARTNER,
            "token" => $token,
        ];
        $this->echoResult();
    }

    private function assertPartner(string $bearer): void
    {
        if ($bearer === "") {
            $this->fail(401, "UNAUTHORIZED", "Unauthorized");
        }
        $hash = hash("sha256", trim($bearer));
        $row = $this->select(
            "SELECT f_id FROM external_api_key WHERE f_enabled = 1 AND f_token_hash = ? LIMIT 1",
            "s",
            [$hash]
        )->fetch_assoc();
        if (!$row) {
            $this->fail(403, "FORBIDDEN", "Forbidden");
        }
    }

    private function assertPollInterval(string $token): void
    {
        $row = $this->select(
            "SELECT f_last_poll FROM order_tracking_poll WHERE f_token = ? LIMIT 1",
            "s",
            [$token]
        )->fetch_assoc();
        if ($row) {
            $last = strtotime((string)$row["f_last_poll"]);
            if ($last !== false && (time() - $last) < self::POLL_INTERVAL_SECONDS) {
                $this->fail(429, "TOO_MANY_REQUESTS", "Too many requests");
            }
            $this->select(
                "UPDATE order_tracking_poll SET f_last_poll = NOW() WHERE f_token = ?",
                "s",
                [$token],
                true
            );
            return;
        }
        $this->insert("order_tracking_poll", [
            "f_token" => $token,
            "f_last_poll" => date("Y-m-d H:i:s"),
        ]);
    }

    private function buildPayload(array $header, array $odata, array $rows, ?DateTime $closedAt): array
    {
        $parents = [];
        $children = [];
        foreach ($rows as $row) {
            if ((int)($row["f_state"] ?? 0) !== 1) {
                continue;
            }
            $parent = trim((string)($row["f_parent"] ?? ""));
            if ($parent === "" || $parent === "0") {
                $parents[] = $row;
            } else {
                $children[$parent][] = $row;
            }
        }

        $items = [];
        $statuses = [];
        $statusTimes = [];
        foreach ($parents as $row) {
            $lineData = json_decode($row["f_data"] ?? "{}", true);
            if (!is_array($lineData)) {
                $lineData = [];
            }
            $process = json_decode($row["f_process_data"] ?? "{}", true);
            if (!is_array($process)) {
                $process = [];
            }
            $kitchen = (int)($row["f_process_status"] ?? 1);
            if ($kitchen <= 0) {
                $kitchen = 1;
            }
            $statuses[] = $kitchen;
            $this->collectStatusTimes($process, $statusTimes);
            $lineTimeline = $this->kitchenTimeline($process);

            $modifiers = $this->modifiersFromData($lineData);
            foreach ($children[$row["f_id"]] ?? [] as $child) {
                $modifiers[] = [
                    "name" => (string)($child["f_name"] ?? ""),
                    "quantity" => (float)($child["f_qty"] ?? 0),
                    "price" => (float)($child["f_price"] ?? 0),
                ];
            }

            $items[] = [
                "item_id" => (string)$row["f_id"],
                "sku" => (string)($row["f_scancode"] ?? ""),
                "name" => (string)($row["f_name"] ?? ""),
                "quantity" => (float)($row["f_qty"] ?? 0),
                "unit_price" => (float)($row["f_price"] ?? 0),
                "total" => (float)($row["f_total"] ?? 0),
                "modifiers" => $modifiers,
                "notes" => (string)($lineData["f_comment"] ?? ""),
                "kitchen_status" => $this->kitchenStatusCode($kitchen),
                "kitchen_status_code" => $kitchen,
                "timeline" => $lineTimeline,
            ];
        }

        $headerState = (int)($header["f_state"] ?? 0);
        $kitchenMin = empty($statuses) ? 1 : min($statuses);
        $status = $this->orderStatus($headerState, $kitchenMin, empty($statuses));

        $createdAt = $this->isoFromParts(
            (string)($odata["f_date_open"] ?? ""),
            (string)($odata["f_time_open"] ?? "")
        );
        $closedIso = $closedAt instanceof DateTime ? $closedAt->format("c") : null;
        $orderTimeline = $this->orderTimeline($createdAt, $closedIso, $headerState);
        $kitchenTimeline = $this->kitchenTimelineFromTimes($statusTimes);
        $updatedAt = $createdAt;
        foreach (array_merge($orderTimeline, $kitchenTimeline) as $event) {
            if ($updatedAt === null || strcmp($event["timestamp"], $updatedAt) > 0) {
                $updatedAt = $event["timestamp"];
            }
        }

        $etaMinutes = (int)($odata["f_eta_minutes"] ?? 0);
        $estimated = null;
        if ($createdAt !== null && $etaMinutes > 0) {
            $created = $this->dateTimeFromIso($createdAt);
            if ($created instanceof DateTime) {
                $created->modify("+" . $etaMinutes . " minutes");
                $estimated = $created->format("c");
            }
        }

        return [
            "order_id" => (string)$header["f_id"],
            "order_number" => (string)($header["f_prefix"] ?? ""),
            "restaurant_id" => (string)($odata["f_qrneto_restaurant_id"] ?? ""),
            "location_id" => (string)($odata["f_qrneto_location_id"] ?? ""),
            "source" => (string)($odata["f_order_source"] ?? ""),
            "status" => $status,
            "header_state" => $this->headerStateCode($headerState),
            "header_state_code" => $headerState,
            "kitchen_status" => $this->kitchenStatusCode($kitchenMin),
            "kitchen_status_code" => $kitchenMin,
            "created_at" => $createdAt,
            "updated_at" => $updatedAt,
            "estimated_ready_at" => $estimated,
            "items" => $items,
            "totals" => [
                "subtotal" => (float)($odata["f_sub_total"] ?? 0),
                "discount" => (float)($odata["f_discount_amount"] ?? 0),
                "service" => (float)($odata["f_service_amount"] ?? 0),
                "tax" => 0,
                "total" => (float)($header["f_amounttotal"] ?? 0),
                "currency" => "AMD",
            ],
            "order_timeline" => $orderTimeline,
            "kitchen_timeline" => $kitchenTimeline,
        ];
    }

    private function modifiersFromData(array $lineData): array
    {
        $raw = $lineData["f_modificators"] ?? [];
        if (!is_array($raw)) {
            return [];
        }
        $out = [];
        foreach ($raw as $mod) {
            if (!is_array($mod)) {
                continue;
            }
            $out[] = [
                "name" => (string)($mod["f_name"] ?? ""),
                "quantity" => 1,
                "price" => (float)($mod["f_price"] ?? 0),
            ];
        }
        return $out;
    }

    private function collectStatusTimes(array $process, array &$statusTimes): void
    {
        for ($st = 1; $st <= 4; $st++) {
            for ($ss = 1; $ss <= 4; $ss++) {
                $raw = trim((string)($process["f_status_{$st}_{$ss}_time"] ?? ""));
                if ($raw === "") {
                    continue;
                }
                $iso = $this->isoFromParts($raw, "");
                if ($iso === null) {
                    continue;
                }
                $code = $this->kitchenStatusCode($st);
                if (!isset($statusTimes[$code]) || strcmp($iso, $statusTimes[$code]) < 0) {
                    $statusTimes[$code] = $iso;
                }
            }
        }
    }

    private function orderTimeline(?string $createdAt, ?string $closedIso, int $headerState): array
    {
        $events = [];
        if ($createdAt !== null && $headerState !== 3) {
            $events[] = ["status" => "OPEN", "timestamp" => $createdAt];
        }
        if ($closedIso !== null && $headerState === 2) {
            $events[] = ["status" => "CLOSED", "timestamp" => $closedIso];
        }
        if ($headerState === 3) {
            $events[] = ["status" => "VOID", "timestamp" => $closedIso ?? $createdAt ?? $this->nowIso()];
        }
        return $events;
    }

    private function kitchenTimeline(array $process): array
    {
        $times = [];
        $this->collectStatusTimes($process, $times);
        return $this->kitchenTimelineFromTimes($times);
    }

    private function kitchenTimelineFromTimes(array $statusTimes): array
    {
        $events = [];
        foreach (["ACCEPTED", "COOKING", "READY", "SERVED"] as $code) {
            if (!empty($statusTimes[$code])) {
                $events[] = ["status" => $code, "timestamp" => $statusTimes[$code]];
            }
        }
        return $events;
    }

    /** Same rule as DlgKitchenInProgressFine::minStatusInLines + card color. */
    private function orderStatus(int $headerState, int $kitchenMin, bool $noLines): string
    {
        if ($headerState === 3 || $kitchenMin === 6) {
            return "VOID";
        }
        if ($headerState === 4) {
            return "EMPTY";
        }
        if ($headerState === 6) {
            return "MOVED";
        }
        if ($noLines && $headerState === 5) {
            return "PREORDER";
        }
        if ($noLines && $headerState === 2) {
            return "SERVED";
        }
        return $this->kitchenStatusCode($kitchenMin);
    }

    private function kitchenStatusCode(int $status): string
    {
        switch ($status) {
            case 2:
                return "COOKING";
            case 3:
                return "READY";
            case 4:
                return "SERVED";
            case 1:
            default:
                return "ACCEPTED";
        }
    }

    private function headerStateCode(int $state): string
    {
        switch ($state) {
            case 1:
                return "OPEN";
            case 2:
                return "CLOSED";
            case 3:
                return "VOID";
            case 4:
                return "EMPTY";
            case 5:
                return "PREORDER";
            case 6:
                return "MOVED";
            default:
                return "NONE";
        }
    }

    private function isoFromParts(string $date, string $time): ?string
    {
        $dt = $this->dateTimeFromParts($date, $time);
        if (!$dt instanceof DateTime || (int)$dt->format("Y") < 2000) {
            return null;
        }
        return $dt->format("c");
    }

    private function dateTimeFromParts(string $date, string $time): ?DateTime
    {
        $date = trim($date);
        $time = trim($time);
        if ($date === "" && $time === "") {
            return null;
        }
        $raw = $time === "" ? $date : ($date === "" ? $time : $date . " " . $time);
        $tz = new DateTimeZone(self::TZ);
        $formats = ["Y-m-d H:i:s", "Y-m-d H:i", "Y-m-d", "d/m/Y H:i:s", "d/m/Y H:i", "d/m/Y"];
        foreach ($formats as $fmt) {
            $dt = DateTime::createFromFormat($fmt, $raw, $tz);
            $errors = DateTime::getLastErrors();
            $clean = $errors === false
                || (((int)($errors["warning_count"] ?? 0) === 0) && ((int)($errors["error_count"] ?? 0) === 0));
            if ($dt instanceof DateTime && $clean) {
                return $dt;
            }
        }
        return null;
    }

    private function dateTimeFromIso(string $iso): ?DateTime
    {
        try {
            return new DateTime($iso);
        } catch (Exception $e) {
            return null;
        }
    }

    private function nowIso(): string
    {
        return (new DateTime("now", new DateTimeZone(self::TZ)))->format("c");
    }

    private function fail(int $http, string $code, string $message): void
    {
        http_response_code($http);
        header("Content-Type: application/json; charset=utf-8");
        echo json_encode([
            "error" => [
                "code" => $code,
                "message" => $message,
            ],
        ], JSON_UNESCAPED_UNICODE);
        exit;
    }
}
