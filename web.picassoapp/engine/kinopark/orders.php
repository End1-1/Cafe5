<?php
require_once __DIR__ . "/index.php";

class KinoParkNewOrders extends PClass {
    public function get() {
        $sql = <<<EOD
        SELECT JSON_OBJECT('id', JSON_OBJECT('table', t.f_name, 'header', ob.f_header, 'headercomment', oh.f_comment,
        'hallid', concat(oh.f_prefix, oh.f_hallid), 
        'totalamount', oh.f_amounttotal, 'cashamount', oh.f_amountcash, 'cardamount', oh.f_amountcard, 'idramamount', oh.f_amountidram,
        'servicefactor', oh.f_servicefactor * 100, 'serviceamount', oh.f_amountservice,
        'dishes', json_arrayagg(JSON_OBJECT('obid', ob.f_id, 'part1', p1.f_name, 'dishname', d.f_name, 
        'qty1', ob.f_qty1,'qty2', ob.f_qty2, 'price', ob.f_price, 'dishcomment', ob.f_comment, 
        'f_print1', ob.f_print1, 'f_print2', ob.f_print2,'store', ob.f_store) ORDER BY p1.f_name desc))) as ord
        FROM o_body ob 
        LEFT JOIN o_header oh ON oh.f_id=ob.f_header 
        LEFT JOIN h_tables t ON t.f_id=oh.f_table
        LEFT JOIN d_dish d ON d.f_id=ob.f_dish
        left join d_part2 p2 on p2.f_id=d.f_part
        left join d_part1 p1 on p1.f_id=p2.f_part
        WHERE oh.f_state=2 and (ob.f_print1='{$this->params->printer}' or ob.f_print2='{$this->params->printer}')
        GROUP BY ob.f_header
        EOD;
        $this->result["orders"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function complete() {
        foreach ($this->params->ids  as $id => $prn) {
            $sql = "update o_body set {$prn}=concat({$prn}, '---PRINTED'), f_printtime=current_timestamp, f_printuser={$this->userid} where f_id='$id'";
            $this->result[$id] = "done";
            $this->stmtall($sql);
        }
        $this->result["ids"] = $this->params->ids;
        $this->echoResult();
    }

    public function status() {
        
    }
}



if (!empty($params->action)) {
    $kp = new KinoParkNewOrders();
    $kp->{$params->action}();
}