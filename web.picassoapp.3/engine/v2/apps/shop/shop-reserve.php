<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-02 19:20:39
# Last Modified: 2026-02-02 19:20:41

require __DIR__ . "/index.php";

class ShopReserve extends Auth
{

    public function get($params)
    {
        $sql = <<<EOD
         SELECT 
        a.*,
        json_object(
        'f_id', s.f_id, 
        'f_name', s.f_name)
        as store_json,
        JSON_OBJECT(
            'f_id', g.f_id,
            'f_group_name', gr.f_name,
            'f_name', g.f_name,
            'f_barcode', g.f_scancode,
            'f_unit_name', u.f_name,
            'f_lastinput', g.f_lastinputprice,
            'f_price1', gp.f_price1,
            'f_price1disc', gp.f_price1disc,
            'f_price2', gp.f_price2,
            'f_price2disc', gp.f_price2disc
            ) AS goods_json
        FROM a_store_reserve a
        left join c_goods g on g.f_id=a.f_goods
        left join c_storages s on s.f_id=a.f_store
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        LEFT JOIN c_units u ON u.f_id = g.f_unit
        LEFT JOIN c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1
        WHERE a.f_id = ?;

        EOD;
        $row = $this->select($sql, "i", [$params->id])->fetch_assoc();

        $row['store_json'] = json_decode($row['store_json'], true);
        $row['goods_json'] = json_decode($row['goods_json'], true);

        $this->result = array_merge($this->result, $row);
        $this->echoResult();
    }

    public function GetStoreGoods($params)
    {

        $sql = <<<EOD
        SELECT 
        json_detailed(json_object(
        'f_id', s.f_id, 
        'f_name', s.f_name))
        as store_json
        from c_storages s
        WHERE s.f_id = ?;
        EOD;
        $this->result["store_json"] = json_decode($this->select($sql, "i", [$params->store_id])->fetch_assoc()["store_json"]);
        $sql = <<<EOD
        select
        json_detailed(JSON_OBJECT(
        'f_id', g.f_id,
        'f_group_name', gr.f_name,
        'f_name', g.f_name,
        'f_barcode', g.f_scancode,
        'f_unit_name', u.f_name,
        'f_lastinput', g.f_lastinputprice,
        'f_price1', gp.f_price1,
        'f_price1disc', gp.f_price1disc,
        'f_price2', gp.f_price2,
        'f_price2disc', gp.f_price2disc
        )) AS goods_json
        from c_goods g 
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        LEFT JOIN c_units u ON u.f_id = g.f_unit
        LEFT JOIN c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1
        where g.f_id=?
        EOD;
        $this->result["goods_json"] = json_decode($this->select($sql, "i", [$params->goods_id])->fetch_assoc()["goods_json"]);
        $this->echoResult();
    }
}
