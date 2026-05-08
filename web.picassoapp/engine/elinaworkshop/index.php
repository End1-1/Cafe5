<?php
require_once __DIR__ . "/elinaworkshop.php";

class IndexPhp extends DB
{
    public function getListOfTask()
    {
        $sql = <<<EOD
        select t.f_id, coalesce(t.f_product, 0) as f_product, coalesce(concat(t.f_id, ' ', p.f_name), '') as f_name from mf_tasks t 
        left join mf_actions_group p on p.f_id=t.f_product   
        where t.f_state=1
        EOD;
        $data = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->echoResult();
    }
    public function getListOfTeamLead()
    {
        $sql = <<<EOD
        select distinct(u.f_teamlead) as f_id ,
        concat_ws(' ', u.f_last, u.f_first) as f_name 
        from s_user m 
        inner join s_user u on u.f_teamlead=m.f_id 
        WHERE u.f_teamlead>0  
        order by 2
        EOD;
        $data = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->echoResult();
    }
    public function getListOfEmployee()
    {
        $sql = <<<EOD
        select u.f_id, u.f_teamlead, concat(u.f_last, ' ', u.f_first) as f_name  
        from s_user u 
        WHERE u.f_teamlead>0 and f_state=1 
        order by 3 
        EOD;
        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
    public function getListOfWorks()
    {
        $where = "where p.f_date='{$this->params->f_date}' ";
        $where .= " and p.f_worker={$this->params->f_worker} ";
        if ($this->params->f_task > 0) {
            $where .= " and t.f_id={$this->params->f_task} ";
        }
        if ($this->params->f_teamlead > 0) {
            $where .= " and t.f_responsible={$this->params->f_teamlead} ";
        }

        $sql = <<<EOD
        select p.f_id, pr.f_name as f_productname, ac.f_name as f_processname, 
        p.f_qty, p.f_price, p.f_taskid, concat('#', t.f_id, ' ', pr.f_name) as f_taskdate, 
        t.f_qty as f_goal, cast( dp.f_qty as int) as f_ready, p.f_process, p.f_laststep 
        from mf_daily_process p 
        inner join mf_actions_group pr on pr.f_id=p.f_product 
        inner join mf_actions ac on ac.f_id=p.f_process 
        left join mf_tasks t on t.f_id=p.f_taskid 
        left join (select dp.f_process, dp.f_taskid, sum(dp.f_qty) as f_qty 
        from mf_daily_process dp group by 1,2) as dp on dp.f_taskid=p.f_taskid and dp.f_process=p.f_process 
        $where
        order by pr.f_name, ac.f_id
        EOD;
        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
    public function listOfTaskWorks()
    {
        $sql = <<<EOD
        select p.f_id , p.f_rowid + 1 as f_rowid , p.f_product , gr.f_name as f_grname, 
        p.f_process , ac.f_name as f_acname, 
        p.f_durationsec , p.f_price , coalesce(ac.f_state  , 0) as f_state
        from mf_process p 
        inner join mf_actions_group gr on gr.f_id=p.f_product 
        inner join mf_actions ac on ac.f_id=p.f_process 
        where p.f_product = {$this->params->f_product} 
        EOD;
        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
    public function addWorkToTask()
    {
        $this->addWorkerToDay(false);

        $v["f_date"] = $this->params->f_date;
        $v["f_worker"] = $this->params->f_worker;
        $v["f_product"] = $this->params->f_product;
        $v["f_process"] = $this->params->f_process;
        $v["f_qty"] = 0;
        $v["f_price"] = $this->params->f_price;
        $v["f_taskid"] = $this->params->f_taskid;
        $v["f_laststep"] = $this->params->f_laststep;
        $this->sinsert("mf_daily_process", $v);
        $this->result["data"] = [];
        $this->echoResult();
    }
    public function employesOfTheDay()
    {
        $where = "where f_date='{$this->params->f_date}' ";
        if ($this->params->f_teamlead > 0) {
            $where .= " and u.f_teamlead={$this->params->f_teamlead} ";
        }

        $sql = <<<EOD
        select m.f_worker as f_id, u.f_teamlead, concat(u.f_last, ' ', u.f_first) as f_name 
        from mf_daily_workers m 
        inner join s_user u on u.f_id=m.f_worker $where
        EOD;
        $data = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        if (!empty($this->params->debug)) {
            $this->result["sql"] = str_replace("\n", " ", str_replace("\r", " ", $sql));
        }
        $this->echoResult();
    }
    public function removeWork()
    {
        $this->stmtall("delete from mf_daily_process where f_id={$this->params->f_id}");
        $this->stmtall("delete from mf_process_details_done where f_processid={$this->params->f_id}");
        $this->result["data"] = [];
        $this->echoResult();
    }
    public function workDetails()
    {
        $id = $this->params->f_id;
        $v["f_task"] = $this->params->f_task;
        $v["f_process"] = $this->params->f_process;
        $v["f_color"] = $this->params->f_color;
        $v["f_34p"] = $this->params->f_34;
        $v["f_36p"] = $this->params->f_36;
        $v["f_38p"] = $this->params->f_38;
        $v["f_40p"] = $this->params->f_40;
        $v["f_42p"] = $this->params->f_42;
        $v["f_44p"] = $this->params->f_44;
        $v["f_46p"] = $this->params->f_46;
        $v["f_48p"] = $this->params->f_48;
        $v["f_50p"] = $this->params->f_50;
        $v["f_52p"] = $this->params->f_52;
        if ($id == 0) {
            $id = $this->sinsert("mf_process_details", $v);
        } else {
            $this->supdate("mf_process_details", $v, $id);
        }
        $task = $this->stmtall("select sum(f_34p+f_36p+f_38p+f_40p+f_42p+f_44p+f_46p+f_48p+f_50p+f_52p) as asum from mf_process_details where f_task={$this->params->f_task}")->fetch_assoc();
        $task = $task["asum"];
        $this->stmtall("update mf_tasks set f_qty=$task where f_id={$this->params->f_task}");
        $this->result["data"] = $id;
        $this->echoResult();
    }

    public function workDetailsList()
    {
        $this->result["data"] = $this->stmtall("select * from mf_process_details where f_task={$this->params->f_taskid}")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
    public function addWorkerToDay($echoresult = true)
    {
        $sqlcheck = <<<EOD
        select f_id from mf_daily_workers where f_date='{$this->params->f_date}' and f_worker={$this->params->f_worker}
        EOD;
        if (empty($this->stmtall($sqlcheck)->fetch_assoc())) {
            $v["f_date"] = $this->params->f_date;
            $v["f_worker"] = $this->params->f_worker;
            $this->sinsert("mf_daily_workers", $v);
        }
        if ($echoresult) {
            $this->result["data"] = [];
            if (!empty($this->params->debug)) {
                $this->result["sqlcheck"] = $sqlcheck;
            }
            $this->echoResult();
        }
    }
    public function workerDetailsUpdate()
    {
        $v[$this->params->f_field] = $this->params->f_qty;
        $this->supdate("mf_process_details", $v, $this->params->f_id);
        $dt = $this->stmtall("select f_task, f_process from mf_process_details where f_id={$this->params->f_id}")->fetch_assoc();


        $task = $dt["f_task"];
        $dd = $this->stmtall("select sum(f_34p+f_36p+f_38p+f_40p+f_42p+f_44p+f_46p+f_48p+f_50p+f_52p) as asum from mf_process_details where f_task=$task")->fetch_assoc();
        $dd = $dd["asum"];
        $this->stmtall("update mf_tasks set f_qty={$dd} where f_id=$task");
        $this->result["data"] = [];
        $this->echoResult();
    }
    public function workerDetailsUpdateDone()
    {

        $sql = <<<EOD
        SELECT pd.f_color, pdd.f_processid, 
        coalesce(pd.f_34p, 0) as f_34p, 
        coalesce(pd.f_36p, 0) as f_36p, 
        coalesce(pd.f_38p, 0) as f_38p, 
        coalesce(pd.f_40p, 0) as f_40p, 
        coalesce(pd.f_42p, 0) as f_42p, 
        coalesce(pd.f_44p, 0) as f_44p,
        coalesce(pd.f_46p, 0) as f_46p, 
        coalesce(pd.f_48p, 0) as f_48p, 
        coalesce(pd.f_50p, 0) as f_50p, 
        coalesce(pd.f_52p, 0) as f_52p, 

        cast(coalesce(pdd.f_34d, 0) as int) as f_34d,
        cast(coalesce(pdd.f_36d, 0) as int) AS f_36d,
        cast(coalesce(pdd.f_38d, 0) as int) AS f_38d,
        cast(coalesce(pdd.f_40d, 0) as int) AS f_40d,
        cast(coalesce(pdd.f_42d, 0) as int) AS f_42d,
        cast(coalesce(pdd.f_44d, 0) as int) AS f_44d,
        cast(coalesce(pdd.f_46d, 0) as int) AS f_46d,
        cast(coalesce(pdd.f_48d, 0) as int) AS f_48d,
        cast(coalesce(pdd.f_50d, 0) as int) AS f_50d,
        cast(coalesce(pdd.f_52d, 0) as int) AS f_52d,

        coalesce(pdc.f_34c, 0) as f_34c,
        coalesce(pdc.f_36c, 0) AS f_36c,
        coalesce(pdc.f_38c, 0) AS f_38c,
        coalesce(pdc.f_40c, 0) AS f_40c,
        coalesce(pdc.f_42c, 0) AS f_42c,
        coalesce(pdc.f_44c, 0) AS f_44c,
        coalesce(pdc.f_46c, 0) AS f_46c,
        coalesce(pdc.f_48c, 0) AS f_48c,
        coalesce(pdc.f_50c, 0) AS f_50c,
        coalesce(pdc.f_52c, 0) AS f_52c,

        coalesce(pdc.f_id, 0) AS f_id, 1000 as f_check 
        FROM mf_process_details pd 

        left join (select f_id, f_color, f_processid,
            sum(f_34d) as f_34d, 
            sum(f_36d) as f_36d, 
            sum(f_38d) as f_38d, 
            sum(f_40d) as f_40d, 
            sum(f_42d) as f_42d, 
            sum(f_44d) as f_44d, 
            sum(f_46d) as f_46d, 
            sum(f_48d) as f_48d, 
            sum(f_50d) as f_50d, 
            sum(f_52d) as f_52d 
        from mf_process_details_done where f_taskid={$this->params->f_task} and f_processid in 
            (select f_id from mf_daily_process where f_product=(select f_product from mf_tasks where f_id={$this->params->f_task}) and f_process={$this->params->f_process}) group by 2) 
            pdd on pd.f_color=pdd.f_color 

        left join (select f_id, f_color, f_processid,
            f_34d as f_34c, 
            f_36d as f_36c,
            f_38d as f_38c, 
            f_40d as f_40c, 
            f_42d as f_42c,
            f_44d as f_44c, 
            f_46d as f_46c, 
            f_48d as f_48c, 
            f_50d as f_50c, 
            f_52d as f_52c 
        from mf_process_details_done where f_taskid={$this->params->f_task}) pdc on pd.f_color=pdc.f_color and pdc.f_processid={$this->params->f_dailyid} 

        WHERE pd.f_task={$this->params->f_task}
        order by 1 
        EOD;
        $ja = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

        for ($i = 0; $i < count($ja); $i++) {
            $jc = $ja[$i];
            if ($jc["f_id"] == 0) {
                $id = 0;
                $v["f_taskid"] = $this->params->f_task;
                $v["f_processid"] = $this->params->f_dailyid;
                $v["f_color"] = $jc["f_color"];
                $id = $this->sinsert("mf_process_details_done", $v);
                $jc["f_id"] = $id;
                $ja[$i] = $jc;
            }
        }
        $this->result["data"] = $ja;
        $this->echoResult();
    }
    public function workerDetailDone()
    {
        $id = empty($this->params->f_id) ? 0 : $this->params->f_id;
        if ($id == 0) {
            $v["f_taskid"] = $this->params->f_taskid;
            $v["f_processid"] = $this->params->f_dailyid;
            $v["f_color"] = $this->params->f_color;
            $this->sinsert("mf_process_details_done", $v);
        }
        $this->stmtall("update mf_process_details_done set {$this->params->f_field}d =coalesce({$this->params->f_field}d, 0)+{$this->params->f_qty} where f_id=$id");
        $this->stmtall("update mf_daily_process set f_qty=f_qty+{$this->params->f_qty} where f_id={$this->params->f_dailyid}");
        $this->result["data"] = $id;
        $this->echoResult();
    }
    public function workerDetailsUndone()
    {

        if ($this->params->f_id == 0) {
            $v["f_taskid"] = $this->params->f_taskid;
            $v["f_processid"] = $this->params->f_dailyid;
            $v["f_color"] = $this->params->f_color;
            $this->params->f_id = $this->sinsert("mf_process_details_done", $v);
        }
        $this->stmtall("update mf_process_details_done set {$this->params->f_field}d=0 where f_id={$this->params->f_id}");
        $this->stmtall("update mf_daily_process set f_qty=f_qty-{$this->params->f_qty} where f_id={$this->params->f_dailyid}");
        $this->result["data"] = [];
        $this->echoResult();
    }
    public function workArrayDone()
    {
        $arr = $this->params->arr;
        $doneArr = [];
        for ($i = 0; $i < count($arr); $i++) {
            $jo = (array) $arr[$i];
            $id = $jo["f_id"];
            if ($id == 0) {
                $v["f_taskid"] = $jo["f_taskid"];
                $v["f_processid"] = $jo["f_dailyid"];
                $v["f_color"] = $jo["f_color"];
                $id = $this->sinsert("mf_process_details_done", $v);
            }

            $this->stmtall("update mf_process_details_done set {$jo["f_field"]}d=coalesce({$jo["f_field"]}d, 0)+{$jo["f_qty"]} where f_id=$id");
            $this->stmtall("update mf_daily_process set f_qty=f_qty+{$jo["f_qty"]} where f_id={$jo["f_dailyid"]}");
            array_push($doneArr, $jo);
        }
        $this->result["data"] = $doneArr;
        $this->echoResult();
    }
}

if (!empty($params->query)) {
    $ddbb = new IndexPhp();
    switch ($params->query) {
        case 1:
            $ddbb->getListOfTask();
            break;
        case 2:
            $ddbb->getListOfTeamLead();
            break;
        case 3:
            $ddbb->getListOfEmployee();
            break;
        case 4:
            $ddbb->getListOfWorks();
            break;
        case 5:
            $ddbb->listOfTaskWorks();
            break;
        case 6:
            $ddbb->addWorkToTask();
            break;
        case 7:
            $ddbb->employesOfTheDay();
            break;
        case 8:
            $ddbb->addWorkerToDay();
            break;
        case 10:
            $ddbb->removeWork();
            break;
        case 12:
            $ddbb->workDetails();
            break;
        case 13:
            $ddbb->workDetailsList();
            break;
        case 14:
            $ddbb->workerDetailsUpdate();
            break;
        case 15:
            $ddbb->workerDetailsUpdateDone();
            break;
        case 16:
            $ddbb->workerDetailDone();
            break;
        case 19:
            $ddbb->workerDetailsUndone();
            break;
        case 20:
            $ddbb->workArrayDone();
            break;
    }
}