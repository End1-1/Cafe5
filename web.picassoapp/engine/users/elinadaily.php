<?php
# © 2025, Picasso Software
# Created - 2025-05-20 08:57:52
# Last modified - 2025-05-20 08:57:56


class elinadaily extends PClass
{
    public function get()
    {
        if ($this->params->config == 0) {
            exitError("No config provided");
        }
        $this->params->date = date("Y-m-d");
        $sql = <<<EOD
        select * from sys_json_config where f_id=?
        EOD;

        $configArray = $this->stmtall($sql, "i", [$this->params->config])->fetch_assoc();
        if (!$configArray) {
            exitError("Config not found");
        }
        $config = json_decode($configArray["f_config"]);
        if (json_last_error() !== JSON_ERROR_NONE) {
            exitError("Invalid config JSON");
        }


        $sql = <<<EOD
        select  coalesce( sum(ec.f_amount*ec.f_sign)) as f_amountcash
        from e_cash ec
        left join a_header h on h.f_id=ec.f_header 
        left join e_cash_names cn on cn.f_id=ec.f_cash  
        where  h.f_state=1 and ec.f_cash=?
        EOD;
        $this->result["cash"] = $this->stmtall($sql, "i", [$config->cash_cash_id])->fetch_assoc()["f_amountcash"];

        $sql = <<<EOD
        select  coalesce( sum(ec.f_amount*ec.f_sign)) as f_amount 
        from e_cash ec 
        left join a_header h on h.f_id=ec.f_header 
        left join e_cash_names cn on cn.f_id=ec.f_cash  
        where  h.f_state=1 and ec.f_cash=? and h.f_date=?
        EOD;
        $this->result["card"] = $this->stmtall($sql, "is", [$config->cash_card_id, $this->params->date])->fetch_assoc()["f_amount"];

        $sql = <<<EOD
        select coalesce(sum(f_amountprepaid),0) as f_amountprepaid 
        from o_header h 
        where h.f_datecash=? and h.f_state=? and h.f_hall=?
        EOD;
        $this->result["prepaid"] = $this->stmtall($sql, "sii", [$this->params->date, 2, $config->hall])->fetch_assoc()["f_amountprepaid"];;

        #daily report
        $sql = <<<EOD
        select * from users_elina_day_end where f_date=? and f_hall=?
        EOD;
        $this->result["daily"] = $this->stmtall($sql, "si", [$this->params->date, $config->hall])->fetch_assoc();
        if (!$this->result["daily"]) {
            $this->result["daily"] = [
                "f_date" => $this->params->date,
                "f_hall" => $config->hall,
                "f_prevday" => 0,
                "f_income" => 0,
                "f_inputother" => 0,
                "f_return" => 0,
                "f_sale" => 0,
                "f_output" => 0,
                "f_final" => 0,
                "f_check" => 0
            ];
            $rowid = $this->sinsert("users_elina_day_end", $this->result["daily"]);
        } else {
            $rowid = $this->result["daily"]["f_id"];
        }

        $storeid = $config->store;
        #yesterday
        $sql = <<<EOD
        SELECT * 
        FROM users_elina_day_end 
        WHERE f_date = ? AND f_hall = ?
        EOD;
        $yesterday = date("Y-m-d", strtotime("-1 day"));
        $r0 = $this->stmtall($sql, "si", [$yesterday, $config->hall])->fetch_assoc();
        if (empty($r0)) {
            $r0["f_prevday"] = 0;
        } else {
            $r0["f_prevday"] = $r0["f_final"];
        }
        #final
        $sql = <<<EOD
        select sum(s.f_qty * s.f_type * if(gp.f_price1disc>0, gp.f_price1disc, gp.f_price1))  as f_amount
        from a_store s 
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency=1
        where s.f_store=$storeid
        EOD;
        $r1 = $this->stmtall($sql)->fetch_assoc();
        #income
        $sql = <<<EOD
        select sum(s.f_qty * s.f_type * if(gp.f_price1disc>0, gp.f_price1disc, gp.f_price1))  as f_amount
        from a_store s 
        left join a_header h on h.f_id=s.f_document 
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency=1
        where h.f_state=1 and h.f_date=current_date 
        and s.f_store=$storeid and s.f_type=1 and s.f_reason=1
        EOD;
        $r2 = $this->stmtall($sql)->fetch_assoc();
        #sale
        $sql = <<<EOD
        select sum(s.f_qty * s.f_type * if(gp.f_price1disc>0, gp.f_price1disc, gp.f_price1))  as f_amount
        from a_store s 
        left join a_header h on h.f_id=s.f_document 
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency=1
        where h.f_state=1 and h.f_date=current_date and s.f_store=$storeid
        and s.f_reason in (4,9)
        EOD;
        $r3 = $this->stmtall($sql)->fetch_assoc();
        #move out
        $sql = <<<EOD
        select sum(s.f_qty * s.f_type * if(gp.f_price1disc>0, gp.f_price1disc, gp.f_price1))  as f_amount
        from a_store s 
        left join a_header h on h.f_id=s.f_document 
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency=1
        where h.f_state=1 and h.f_date=current_date and s.f_store=$storeid
        and s.f_type=-1 and s.f_reason <> 4
        EOD;
        $r4 = $this->stmtall($sql)->fetch_assoc();
        #move in
        $sql = <<<EOD
        select sum(s.f_qty * s.f_type * if(gp.f_price1disc>0, gp.f_price1disc, gp.f_price1))  as f_amount
        from a_store s 
        left join a_header h on h.f_id=s.f_document 
        left join c_goods_prices gp on gp.f_goods=s.f_goods and gp.f_currency=1
        where h.f_state=1 and h.f_date=current_date and s.f_store=$storeid
        and s.f_type=1 and s.f_reason <> 1
        EOD;
        $r5 = $this->stmtall($sql)->fetch_assoc();
        #update report
        $v["f_prevday"] = $r0["f_prevday"];
        $v["f_final"] = $r1["f_amount"];
        $v["f_income"] = $r2["f_amount"];
        $v["f_sale"] = $r3["f_amount"];
        $v["f_output"] = $r4["f_amount"];
        $v["f_inputother"] = $r5["f_amount"];
        $this->supdate("users_elina_day_end", $v, $rowid);

        $sql = <<<EOD
        select * from users_elina_day_end where f_date=? and f_hall=?
        EOD;
        $this->result["daily"] = $this->stmtall($sql, "si", [$this->params->date, $config->hall])->fetch_assoc();

        $this->echoResult();
    }
}
