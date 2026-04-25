<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-31 00:47:46
# Last modified - 2025-03-31 00:47:49

class goods extends PClass {
    public function semireadyPriceUpdate() {
        $goodslist = $this->stmtall("select distinct(f_base) from c_goods_complectation")->fetch_all();
        foreach ($goodslist as $g) {
            $sql = <<<EOD
            update c_goods set f_lastinputprice = 
            (select sum(gg.f_lastinputprice * gc.f_qty) 
            from c_goods_complectation gc
            left join c_goods gg on gc.f_goods=gg.f_id
            where gc.f_base=?)
            where f_id=?
            EOD;
            $this->stmtall($sql, "ss", [$g[0], $g[0]]);
        }
        $this->echoResult();
    }
}