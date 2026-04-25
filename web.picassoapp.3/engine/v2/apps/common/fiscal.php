<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-23 01:25:41
# Last Modified: 2026-02-26 00:28:44

require_once __DIR__ . "/index.php";

class Fiscal extends Auth
{
    public function InsertLog($params)
    {
        $v["f_id"] = $params->f_id;
        $v["f_order"] = $params->f_order;
        $v["f_date"] = date("Y-m-d");
        $v["f_time"] = date("H:i:s");
        $v["f_elapsed"] = $params->elapsed ?? 0;
        $v["f_in"] = is_string($params->in) ? $params->in : json_encode($params->in, JSON_UNESCAPED_UNICODE);
        $v["f_out"] = is_string($params->out) ? $params->out : json_encode($params->out, JSON_UNESCAPED_UNICODE);
        $v["f_err"] = $params->error;
        $v["f_result"] = $params->result;
        $v["f_state"] = $params->result === 0 ? 1 : 0;
        $this->insert("o_tax_log", $v);
        if ($params->result === 0) {
            $this->updateJsonField("o_header", $params->f_order, "f_data", "f_fiscal", is_string($params->out) ? json_decode($params->out, true) : $params->out);
        }
    }

    public function Log($params)
    {

        $this->InsertLog($params);

        $this->echoResult();
    }

    public function Get($params)
    {
        $this->result["header"] = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        $sql = <<<EOD
        select og.f_id, og.f_goods, g.f_name, og.f_qty, 
        gu.f_name as f_unitname, og.f_price, og.f_total,
        t.f_taxdept, t.f_adgcode, 
        og.f_store 
        from o_goods og 
        left join c_goods g on g.f_id=og.f_goods 
        left join c_units gu on gu.f_id=g.f_unit 
        left join c_groups t on t.f_id=g.f_group 
        where og.f_header=? 
        order by og.f_row 
        EOD;
        $this->result["goods"] = $this->select($sql, "s", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["partner"] = $this->select("select * from c_partners where f_id=?", "i", [$this->result["header"]["f_partner"]])->fetch_assoc();
        $this->echoResult();
    }
}
