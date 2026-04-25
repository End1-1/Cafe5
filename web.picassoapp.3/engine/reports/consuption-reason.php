<?php
#(C) 05/02/2025 Kudryashov Vasili
require_once __DIR__ . "/reports.php";

class ConsuptionReason extends Report
{
    public function __construct()
    {
        parent::__construct();

    }

    protected function columns()
    {
        return [
            $this->tr("Date"),
            $this->tr("Order number"),
            $this->tr("Group of dish"),
            $this->tr("Dish name"),
            $this->tr("Goods"),
            $this->tr("Store"),
            $this->tr("Qty of recipe"),
            $this->tr("Qty of sale"),
            $this->tr("Total qty")
        ];
    }
    protected function filter()
    {
        return [

        ];
    }
    protected function rows()
    {
        $sql = <<<EOD
        select oh.f_datecash,concat(oh.f_prefix, oh.f_hallid), dp.f_name as f_partname,d.f_name as f_dishname,
        g.f_name as f_goodsname,st.f_name as f_storename,
        og.f_qty as f_recipeqty,sum(ob.f_qty1) as f_saleqty, sum(og.f_qty*ob.f_qty1) as f_output 
        from o_body ob 
        left join o_header oh on oh.f_id=ob.f_header 
        left join d_dish d on d.f_id=ob.f_dish 
        left join d_part2 dp on dp.f_id=d.f_part 
        left join d_recipes og on og.f_dish=ob.f_dish 
        left join c_goods g on g.f_id=og.f_goods 
        left join c_storages st on st.f_id=ob.f_store   
        where oh.f_datecash between ? and ? 
        and oh.f_state=2
        and ob.f_state in (1,3)  and ob.f_store =?  
        and og.f_goods =?  
        group by 1,2,3,4,5
        union 
        SELECT oh.f_datecash,concat(oh.f_prefix, oh.f_hallid), dp.f_name as f_partname,d.f_name as f_dishname,
        g.f_name as f_goodsname,st.f_name as f_storename,
        og.f_qty as f_recipeqty,sum(ob.f_qty1) as f_saleqty, sum(og.f_qty*ob.f_qty1*ds.f_qty) as f_output 
        from o_body ob 
        left join o_header oh on oh.f_id=ob.f_header 
        left join d_dish d on d.f_id=ob.f_dish 
        left join d_part2 dp on dp.f_id=d.f_part 
        inner join d_dish_set ds on ds.f_id=ob.f_dish 
        left join d_recipes og on og.f_dish=ds.f_part 
        left join c_goods g on g.f_id=og.f_goods 
        left join c_storages st on st.f_id=ob.f_store   
        where oh.f_datecash between ? and ? 
        and oh.f_state=2
        and ob.f_state in (1,3)  
        and ob.f_store =? and og.f_goods =?  
        group by 1,2,3,4,5 
        order by 1,2
        EOD;
        return $this->db->stmtall(
            $sql,
            "ssiissii",
            [
                $this->params->date1,
                $this->params->date2,
                $this->params->store,
                $this->params->goods,
                $this->params->date1,
                $this->params->date2,
                $this->params->store,
                $this->params->goods
            ]
        )->fetch_all();
    }

    protected function sumColumns()
    {
        return ["6"=>0,"7"=>0,"8"=>0];
    }
    protected function widget()
    {
        return ["icon" => "cash.png", "title" => $this->tr("Consuption reason")];
    }
}

$out = new ConsuptionReason();
$out->echoResult();