<?php
# (C) 2024-2025 Kudryashov Vasili
# Created - ../../2025 :)
# Last modified - 2026-07-22
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class ProcessBarCode extends Auth
{
    private $code = "";
    private $qrcode = "";
    private $params;

    private function goodsSelectSql()
    {
        return <<<SQL
            SELECT g.f_id as f_dish, g.f_name as f_dish_name, gp.f_name as f_groupname, g.f_scancode,  g.f_lowlevel,
                g.f_qtybox, u.f_name as f_unit_name, gp.f_taxdept, gp.f_adgcode as f_adgt, g.f_fiscalname as f_fiscal_name,
                g.f_candiscount, g.f_autodiscount, g.f_data,
                coalesce(g.f_service, 0) as f_is_service, coalesce(u.f_defaultqty, 0) as f_default_qty,
                if(cp.f_price1disc>0, cp.f_price1disc, cp.f_price1) as f_price1,
                if(cp.f_price2disc>0, cp.f_price2disc, cp.f_price2) as f_price2
            FROM c_goods g
            LEFT JOIN c_groups gp on gp.f_id=g.f_group
            LEFT JOIN c_goods_prices cp ON cp.f_goods=g.f_id AND cp.f_currency=1
            LEFT JOIN c_units u ON u.f_id=g.f_unit
        SQL;
    }

    /** Stock qty from store_stock (same as goods-list / F5). */
    private function stockQty(int $storeId, int $goodsId): float
    {
        if ($storeId <= 0 || $goodsId <= 0) {
            return 0.0;
        }
        $row = $this->select(
            "SELECT coalesce(SUM(f_qty_left), 0) AS f_qty FROM store_stock WHERE f_store_id=? AND f_item_id=?",
            "ii",
            [$storeId, $goodsId]
        )->fetch_assoc();
        // FrontDesk/stock editor uses money_fmt(...) which effectively rounds for display.
        // Shop stock checks compare doubles strictly enough that tiny float errors can
        // turn "2" into "1.9996" and block adding.
        $qty = (float)($row["f_qty"] ?? 0);
        if ($qty < 0) {
            $qty = 0.0;
        }
        return round($qty, 3);
    }

    private function getGoods($goods, $echoresult = true)
    {
        $storeId = (int)($this->params->store ?? 0);
        $goodsId = (int)($goods["f_dish"] ?? 0);

        $fdata = $goods["f_data"] ?? "{}";
        if (is_string($fdata)) {
            $decoded = json_decode($fdata, true);
            $goods["f_data"] = is_array($decoded) ? $decoded : [];
        } elseif (is_object($fdata)) {
            $goods["f_data"] = (array)$fdata;
        } elseif (!is_array($fdata)) {
            $goods["f_data"] = [];
        }

        $storeOverride = (int)($goods["f_data"]["f_store_override"] ?? 0);
        if ($storeOverride > 0) {
            $storeId = $storeOverride;
        }
        $goods["f_store_override"] = $storeOverride;

        $qty = 0.0;
        if ((int)($goods["f_is_service"] ?? 0) === 0) {
            $qty = $this->stockQty($storeId, $goodsId);
        }

        $goods["f_qty"] = $qty;
        $goods["f_stock"] = $qty;
        $store = [
            "f_qty" => $qty,
            "f_reserved" => 0.0,
            "f_draft" => 0.0,
        ];

        $this->result["goods"] = $goods;
        $this->result["store"] = $store;
        $this->result["barcode"] = 2;
        $this->result["emarks"] = $this->qrcode;
        if ($echoresult) {
            $this->echoResult();
        }
        return true;
    }

    private function findGoodsById($goodsId)
    {
        $sql = $this->goodsSelectSql() . " WHERE g.f_id=? AND g.f_enabled=1";
        return $this->select($sql, "i", [(int)$goodsId])->fetch_assoc();
    }

    private function findGoodsByBarcode($code)
    {
        $candidates = [];
        $code = (string)$code;
        if ($code !== "") {
            $candidates[] = $code;
            $trimmed = ltrim($code, "0");
            if ($trimmed === "") {
                $trimmed = "0";
            }
            if ($trimmed !== $code) {
                $candidates[] = $trimmed;
            }
            if (ctype_digit($code) && strlen($code) < 5) {
                $candidates[] = str_pad($code, 5, "0", STR_PAD_LEFT);
            }
        }
        $candidates = array_values(array_unique($candidates));

        foreach ($candidates as $try) {
            // Additional barcodes from c_goods_multiscancode → primary scancode
            $multibarcode = $this->select(
                "SELECT g.f_scancode FROM c_goods_multiscancode m LEFT JOIN c_goods g ON g.f_id=m.f_goods WHERE m.f_id=?",
                "s",
                [$try]
            )->fetch_assoc();
            $lookup = $try;
            if (!empty($multibarcode["f_scancode"])) {
                $lookup = $multibarcode["f_scancode"];
                $this->code = $lookup;
            }

            $sql = $this->goodsSelectSql() . " WHERE g.f_scancode=? AND g.f_enabled=1";
            $goods = $this->select($sql, "s", [$lookup])->fetch_assoc();
            if ($goods) {
                return $goods;
            }

            // Fallback: barcode exists only in multiscancode (primary f_scancode empty/mismatched)
            $sql = $this->goodsSelectSql() . <<<SQL
            INNER JOIN c_goods_multiscancode m ON m.f_goods=g.f_id
            WHERE m.f_id=? AND g.f_enabled=1
        SQL;
            $goods = $this->select($sql, "s", [$try])->fetch_assoc();
            if ($goods) {
                return $goods;
            }
        }

        return null;
    }

    /** In-store weighted EAN-13: FF + PPPPP + WWWWWW (weight kg * 10000). */
    private function parseScaleBarcode($code)
    {
        $code = (string)$code;
        if (strlen($code) !== 13 || $code[0] !== "2" || !ctype_digit($code)) {
            return null;
        }
        $pluRaw = substr($code, 2, 5);
        $plu = ltrim($pluRaw, "0");
        if ($plu === "") {
            return null;
        }
        $weight = ((float)substr($code, 7, 6)) / 10000.0;
        if ($weight <= 0) {
            return null;
        }
        return [
            "plu" => $plu,
            "plu_padded" => $pluRaw,
            "weight" => $weight,
        ];
    }

    private function respondGoods($goods)
    {
        if (!$goods) {
            dieWithCode(Translator::t("Wrong barcode") . "<br>" . $this->code);
        }
        $fdata = $goods["f_data"] ?? "{}";
        if (is_string($fdata)) {
            $decoded = json_decode($fdata, true);
            $goods["f_data"] = is_array($decoded) ? $decoded : [];
        } elseif (is_object($fdata)) {
            $goods["f_data"] = (array)$fdata;
        } elseif (!is_array($fdata)) {
            $goods["f_data"] = [];
        }
        if ($goods["f_is_service"] > 0) {
            $this->result["goods"] = $goods;
            $this->echoResult();
            return;
        }
        $goods["f_barcode"] = $this->code !== "" ? $this->code : ($goods["f_scancode"] ?? "");
        if ($this->getGoods($goods)) {
            return;
        }
        dieWithCode(Translator::t("Wrong barcode") . "<br>" . ($this->result["echocode"] ?? $this->code));
    }

    public function get($params)
    {
        $this->params = $params;
        $goodsId = (int)($params->goods_id ?? 0);
        if ($goodsId > 0) {
            $this->code = (string)($params->barcode ?? "");
            $this->respondGoods($this->findGoodsById($goodsId));
            return;
        }

        $this->code = (string)($params->barcode ?? "");
        if (strlen($this->code) >= 29) {
            $this->qrcode = $this->code;
            if (substr($this->code, 0, 3) == "010") {
                $this->code = (string) intval(substr($this->code, 3, 13));
            } else if (substr($this->code, 0, 6) == "000000") {
                $this->code = substr($this->code, 6, 8);
            }
            if (strlen($this->code) >= 29) {
                $this->code = substr($this->code, 1, 13);
            }
        }

        $goods = $this->findGoodsByBarcode($this->code);
        if (!$goods) {
            $scale = $this->parseScaleBarcode($this->code);
            if ($scale) {
                $goods = $this->findGoodsByBarcode($scale["plu"]);
                if (!$goods) {
                    $goods = $this->findGoodsByBarcode($scale["plu_padded"]);
                }
                if ($goods) {
                    $this->code = $scale["plu"];
                    $goods["f_default_qty"] = $scale["weight"];
                    $this->result["scale_weight"] = $scale["weight"];
                }
            }
        }

        $this->respondGoods($goods);
    }
}
