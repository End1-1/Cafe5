<?php
# © 2026 , Kudryashov Vasili
# Public JSON API for guest order status by unguessable f_public_token.
# Called by external landing sites (QR opens their page; page fetches this API).

require_once __DIR__ . "/../../worker/db.php";
require_once __DIR__ . "/../../worker/helper.php";

class OrderStatus extends Db
{
    public function __construct()
    {
        parent::__construct();
    }

    /** Always allow — access is gated by the token entropy. */
    public function auth()
    {
        return true;
    }

    /**
     * GET/POST /ararix/order-status/by-token
     * Body/query: { "token": "..." , "locale": "hy|ru|en" optional }
     *
     * Stable kitchen_status codes for UI:
     *   0 pending, 1 accepted, 2 cooking, 3 ready, 4 served
     */
    public function ByToken($params)
    {
        $token = trim((string)($params->token ?? ""));
        if ($token === "" || !preg_match('/^[a-f0-9]{32,64}$/i', $token)) {
            dieWithCode(Translator::t("Order not found"), 404);
        }

        $header = $this->select(
            <<<EOD
            SELECT oh.f_id, oh.f_state, oh.f_prefix, oh.f_amounttotal, oh.f_data, oh.f_public_token,
                   oh.f_datecash, oh.f_timeclose,
                   h.f_name AS f_hall_name, t.f_name AS f_table_name
            FROM o_header oh
            LEFT JOIN h_tables t ON t.f_id = oh.f_table
            LEFT JOIN h_halls h ON h.f_id = t.f_hall
            WHERE oh.f_public_token = ?
            LIMIT 1
            EOD,
            "s",
            [$token]
        )->fetch_assoc();

        if (!$header) {
            dieWithCode(Translator::t("Order not found"), 404);
        }

        $odata = json_decode($header["f_data"] ?? "{}", true);
        if (!is_array($odata)) {
            $odata = [];
        }

        $dishes = $this->select(
            <<<EOD
            SELECT og.f_qty, og.f_price, og.f_total, og.f_state AS f_line_state,
                   g.f_name AS f_name,
                   COALESCE(ogp.f_status, 0) AS f_process_status
            FROM o_goods og
            LEFT JOIN c_goods g ON g.f_id = og.f_goods
            LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
            WHERE og.f_header = ?
              AND og.f_state = 1
              AND (og.f_parent IS NULL OR og.f_parent = '')
            ORDER BY og.f_row
            EOD,
            "s",
            [$header["f_id"]]
        )->fetch_all(MYSQLI_ASSOC);

        $items = [];
        $minKitchen = null;
        foreach ($dishes as $d) {
            $ps = (int)($d["f_process_status"] ?? 0);
            if ($ps > 0) {
                $minKitchen = $minKitchen === null ? $ps : min($minKitchen, $ps);
            }
            $items[] = [
                "name" => (string)($d["f_name"] ?? ""),
                "qty" => (float)($d["f_qty"] ?? 0),
                "price" => (float)($d["f_price"] ?? 0),
                "total" => (float)($d["f_total"] ?? 0),
                "kitchen_status" => $ps,
                "kitchen_status_code" => $this->kitchenStatusCode($ps),
                "kitchen_status_name" => $this->kitchenStatusName($ps),
            ];
        }

        $kitchenStatus = (int)($minKitchen ?? 0);
        $orderState = (int)($header["f_state"] ?? 0);
        $this->result = [
            "status" => 1,
            "order" => [
                "token" => (string)$header["f_public_token"],
                "number" => (string)($header["f_prefix"] ?? ""),
                "state" => $orderState,
                "state_name" => $this->orderStateName($orderState),
                "amount_total" => (float)($header["f_amounttotal"] ?? 0),
                "hall_name" => (string)($header["f_hall_name"] ?? ""),
                "table_name" => (string)($header["f_table_name"] ?? ""),
                "date_open" => (string)($odata["f_date_open"] ?? ""),
                "time_open" => (string)($odata["f_time_open"] ?? ""),
                "date_close" => (string)($odata["f_date_close"] ?? ($header["f_datecash"] ?? "")),
                "time_close" => (string)($odata["f_time_close"] ?? ($header["f_timeclose"] ?? "")),
                "kitchen_status" => $kitchenStatus,
                "kitchen_status_code" => $this->kitchenStatusCode($kitchenStatus),
                "kitchen_status_name" => $this->kitchenStatusName($kitchenStatus),
                "items" => $items,
            ],
        ];
        $this->echoResult();
    }

    private function orderStateName(int $state): string
    {
        switch ($state) {
            case 1:
                return Translator::t("In progress");
            case 2:
                return Translator::t("Closed");
            case 3:
                return Translator::t("Cancelled");
            case 5:
                return Translator::t("Preorder");
            default:
                return Translator::t("Unknown");
        }
    }

    /** Machine-stable codes for external landing UIs. */
    private function kitchenStatusCode(int $status): string
    {
        switch ($status) {
            case 1:
                return "accepted";
            case 2:
                return "cooking";
            case 3:
                return "ready";
            case 4:
                return "served";
            default:
                return "pending";
        }
    }

    private function kitchenStatusName(int $status): string
    {
        switch ($status) {
            case 1:
                return Translator::t("Accepted");
            case 2:
                return Translator::t("Cooking");
            case 3:
                return Translator::t("Ready");
            case 4:
                return Translator::t("Served");
            default:
                return Translator::t("Pending");
        }
    }
}
