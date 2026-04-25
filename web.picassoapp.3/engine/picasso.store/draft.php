<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-26 14:03:36
# Last modified - 2026-02-06 12:40:39
defined('APP') or die('Die, vampire!');

class draft extends PClass
{
    public function save()
    {

        $this->db->begin_transaction();
        $isnew = empty($this->params->header->f_id);
        if ($isnew) {
            $this->params->header->f_id = $this->uuidv4();
        }
        $this->params->header->f_date = date("Y-m-d");
        $this->params->header->f_time = date("H:i:s");
        $h = (array) $this->params->header;
        $this->sinsertupdate("o_draft_sale", $h, $h["f_id"], $isnew);
        $i = 0;
        foreach ($this->params->body as $b) {
            $isnew = empty($b->f_id);
            if ($isnew) {
                $b->f_id = $this->uuidv4();
                $b->f_dateappend = date("Y-m-d");
                $b->f_timeappend = date("H:i:s");
            }
            $b->f_row = $i++;
            $b->f_header = $this->params->header->f_id;
            $this->sinsertupdate("o_draft_sale_body", $b, $b->f_id, $isnew);
        }
        $this->db->commit();
        $this->echoResult();
    }

    public function openDraft()
    {
        $sql = <<<EOD
        select f_id, f_datefor, f_comment, f_partner, f_state 
        from o_draft_sale 
        where f_id=?
        EOD;
        $ord = $this->stmtall($sql, "s", [$this->params->id])->fetch_assoc();
        if (empty($ord["f_state"]) || $ord["f_state"] != 1) {
            $this->openOrder();
            return;
        }
        $this->result["order"] = $ord;
        $sql = <<<EOD
        select f_id as id, f_taxname as taxname, coalesce(f_taxcode, '') as tin, coalesce(f_price_politic,1) as mode,
        coalesce(f_address,'') as address, coalesce(f_permanent_discount,0) as discount,
        coalesce(f_phone, '') as phone, coalesce(f_contact,'') as contact, coalesce(f_name,'') as name
        from c_partners where f_id=?
        EOD;
        $this->result["partner"] = $this->stmtall($sql, "i", [$ord["f_partner"]])->fetch_assoc();
        $sql = <<<EOD
        select d.f_id as uuid, d.f_qty ,
        g.f_id as f_id, gr.f_id as f_group_id, g.f_name as f_name, gr.f_name as f_group_name, u.f_name as f_unit_name,
        d.f_price as f_price1, 0 as f_price1disc, d.f_price as f_price2, 0 as f_price2disc, coalesce(g.f_scancode, '') as f_barcode
        from o_draft_sale_body d
        left join c_goods g on g.f_id=d.f_goods
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        where d.f_state=1 and d.f_header=?
        EOD;
        $this->result["goods"] = $this->stmtall($sql, "s", [$this->params->id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function openOrder()
    {
        $sql = <<<EOD
        select o.f_id, od.f_datefor, o.f_comment, o.f_partner ,
        o.f_state
        from o_header o
        left join o_draft_sale od on od.f_id=o.f_id
        where o.f_id=?
        EOD;
        $ord = $this->stmtall($sql, "s", [$this->params->id])->fetch_assoc();
        $this->result["order"] = $ord;
        $sql = <<<EOD
        select f_id as id, f_taxname as taxname, coalesce(f_taxcode, '') as tin, f_price_politic as mode,
        f_address as address, f_permanent_discount as discount,
        f_phone as phone, f_contact as contact
        from c_partners where f_id=?
        EOD;
        $this->result["partner"] = $this->stmtall($sql, "i", [$ord["f_partner"]])->fetch_assoc();
        $sql = <<<EOD
        select d.f_id as uuid, d.f_qty ,
        g.f_id as f_id, gr.f_id as f_group_id, g.f_name, gr.f_name as f_group_name, u.f_name as f_unit_name,
        d.f_price f_price1, 0 as f_price1disc, d.f_price as f_price2, 0 as f_price2disc, coalesce(g.f_scancode, '') as f_barcode
        from o_goods d
        left join c_goods g on g.f_id=d.f_goods
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        where d.f_header=?
        EOD;
        $this->result["goods"] = $this->stmtall($sql, "s", [$this->params->id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function remove()
    {
        $this->db->begin_transaction();
        $this->stmtall("update o_draft_sale set f_state=3 where f_state=1 and f_id=?", "s", [$this->params->id]);
        $this->stmtall("update o_draft_sale_body set f_state=3 where f_state=1 and f_header=?", "s", [$this->params->id]);
        $this->db->commit();
        $this->echoResult();
    }

    public function removeOneRow()
    {
        $this->stmtall("update o_draft_sale_body set f_state=3 where f_id=?", "s", [$this->params->id]);
        $this->echoResult();
    }

    public function getDrafts()
    {
        $sql = <<<EOD
        select o.f_id, o.f_datefor, 
        p.f_taxname, p.f_address, p.f_taxcode,
        o.f_amount
        from o_draft_sale o
        left join c_partners p on p.f_id=o.f_partner
        where o.f_state = 1 and o.f_staff={$this->userid}
        order by o.f_date, o.f_time
        EOD;

        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function getCompletedOrders()
    {
        $sql = <<<EOD
        select o.f_id, od.f_datefor, o.f_state, 
        p.f_taxname, p.f_address, p.f_taxcode,
        o.f_amounttotal
        from o_header o
        left join o_draft_sale od on od.f_id=o.f_id
        left join c_partners p on p.f_id=o.f_partner
        where o.f_state = 2 and o.f_staff={$this->userid} 
        and o.f_datecash between '{$this->params->datestart}' and '{$this->params->dateend}'
        EOD;

        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
