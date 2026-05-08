<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-27 17:22:07
# Last modified - 2025-03-27 17:22:10
defined('APP') or die('Die, vampire!');

class waiter extends PClass
{
    public function checkQr($params)
    {
        $sql = <<<EOD
        select * from o_body where f_state=1 and f_emarks=?
        EOD;
        $r = $this->stmtall($sql, "s", [$params->qr])->fetch_assoc();
        if (!empty($r)) {
            $this->exitError($this->tr("Emarks duplicate"));
        }
        $this->echoResult();
    }

    public function callproc($name, $params)
    {
        $this->db->begin_transaction();
        $stmt = $this->db->prepare("CALL $name(?, @f_out)");
        $inp = json_encode($params);
        $stmt->bind_param("s", $inp);
        $stmt->execute();
        $result = $this->db->query("SELECT @f_out AS f_out");
        $row = json_decode($result->fetch_assoc()["f_out"], true);
        $this->result = $row;
        $this->db->commit();
        $this->db->close();
        $this->echoResult();
    }

    public function opentable($params)
    {
        $this->callproc("waiter_open_table", $params);
    }

    public function closetable($params)
    {
        $this->callproc("waiter_close_table", $params);
    }
}
