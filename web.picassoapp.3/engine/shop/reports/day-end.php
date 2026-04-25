<?php
# (C) 2025 Kudryashov Vasili
# Last modified - 2025-08-29 01:16:03
require_once __DIR__ . "/index.php";

class DayEnd extends PClass {
    public function get() {
        $sql = <<<EOD
        select h.f_name, d.* 
        from users_elina_day_end d
        left join h_halls h on h.f_id=d.f_hall 
        where d.f_date=?
        EOD;
        $this->result["report"] = $this->stmtall($sql, "s", [$this->params->date])->fetch_all(MYSQLI_ASSOC);
        $this->result["test"] = true;
        $this->echoResult();
    }
}

if (!empty ($params->action)) {
    $de = new DayEnd();
    $de->{$params->action}();
}