<?php
define("AUTH", 1);
require_once __DIR__ . "/shop.php";

class CreateOnline extends PClass
{
    private $endpoint = false;
    public function __construct()
    {
        parent::__construct();
        if (empty($this->params->key) || $this->params->key != "fae292deae2e11eb85290242ac130003") {
            $this->exitError("Are You Jennifer Lopez?");
        }
        $v["f_document"] = "create shop";
        $v["f_request"] = json_encode($this->params);
        $this->sinsert("a_result", $v);
    }
    public function create()
    {

        if (!$this->endpoint) {
            $url = "https://e3.picasso.am/engine/shop/create-online.php";
            $jsonData = json_encode((array) $this->params);
            $ch = curl_init($url);
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonData);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($ch, CURLOPT_HTTPHEADER, [
                'Content-Type: application/json',
                'Content-Length: ' . strlen($jsonData)
            ]);
            $response = curl_exec($ch);
            $response = json_decode($response);
            $this->result["data"] = $response->data;
            $this->result["uuid"] = $response->data->uuid;
            $this->result["order_id"] = $response->data->order_id;
            $this->echoResult();
            return;
        }

        $amountTotal = 0;
        //Check for multiple quantity and separate one by one
        $goodsSku = [];
        for ($i = 0; $i < count($this->params->order->items); $i++) {
            $item = $this->params->order->items[$i];
            $amountTotal += $item->qty * $item->price;
            array_push($goodsSku, $item->sku);
        }
        $amountTotal += $this->params->order->delivery;
        $goodsSkuCond = "'" . implode(",'", $goodsSku) . "'";
        $tmp = $this->stmtall("select f_id, f_scancode from c_goods where f_scancode in ($goodsSkuCond) ")->fetch_all(MYSQLI_NUM);
        $goodsSkuIdMap = [];
        foreach ($tmp as $t) {
            $goodsSkuIdMap[$t[1]] = $t[0];
        }

        $amountCash = $this->params->order->payment_type == 1 ? $amountTotal : 0;
        $amountCard = $this->params->order->payment_type == 2 ? $amountTotal : 0;
        $amountOther = $this->params->order->payment_type > 4 ? $amountTotal : 0;
        $amountIDram = $this->params->order->payment_type == 6 ? $amountTotal : 0;
        $amountTellCell = $this->params->order->payment_type == 7 ? $amountTotal : 0;


        $uuid = $this->uuidv4();
        $v["f_id"] = $uuid;
        $v["f_state"] = 1;
        $v["f_saletype"] = 1;
        $v["f_date"] = date("Y-m-d");
        $v["f_time"] = date("H:i:s");
        $v["f_staff"] = 1;
        $v["f_amount"] = $amountTotal;
        $v["f_payment"] = $this->params->order->payment_type;
        #$v["f_currency"] = 1;
        $v["f_comment"] = $this->tr("Delivery address") . ": "
            . $this->params->order->address->city
            . ", " . $this->params->order->address->address
            . ", " . $this->tr("phone") . ": "
            . $this->params->order->address->phone;
        $this->sinsert("o_draft_sale", $v);



        /* A_STORE_DRAFT, O_GOODS */
        for ($i = 0; $i < count($this->params->order->items); $i++) {
            $item = $this->params->order->items[$i];
            $goodsid = $this->uuidv4();
            $v["f_id"] = $goodsid;
            $v["f_header"] = $uuid;
            $v["f_state"] = 1;
            $v["f_dateappend"] = date("Y-m-d");
            $v["f_timeappend"] = date("H:i:s");
            $v["f_goods"] = $goodsSkuIdMap[$item->sku];
            $v["f_qty"] = $item->qty;
            $v["f_price"] = $item->price;
            $v["f_userappend"] = 1;
            $this->sinsert("o_draft_sale_body", $v);
        }

        if ($this->params->order->delivery > 0) {
            $goodsid = $this->uuidv4();
            $v["f_id"] = $goodsid;
            $v["f_header"] = $uuid;
            $v["f_state"] = 1;
            $v["f_dateappend"] = date("Y-m-d");
            $v["f_timeappend"] = date("H:i:s");
            $v["f_goods"] = 9490;
            $v["f_qty"] = 1;
            $v["f_price"] = $this->params->order->delivery;
            $v["f_userappend"] = 1;
            $this->sinsert("o_draft_sale_body", $v);
        }

        $data = ["order_id" => $uuid, "uuid"=>$uuid];
        $this->result["data"] = $data;
        $this->result["status"] = 0;
        $this->echoResult();
    }

    public function checkStock()
    {
        if (!$this->endpoint) {
            $url = "https://e3.picasso.am/engine/shop/create-online.php";
            $jsonData = json_encode((array) $this->params);
            $ch = curl_init($url);
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonData);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($ch, CURLOPT_HTTPHEADER, [
                'Content-Type: application/json',
                'Content-Length: ' . strlen($jsonData)
            ]);
            $response = curl_exec($ch);
            $response = json_decode($response);
            $this->result["data"] = $response->data;
            $this->echoResult();
            return;
        }
        if (empty($this->params->sku)) {
            $skulist = "";
        } else {
            $skulist = "'" . implode("','", $this->params->sku) . "'";
            if (empty($this->params->sku)) {
                $skulist = "";
            } else {
                $skulist = " and g.f_scancode in ($skulist) ";
            }
        }
        $sql = <<<EOD
        select  g.f_scancode as sku, s.f_goods, gp.f_price1, sum(s.f_qty*s.f_type) as qty 
        from a_store s 
        inner join c_goods g on g.f_id=s.f_goods
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency = 1 
        where  s.f_store in (2,3,5,24,8) $skulist
        group by 1 
        having sum(s.f_qty*s.f_type)>0
        EOD;
        $data = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->result["status"] = 0;
        $this->echoResult();
    }

    public function checkSkuQty()
    {
        if (!$this->endpoint) {
            $url = "https://e3.picasso.am/engine/shop/create-online.php";
            $jsonData = json_encode((array) $this->params);
            $ch = curl_init($url);
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonData);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($ch, CURLOPT_HTTPHEADER, [
                'Content-Type: application/json',
                'Content-Length: ' . strlen($jsonData)
            ]);
            $response = curl_exec($ch);
            $response = json_decode($response);
            $this->result["data"] = $response->data;
            $this->echoResult();
            return;
        }
        $sql = <<<EOD
        SELECT g.f_scancode AS sku, gp.f_price1 AS price, g.f_name as name, 
            gp.f_price1 as price1, coalesce(gp.f_price1disc, 0) as price2 
            FROM c_goods g 
            LEFT join c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1 
            WHERE LENGTH(g.f_scancode)>0 AND gp.f_price1 >0
        EOD;
        $data = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->result["status"] = 0;
        $this->echoResult();
    }
}

$params->app="website";
if (!empty($params->action)) {
    $co = new CreateOnline();
    if (!method_exists($co, $params->action)) {
        header("HTTP/1.1 500 Server error", true, 500);
        echo "Are You Jennifer?";
        return;
    }
    $co->{$params->action}();
} else {
    echo "how do?";
}