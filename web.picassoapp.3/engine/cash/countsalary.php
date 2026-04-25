<?php
require_once(__DIR__ . "/../app.php");

class CountSalary extends DB
{
    public function __construct()
    {
        parent::__construct();
    }

    public function count()
    {
        //total amount of shift
        $amount = $this->stmtall("select coalesce(sum(f_amounttotal), 0) as amount from o_header where f_working_session=? and f_state=2", "i", [
            $this->params->cashsession
        ])->fetch_assoc()["amount"];

        #Spesial carwash
        if (!empty($this->params->carwash)) {
            #count manager
            $countman = $this->stmtall(
                "select coalesce(count(f_id), 0) as man from s_salary_attendance where f_position=? and f_shift=? ",
                "ii",
                [3, $this->params->cashsession]
            )->fetch_assoc();

            //count washer
            $countwash = $this->stmtall(
                "select coalesce(count(f_id), 0) as man from s_salary_attendance where f_position=? and f_shift=? ",
                "ii",
                [4, $this->params->cashsession]
            )->fetch_assoc();

            $amountofman = 0;
            if ($countman["man"] > 0) {
                $amountofman = ($amount * 0.1) / $countman["man"];
            }

            $amountofwash = 0;
            if ($countwash["man"] > 0) {
                $amountofwash = (($amount - ($amount * 0.1)) * 0.4) / $countwash["man"];
            }


            $this->stmtall(
                "update s_salary_attendance set f_amount=? where f_shift=? and f_position=3",
                "ii",
                [$amountofman, $this->params->cashsession]
            );

            $this->stmtall(
                "update s_salary_attendance set f_amount=? where f_shift=? and f_position=4",
                "ii",
                [$amountofwash, $this->params->cashsession]
            );
        }
    }
}

if ($params->action == "count") {
    $cs = new CountSalary();
    $cs->count();
    $cs->echoResult();
}