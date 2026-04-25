<?php
require_once __DIR__ . "/../app.php";

class CreateGroupDiscount extends PClass
{
    public function __construct()
    {
        parent::__construct();
    }

    public function createGroupDiscount()
    {
        $v["f_name"] = $this->params->name;
        $v["f_body"] = "{}";
        $v["f_state"] = 1;
        $this->result["id"] = $this->sinsert("c_goods_price_order", $v);
        $this->result["name"] = $this->params->name;
        $this->echoResult();
    }

    public function getAll()
    {
        $all = $this->stmtall("select  * from c_goods_price_order")->fetch_all(MYSQLI_ASSOC);
        #check current prices, update new items, etc...
        foreach ($all as &$a) {
            $body = json_decode($a["f_body"]);
            if (property_exists($body, "groups")) {
                foreach ($body->groups as $group) {
                    $group->qty = $this->getQtyOfGroup($group->f_id);
                    $sql = <<<EOD
                SELECT g.f_id, g.f_name, gp.f_price1, gp.f_price2, gp.f_price1disc, gp.f_price2disc, g.f_scancode
                FROM c_goods g 
                INNER JOIN c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1
                WHERE g.f_group=?
                EOD;
                    $check = $this->stmtall($sql, "i", [$group->f_id])->fetch_all(MYSQLI_ASSOC);
                    foreach ($check as $c) {
                        for ($i = 0; $i < count($group->items); $i++) {
                            $found = false;
                            if ($group->items[$i]->f_id == $c["f_id"]) {
                                $found = true;
                                $group->items[$i]->f_price1 = $c["f_price1"];
                                $group->items[$i]->f_price2 = $c["f_price2"];
                                break;
                            }
                            if ($found) {

                            }
                        }
                    }
                }
                $a["f_body"] = json_encode($body, JSON_UNESCAPED_UNICODE);
            }
        }
        $this->result["all"] = $all;
        $this->echoResult();
    }

    public function getGroupItems()
    {
        $this->result["itemid"] = $this->params->item;
        $this->result["group"] = $this->params->group;
        $this->result["groupname"] = $this->params->groupname;
        $sql = <<<EOD
        select c.f_id, c.f_name, c.f_scancode, 
        cp.f_price1, cp.f_price2, 
        coalesce(cp.f_price1disc, 0) as f_price1disc,
        coalesce(cp.f_price2disc, 0) f_price2disc
        from c_goods c 
        left join c_goods_prices cp on cp.f_goods=c.f_id and cp.f_currency=1 
        where c.f_group=?
        EOD;
        $this->result["items"] = $this->stmtall($sql, "i", [$this->params->group])->fetch_all(MYSQLI_ASSOC);
        $this->result["qty"] = $this->getQtyOfGroup($this->params->group);
        if (!empty($this->result["items"])) {
            $this->result["retail"] = $this->result["items"][0]["f_price1"];
            $this->result["whosale"] = $this->result["items"][0]["f_price2"];
        }
        $this->echoResult();
    }

    public function save()
    {
        $v["f_name"] = $this->params->name;
        $v["f_body"] = $this->params->body;
        $v["f_state"] = 1;
        $this->supdate("c_goods_price_order", $v, $this->params->itemid);
        $this->result["result"] = "not implemented";
        $this->echoResult();
    }

    public function discount()
    {
        $v["f_name"] = $this->params->name;
        $v["f_body"] = $this->params->body;
        $v["f_state"] = 1;
        $this->supdate("c_goods_price_order", $v, $this->params->itemid);
        $body = json_decode($this->params->body);
        //var_dump($body);
        foreach ($body->groups as $b) {
            foreach ($b->items as $i) {
                $this->stmtall("update c_goods_prices set f_price1disc=?, f_price2disc=? where f_goods=? and f_currency=1", "ddi", [
                    $i->f_price1disc,
                    $i->f_price2disc,
                    $i->f_id
                ]);
            }
        }
        $this->echoResult();
    }

    public function rollback()
    {
        $v["f_name"] = $this->params->name;
        $v["f_body"] = $this->params->body;
        $v["f_state"] = 1;
        $this->supdate("c_goods_price_order", $v, $this->params->itemid);
        $body = json_decode($this->params->body);
        //var_dump($body);
        foreach ($body->groups as $b) {
            foreach ($b->items as $i) {
                $this->stmtall("update c_goods_prices set f_price1disc=0, f_price2disc=0 where f_goods=? and f_currency=1", "i", [
                    $i->f_id
                ]);
            }
        }
        $this->echoResult();
    }

    public function refreshSaleStore()
    {
        $sql = <<<EOD
        SELECT g.f_group, SUM(s.f_qty*s.f_type) + s2.f_qty as f_qty
        FROM a_store s
        LEFT JOIN c_goods g ON g.f_id=s.f_goods
        left JOIN (SELECT g2.f_group, coalesce(sum(s.f_qty*s.f_type*c.f_qty),0) AS f_qty
        FROM a_store s
        LEFT JOIN c_goods g1 ON g1.f_id=s.f_goods
        LEFT JOIN c_goods_complectation c ON c.f_base=g1.f_id
        LEFT JOIN c_goods g2 ON g2.f_id=c.f_goods
        WHERE g2.f_group in (?)
        ) s2 ON s2.f_group=g.f_group
        WHERE g.f_group in (?)
        EOD;
        $this->result["store"] = $this->stmtall($sql, "ss", [$this->params->groups, $this->params->groups])->fetch_all();
        $this->result["sale"] = $this->stmtall("select f_group, sum(s.f_sign*f_qty) "
            . "from o_goods s "
            . "left join o_header h on h.f_id=s.f_header "
            . "left join c_goods g on g.f_id=s.f_goods "
            . "where h.f_datecash between ? and ? and g.f_group in ({$this->params->groups}) "
            . "group by 1 ", "ss", [$this->params->date1, $this->params->date2])->fetch_all();
        $this->echoResult();
    }

    public function editLwName()
    {
        $this->result["id"] = $this->params->id;
        $this->result["name"] = $this->params->name;
        $this->stmtall("update c_goods_price_order set f_name=? where f_id=?", "si", [$this->params->name, $this->params->id]);
        $this->echoResult();
    }

    public function removeLwName()
    {
        $this->result["id"] = $this->params->id;
        $this->stmtall("delete from c_goods_price_order where f_id=?", "i", [$this->params->id]);
        $this->echoResult();
    }

    public function getQtyOfGroup($group)
    {
        $sql = <<<EOD
        SELECT g.f_group, SUM(s.f_qty*s.f_type) + s2.f_qty as f_qty
        FROM a_store s
        LEFT JOIN c_goods g ON g.f_id=s.f_goods
        left JOIN (SELECT g2.f_group, coalesce(sum(s.f_qty*s.f_type*c.f_qty),0) AS f_qty
        FROM a_store s
        LEFT JOIN c_goods g1 ON g1.f_id=s.f_goods
        LEFT JOIN c_goods_complectation c ON c.f_base=g1.f_id
        LEFT JOIN c_goods g2 ON g2.f_id=c.f_goods
        WHERE g2.f_group=?
        ) s2 ON s2.f_group=g.f_group
        WHERE g.f_group=?
        EOD;
        $row = $this->stmtall($sql, "ii", [$group, $group])->fetch_assoc();
        return $row["f_qty"];
    }
}

if (!empty($params->action)) {
    $c = new CreateGroupDiscount();
    $c->{$params->action}();
}