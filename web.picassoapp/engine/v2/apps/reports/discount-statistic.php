<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-10-20 12:36:59
# Last Modified: 2025-11-09 16:21:39

require_once "report.php";

class DiscountStatistic extends Report
{

    public function widget()
    {
        return [
            "title" => Translator::t("Discount/accumulation"),
            "icon" => "template.png",
            "version" => 2
        ];
    }

    public function  sumColumns()
    {
        return [["9" => 0], ["8" => 0]];
    }

    public function hiddenCols()
    {
        return [0, 1, 2, 3];
    }

    public function handler() {
        return [
            'd563c585-aeb9-11f0-a2cb-8a884be02f31', 'd563c585-aeb9-11f0-a2cb-8a884be02f31'
        ];
    }

    public function columns()
    {
        return [
            Translator::t("UUID"),
            Translator::t("Card code"),
            Translator::t("Partner code"),
            Translator::t("Discount type code"),
            Translator::t("Order"),
            Translator::t("Date"),
            Translator::t("Discount type"),
            Translator::t("Card number"),
            Translator::t("Person"),
            Translator::t("Value"),
            Translator::t("Discount/accumulation"),
            Translator::t("Order amount")
        ];
    }

    public function filter()
    {
        return [
            ["type" => "date", "title" => Translator::t("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => Translator::t("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" =>  Translator::t("Type of discount"), "field" => "discounttype", "filter" => "discounttype"],
            ["type" => "keyvalue", "title" =>  Translator::t("Partner"), "field" => "partner", "filter" => "partners"],
        ];
    }

    public function rows()
    {
        if (empty($this->params->date1)) {
            $this->params->date1 = date('Y-m-d');
            $this->params->date2 = date('Y-m-d');
        }
         $where =   " where  (oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}' ";
        if (!empty($this->params->discounttype)) {
            $where .= " and bd.f_mode in ({$this->params->discounttype}) ";
        }
        if (!empty($this->params->partner)) {
            $where .= " and oh.f_partner in ({$this->params->partner}) ";
        }
        $where .= ") ";
        $where .= " or ( bh.f_date between '{$this->params->date1}' and '{$this->params->date2}' ) ";

        $sql = <<<EOD
        select if(bh.f_type=9, bh.f_id, oh.f_id) as f_id, bh.f_card, oh.f_partner, bh.f_type,
        concat(oh.f_prefix, oh.f_hallid) as f_prefix,if (bh.f_type=9, bh.f_date, oh.f_datecash),
        dt.f_name as dtname,bd.f_code,concat_ws(',', bc.f_taxname,  bc.f_contact, bc.f_phone) ,bd.f_value,
        sum(bh.f_data) as f_data, sum(oh.f_amounttotal) as f_amounttotal
        from b_history bh 
        left join o_header oh on oh.f_id=bh.f_id 
        left join b_cards_discount bd on bd.f_id=bh.f_card 
        left join b_card_types dt on dt.f_id=bd.f_mode 
        left join c_partners bc on bc.f_id=bd.f_client  
        $where 
        group by bh.f_id
        EOD;
        $this->debugi = $sql;
        return $this->select($sql)->fetch_all();
    }
}
