<?php
# (C) 2024-2025 Kudryashov Vasili
# Created - 25/02/2025 
# Last modified - 2025-07-14 17:51:57
require_once __DIR__ . "/reports.php";
class Effectivness extends Report
{
    protected function hiddenCols()
    {
        return [0];
    }
    protected function columns()
    {
        if (empty($this->params->view)) {
            return [
                $this->tr("Goods code"),
                $this->tr("Group"),
                $this->tr("Goods"),
                $this->tr("Barcode"),
                $this->tr("Begin store"),
                $this->tr("Begin shop"),
                $this->tr("Input"),
                $this->tr("Sale"),
                $this->tr("Output"),
                $this->tr("Final store"),
                $this->tr("Final shop"),
                $this->tr("Effectiveness"),
                $this->tr("Delta"),
                $this->tr("Whosale price"),
                $this->tr("Retail price"),
                $this->tr("Retail discounted")
            ];
        } else {
            return [
                $this->tr("Group code"),
                $this->tr("Group"),
                $this->tr("Begin store"),
                $this->tr("Begin shop"),
                $this->tr("Input"),
                $this->tr("Sale"),
                $this->tr("Output"),
                $this->tr("Final store"),
                $this->tr("Final shop"),
                $this->tr("Effectiveness"),
                $this->tr("Delta"),
                $this->tr("Whosale price"),
                $this->tr("Retail price"),
                $this->tr("Retail discounted")
            ];
        }
    }
    protected function widget()
    {
        return ["icon" => "cash.png", "title" => $this->tr("Effectivness")];
    }
    protected function filter()
    {
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "d1"],
            ["type" => "date", "title" => tr("Date of end"), "field" => "d2"],
            ["type" => "keyvalue", "title" => $this->tr("Point of sale"), "field" => "pos", "filter" => "storages"],
            ["type" => "keyvalue", "title" => $this->tr("Group"), "field" => "gr", "filter" => "goodsgroup"],
            ["type" => "checkbox", "title" => tr("Groupping"), "field" => "view"]
        ];
    }
    protected function rows()
    {

        if (empty($this->params->d1)) {
            $this->params->d1 = date("Y-m-d");
        }
        if (empty($this->params->d2)) {
            $this->params->d2 = date("Y-m-d");
        }
        if (empty($this->params->view)) {
            return $this->getGoods();
        } else {
            return $this->getGroup();
        }
    }

    protected function sumColumns() {}

    private function getGoods()
    {
        $where = "";
        $join  = "";
        if (!empty($this->params->pos)) {
            $where .= " and s.f_store in ({$this->params->pos}) ";
        }
        if (!empty($this->params->gr)) {
            $where .= " and g.f_group in({$this->params->gr}) ";
            $join .= " inner join c_goods g on g.f_id=s.f_goods ";
        }
        $sql = <<<EOD
        create temporary table t_rep (f_goods int, f_groupname tinytext, f_goodsname tinytext, f_scancode tinytext, 
        f_startqty float, f_input float, f_sale float, f_output float, f_final float,f_effectiveness float, f_delta float,
        f_wholesale float, f_retail float, f_retaildisc float);
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        insert into t_rep 
        select g.f_id, gr.f_name, g.f_name, g.f_scancode, 0, 0, 0, 0, 0, 0, 0 ,
        gp.f_price2, gp.f_price1, coalesce(gp.f_price1disc, 0)
        from c_goods g
        left join c_goods_prices gp on gp.f_goods=g.f_id and gp.f_currency=1
        left join c_groups gr on gr.f_id=g.f_group 
        EOD;
        $this->db->stmtall($sql);
        #begin qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date < '{$this->params->d1}' $where
        group by 1) s on s.f_goods = t.f_goods
        set t.f_startqty=COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        #input qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date between '{$this->params->d1}' and '{$this->params->d2}' and s.f_type=1 $where
        group by 1) s on s.f_goods = t.f_goods
        set t.f_input=COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        #sale qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_sign) as f_qty,sum(s.f_total*s.f_sign) as f_total 
        from o_goods s 
        inner join o_header h on h.f_id=s.f_header 
        $join
        where h.f_datecash between '{$this->params->d1}' and '{$this->params->d2}' $where
        group by 1) s on s.f_goods = t.f_goods
        set t.f_sale=COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        #other output
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date between '{$this->params->d1}' and '{$this->params->d2}' and s.f_type=-1 and s.f_reason<>4 $where
        group by 1) s on s.f_goods = t.f_goods
        set t.f_output=COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        #final qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date <= '{$this->params->d2}' $where
        group by 1) s on s.f_goods = t.f_goods
        set t.f_final=COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        #calculate effectiveness
        $sql = <<<EOD
        update t_rep set f_effectiveness=if ((f_startqty+f_input)<>0, (f_sale*100)/(f_startqty+f_input), 0)
        EOD;
        $this->db->stmtall($sql);
        #calculate delta
        $sql = <<<EOD
        update t_rep set f_delta = if(f_startqty, ((f_final*100)/f_startqty)-100, 0);
        EOD;
        $this->db->stmtall($sql);
        #final report
        $this->db->stmtall("delete from t_rep where f_startqty=0 and f_input=0 and f_sale=0 and f_output=0");
        return $this->db->stmtall("select* from t_rep")->fetch_all();
    }

    private function getGroup()
    {
        $where = "";
        $whereStore = "";
        $join  = "";
        if (!empty($this->params->pos)) {
            $whereStore .= " {$this->params->pos} ";
        } else {
            $whereStore .= " 5,24,2,3 ";
        }
        if (!empty($this->params->gr)) {
            $where .= " and g.f_group in({$this->params->gr}) ";
        }
        $join .= " inner join c_goods g on g.f_id=s.f_goods ";

        $sql = <<<EOD
        create temporary table t_rep (f_goods int, f_groupname tinytext, f_goodsname tinytext, f_scancode tinytext, 
        f_start2 FLOAT, f_startqty float, f_input float, f_sale float, f_output float, 
        f_final FLOAT, f_final2 FLOAT, f_effectiveness float, f_delta float,
        f_wholesale float, f_retail float, f_retaildisc float);    
        EOD;
        $this->db->stmtall($sql);

        $sql = <<<EOD
        insert into t_rep 
        select g.f_id, gr.f_name, g.f_name, g.f_scancode, 
        0, 0, 0, 0, 0, 0, 0 ,0,0,
        gp.f_price2, gp.f_price1, coalesce(gp.f_price1disc, 0)
        from c_goods g
        left join c_goods_prices gp on gp.f_goods=g.f_id and gp.f_currency=1
        left join c_groups gr on gr.f_id=g.f_group 
        EOD;
        $this->db->stmtall($sql);


        #begin qty store
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date < '{$this->params->d1}' and g.f_unit<>10 and s.f_store in (28,16,8) 
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_startqty=t.f_startqty+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type) as f_qty,sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date < '{$this->params->d1}' AND g.f_unit=10 and s.f_store in (28,16,8)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_start2=t.f_start2+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);




        #begin qty shop
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date < '{$this->params->d1}' and g.f_unit<>10 and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_startqty=t.f_startqty+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type) as f_qty,sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date < '{$this->params->d1}' AND g.f_unit=10 and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_startqty=t.f_startqty+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);


        #input qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date between '{$this->params->d1}' and '{$this->params->d2}' and s.f_type=1 and g.f_unit<>10 and s.f_store in (16)
        group by 1
         ) 
        s on s.f_goods = t.f_goods
        set t.f_input=t.f_input+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);



        #sale qty
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type*-1) as f_qty,sum(s.f_total*s.f_type*-1) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date between '{$this->params->d1}' and '{$this->params->d2}' 
        and  s.f_reason=4 and g.f_unit<>10 and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_sale=t.f_sale+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
         update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type*-1) as f_qty,
        sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty*-1) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date  between '{$this->params->d1}' and '{$this->params->d2}' 
        and  s.f_reason=4 and g.f_unit=10 and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_sale=t.f_sale+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);


        #other output
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date between '{$this->params->d1}' and '{$this->params->d2}' and s.f_type=-1 and s.f_reason<>4 and g.f_unit<>10 and s.f_store in ($whereStore, 28,16,8)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_output=t.f_output+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type) as f_qty,sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date  between '{$this->params->d1}' and '{$this->params->d2}' and s.f_type=1 and s.f_reason<>4 and g.f_unit=10 and s.f_store in ($whereStore, 28,16,8)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_output=t.f_output+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);



        #final qty store
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date <= '{$this->params->d2}' and g.f_unit<>10 and s.f_store in (28,16,8)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_final2=t.f_final2+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type) as f_qty,
        sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date <= '{$this->params->d2}' AND g.f_unit=10 and s.f_store in (28,16,8)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_final2=t.f_final2+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);



        #final qty shop
        $sql = <<<EOD
        update t_rep t
        left join (
        select s.f_goods, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        $join
        where h.f_date <= '{$this->params->d2}' and g.f_unit<>10  and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_final=t.f_final+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);
        $sql = <<<EOD
        update t_rep t
        left join (
        SELECT  gc.f_goods,  sum(gc.f_qty*s.f_qty*s.f_type) as f_qty,
        sum(ggc.f_lastinputprice*s.f_type*gc.f_qty*s.f_qty) as f_total 
        from a_store s 
        inner join a_header h on h.f_id=s.f_document 
        inner join c_goods g on g.f_id=s.f_goods
        INNER JOIN c_goods_complectation gc ON gc.f_base=g.f_id
        LEFT JOIN c_goods ggc ON ggc.f_id=gc.f_goods
        where h.f_date <= '{$this->params->d2}' AND g.f_unit=10  and s.f_store in ($whereStore)
        group by 1
        ) s on s.f_goods = t.f_goods
        set t.f_final=t.f_final+COALESCE(s.f_qty, 0)
        EOD;
        $this->db->stmtall($sql);


        #calculate effectiveness
        $sql = <<<EOD
        update t_rep set f_effectiveness=if ((f_startqty+f_input+f_start2)<>0, (f_sale*100)/(f_startqty+f_input+f_start2), 0)
        EOD;
        $this->db->stmtall($sql);
        #calculate delta
        $sql = <<<EOD
        update t_rep set f_delta = if(f_startqty+f_start2>0, ((f_final*100)/(f_startqty+f_start2))-100, 0);
        EOD;
        $this->db->stmtall($sql);
        #final report
        $this->db->stmtall("delete from t_rep where f_start2=0 and f_startqty=0 and f_input=0 and f_sale=0 and f_output=0 and f_final=0 and f_final2=0");
        $s = <<<EOD
        select 0, f_groupname , sum(f_start2) as f_startstore,
        sum(f_startqty), sum(f_input) , sum(f_sale ), sum(f_output) , 
        sum(f_final) , sum(f_final2) as f_finalstore,
        sum(f_effectiveness) , sum(f_delta ),
        f_wholesale , f_retail , f_retaildisc  from t_rep 
        group by f_groupname, f_wholesale, f_retail, f_retaildisc
        EOD;
        return $this->db->stmtall($s)->fetch_all();
    }
}

$e = new Effectivness();
$e->echoResult();
