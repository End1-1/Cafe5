<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-31 15:01:53
# Last modified - 2025-04-03 10:09:21

class checksaleoutput extends PClass
{
    public function open()
    {
        $docparams = explode(";", $this->params->docparams);
        $doc = $this->stmtall("select * from o_header where f_id=?", "s", [$docparams[1]])->fetch_assoc();
        if (empty($doc)) {
            $this->exitError($this->tr("Document not found {$docparams[1]}"));
        }
        if ($docparams[3] != $doc["lu"]) {
            $this->exitError($this->tr("Document was changed"));
        }
        $sql = <<<EOD
        select sum(d.f_qty) as qty,
        g.f_id as id, gr.f_id as groupid, g.f_name as name, gr.f_name as groupname, u.f_name as unit,
        d.f_price as p1, 0 as p1d, d.f_price as p2, 0 as p2d, coalesce(g.f_scancode, '') as sku
        from o_goods d
        left join c_goods g on g.f_id=d.f_goods
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        where d.f_header=?
        group by d.f_goods
        EOD;
        $this->result["id"] = $docparams[1];
        $this->result["goods"] = $this->stmtall($sql, "s", [$docparams[1]])->fetch_all(MYSQLI_ASSOC);
        $this->result["qrlist"] = $this->stmtall("select f_qr from o_qr where f_header=?", "s", [$docparams[1]])->fetch_all(MYSQLI_ASSOC);
        $this->result["docparams"] = $docparams;
        $this->echoResult();
    }

    public function checkQr() {
        $sql = <<<EOD
        select * from o_qr where f_qr=?
        EOD;
        $qr = $this->stmtall($sql, "s", [$this->params->f_qr])->fetch_assoc();
        if (!empty($qr)) {
            $this->exitError($this->tr("QR code used"));
        }
        $v["f_header"] = $this->params->f_header;
        $v["f_qr"] = $this->params->f_qr;
        $this->sinsert("o_qr", $v);
        $this->echoResult();
    }

    public function confirm() {
        $this->result["yes"] = "its ttrue";
        $this->echoResult();
    }

    public function removeQr() {
        $this->stmtall("delete from o_qr where f_header=? and f_qr=?", "ss", [$this->params->f_header, $this->params->f_qr]);
        $this->echoResult();
    }
}