<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Goods extends Auth
{
    private function goodsSelectSql(string $where): string
    {
        return <<<SQL
            SELECT
                g.f_id,
                g.f_group AS f_group_id,
                gr.f_name AS f_group_name,
                g.f_name,
                g.f_scancode AS f_barcode,
                u.f_name AS f_unit_name,
                COALESCE(gp.f_price1, 0) AS f_price1,
                COALESCE(gp.f_price1disc, 0) AS f_price1disc,
                COALESCE(gp.f_price2, 0) AS f_price2,
                COALESCE(gp.f_price2disc, 0) AS f_price2disc,
                g.f_lastinputprice AS f_lastinputprice,
                IF(LENGTH(COALESCE(g.f_adg, '')) > 0, g.f_adg, gr.f_adgcode) AS f_adgt
            FROM c_goods g
            LEFT JOIN c_groups gr ON gr.f_id = g.f_group
            LEFT JOIN c_units u ON u.f_id = g.f_unit
            LEFT JOIN c_goods_prices gp ON gp.f_goods = g.f_id AND gp.f_currency = 1
            WHERE {$where}
            LIMIT 5
        SQL;
    }

    private function loadGoodsRow(int $id): ?array
    {
        return $this->select($this->goodsSelectSql("g.f_id = ?"), "i", [$id])->fetch_assoc() ?: null;
    }

    private function findOrCreateUnit(string $unitName): int
    {
        $unitName = trim(preg_replace('/\s+/u', ' ', $unitName));
        if ($unitName === "") {
            $unitName = "հատ";
        }

        $row = $this->select(
            "SELECT f_id FROM c_units WHERE LOWER(TRIM(f_name)) = LOWER(?) LIMIT 1",
            "s",
            [$unitName]
        )->fetch_assoc();
        if ($row) {
            return (int)$row["f_id"];
        }

        return (int)$this->insert("c_units", [
            "f_name" => $unitName,
            "f_fullname" => $unitName,
            "f_defaultqty" => 1.0,
        ]);
    }

    public function Get($params)
    {
        $id = (int)($params->f_id ?? 0);
        if ($id <= 0) {
            dieWithCode("f_id is required");
        }
        $row = $this->loadGoodsRow($id);
        if (!$row) {
            dieWithCode("Goods not found");
        }
        $this->result["goods"] = $row;
        $this->echoResult();
    }

    public function FindByExactName($params)
    {
        $name = trim(preg_replace('/\s+/u', ' ', (string)($params->f_name ?? "")));
        if ($name === "") {
            dieWithCode("f_name is required");
        }
        $rows = $this->select(
            $this->goodsSelectSql("g.f_enabled = 1 AND LOWER(TRIM(g.f_name)) = LOWER(?)"),
            "s",
            [$name]
        )->fetch_all(MYSQLI_ASSOC);

        // Exact unique match only — ambiguous / missing names are handled by the client dialog.
        $this->result["goods"] = (count($rows) === 1) ? $rows[0] : null;
        $this->echoResult();
    }

    public function CreateFromXml($params)
    {
        $name = trim(preg_replace('/\s+/u', ' ', (string)($params->f_name ?? "")));
        $unitName = trim(preg_replace('/\s+/u', ' ', (string)($params->f_unit_name ?? "")));
        $supplier = (int)($params->f_supplier ?? 0);
        $adg = trim((string)($params->f_adg ?? ""));
        $lastInput = (float)($params->f_lastinputprice ?? 0);

        if ($name === "") {
            dieWithCode("f_name is required");
        }
        if ($supplier <= 0) {
            dieWithCode("f_supplier is required");
        }

        $unitId = $this->findOrCreateUnit($unitName);

        $goodsData = [
            "f_name" => $name,
            "f_type" => 1,
            "f_supplier" => $supplier,
            "f_group" => 1,
            "f_unit" => $unitId,
            "f_iscomplect" => 0,
            "f_base_currency" => 1,
            "f_enabled" => 1,
            "f_service" => 0,
            "f_lastinputprice" => $lastInput,
            "f_storeid" => 0,
        ];
        if ($adg !== "") {
            $goodsData["f_adg"] = $adg;
        }

        $goodsId = (int)$this->insert("c_goods", $goodsData);
        $this->update("c_goods", ["f_storeid" => $goodsId], $goodsId);

        $currencies = $this->select("SELECT f_id FROM e_currency ORDER BY f_id", "", [])->fetch_all(MYSQLI_ASSOC);
        if (empty($currencies)) {
            $currencies = [["f_id" => 1]];
        }
        foreach ($currencies as $currency) {
            $this->insert("c_goods_prices", [
                "f_goods" => $goodsId,
                "f_currency" => (int)$currency["f_id"],
                "f_price1" => 0.0,
                "f_price2" => 0.0,
                "f_price1disc" => 0.0,
                "f_price2disc" => 0.0,
            ]);
        }

        $notify = require_once __DIR__ . "/../../worker/ws-notify.php";
        $notify->notify("goods", $goodsId, true);

        $row = $this->loadGoodsRow($goodsId);
        if (!$row) {
            dieWithCode("Failed to create goods");
        }
        $this->result["goods"] = $row;
        $this->echoResult();
    }

    public function Rename($params)
    {
        $id = (int)($params->f_id ?? 0);
        $name = trim((string)($params->f_name ?? ""));
        if ($id <= 0) {
            dieWithCode("f_id is required");
        }
        if ($name === "") {
            dieWithCode("f_name is required");
        }
        $row = $this->select("select f_id from c_goods where f_id=?", "i", [$id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Goods not found");
        }
        $this->update("c_goods", ["f_name" => $name], $id);
        $notify = require_once __DIR__ . "/../../worker/ws-notify.php";
        $notify->notify("goods", $id, false);
        $this->result["f_id"] = $id;
        $this->echoResult();
    }
}
