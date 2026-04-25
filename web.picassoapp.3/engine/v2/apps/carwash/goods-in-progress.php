<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-31 14:32:35
# Last Modified: 2026-03-31 14:32:38

require_once __DIR__ . "/index.php";

class GoodsInProgress extends Auth
{
    public function get($params)
    {
        $sql = <<<EOD
        SELECT oh.f_id as f_header_id, og.f_id, JSON_DETAILED(oh.f_data) AS f_header_data, ogp.f_daily_number, ogp.f_status, cg.f_name, JSON_DETAILED(ogp.f_data) as f_ogp_data,
        coalesce(json_value(og.f_data, '$.f_cooking_time'), json_value(cg.f_data, '$.f_cooking_time')) as f_cooking_time,
        oh.f_table, t.f_name as f_table_name
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id=ogp.f_header
        inner join h_tables t on t.f_id=oh.f_table
        inner JOIN o_goods og ON og.f_id=ogp.f_id
        inner JOIN c_goods cg ON cg.f_id=og.f_goods
        WHERE ogp.f_status IN (1,2,3)
        EOD;
        $this->result["data"] = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["tables"] = $this->select("select f_id, f_name from h_tables where f_hall=1")->fetch_all(MYSQLI_ASSOC);
        $this->result["dry"] = $this->select("select f_id, f_name from h_tables where f_hall=2")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }


    public function UpdateStatus($params)
    {
        $st = (int) $params->status;
        $ss = (int) $params->substatus;
        // Один формат с TV-board / migrate-html: f_status_1_1_time, f_status_3_4_time, …
        $jsonTimePath = sprintf('$.f_status_%d_%d_time', $st, $ss);
        $sql = <<<EOD
        UPDATE o_goods_process 
        SET f_status = ?,
        f_data = JSON_SET(
            COALESCE(f_data, '{}'),
            '$jsonTimePath', NOW(),
            '$.f_substatus', ?
        )
        WHERE f_id = ?
        EOD;
        // i = корневой f_status; i или s = f_substatus в JSON (подставьте тип под вашу схему); s = f_id (uuid)
        $this->select($sql, "iis", [$st, $ss, $params->id], true);
        $this->get($params);
    }
}
