<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-28 22:09:15
# Last modified - 2025-03-31 09:45:23

require_once __DIR__ . "/../functions/ws.php";

class partners extends PClass
{
    private function getPartner($tin) {
        $sql = <<<EOD
        select * from c_partners where f_taxcode=?
        EOD;
        $partner = $this->stmtall($sql, "s", [$tin])->fetch_assoc();
        return $partner;
    }
    public function newPartner()
    {
        if (!empty($this->getPartner($this->params->tin))) {
            $this->exitError($this->tr("You cannot edit partner data"));
        }
        global $dbname;
        $v["f_state"] = 1;
        $v["f_category"] = 1;
        $v["f_group"] = 1;
        $v["f_taxname"] = $this->params->taxpayername;
        $v["f_taxcode"] = $this->params->tin;
        $v["f_address"] = $this->params->address;
        $v["f_contact"] = $this->params->contact;
        $v["f_phone"] = $this->params->phone;
        $v["f_manager"] = 1;
        $v["f_price_politic"] = 1;
        $v["f_permanent_discount"] = 0;
        $this->result["id"] = $this->sinsert("c_partners", $v);

        $wc = new WSClient();
        if ($wc->connect()) {
            $wc->handshake();
            $msg = [
                "database" => $dbname,
                "command" => "search_update_partner_cache",
                "id" => $this->result["id"]
            ];
            $wc->sendMessage(json_encode($msg));
            if (!empty($params->debug)) {
                $this->result["websocket answer"] = $wc->receiveMessage();
            }
        }
        $this->echoResult();
    }

    public function check()
    {
        $partner = $this->getPartner($this->params->tin);
        if (!empty($partner)) {
            $this->result["status"] = 2;
            $this->result["message"] = $this->tr("Taxpayer code duplicate");
            $this->result["partner"] = $partner;
        }
        $this->echoResult();
        return empty($partner);
    }
}