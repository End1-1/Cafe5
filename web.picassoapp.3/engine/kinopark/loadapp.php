<?php
#(C) 02/02/2025 Kudryashov Vasili
require_once __DIR__ . "/index.php";
class LoadApp extends PClass
{
   public function echo()
   {

      $sql = <<<EOD
   select distinct(p1.f_id), p1.f_name, coalesce(p1.f_image, '') as f_image
   from d_menu m
   inner join d_dish d on d.f_id=m.f_dish
   inner join d_part2 p2 on p2.f_id=d.f_part
   inner join d_part1 p1 on p1.f_id=p2.f_part
   where m.f_state=1
   EOD;
      $part1 = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

      $sql = <<<EOD
      SELECT DISTINCT(p.f_id) AS f_id, 
      pp.f_name AS f_parentnamehy, 
      t2.f_en AS f_parentnameen, 
      t2.f_ru AS f_parentnameru, 
      p.f_part, p.f_name, coalesce(p.f_image,'') as f_image, t.f_en, t.f_ru
      from d_menu m
      left join d_dish d on d.f_id=m.f_dish
      left join d_part2 p on p.f_id=d.f_part
      LEFT JOIN d_part2 pp ON p.f_parent=pp.f_id
      left join d_translator t on t.f_id=p.f_id AND t.f_mode=2
      LEFT JOIN d_translator t2 ON t2.f_id=pp.f_id AND t.f_mode=2
      where m.f_state=1
      order by pp.f_queue, p.f_queue
    EOD;
      $part2 = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

      $sql = <<<EOD
    select d.f_id, d.f_part, d.f_name, md.f_price, coalesce(d.f_image, '') as f_image, md.f_store,
    coalesce(md.f_print1, '') as f_print1, coalesce(md.f_print2, '') as f_print2, md.f_price, d.f_netweight, 1 as f_qty,
    t.f_ru, t.f_en,
    d.f_comment as f_descriptionhy, t2.f_en as f_descriptionen, t2.f_ru as f_descriptionru
    from d_menu md 
    left join d_dish d on d.f_id=md.f_dish 
    left join d_translator t on t.f_id=d.f_id and t.f_mode=1
    left join d_translator t2 on t2.f_id=d.f_id and t2.f_mode=3
    where md.f_state=1 and md.f_menu=1
    order by d.f_queue
    EOD;
      $menu = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);


      $sql = <<<EOD
      SELECT JSON_OBJECT('dish', d.f_dish, 'comments', json_arrayagg(d.f_comment)) as d
      FROM d_special d
      GROUP BY d.f_dish
      EOD;
      $dishspecial = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

      $sql = <<<EOD
      select json_object('f_part1', p2.f_part, 'recent', json_arrayagg(m.f_dish)) as d
      from d_menu m
      left join d_dish dd on dd.f_id=m.f_dish
      left join d_part2 p2 on p2.f_id=dd.f_part
      where m.f_state=1 and m.f_recent=1
      group by p2.f_part
      EOD;
      $recent = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

      $this->result["part1"] = $part1;
      $this->result["part2"] = $part2;
      $this->result["menu"] = $menu;
      $this->result["recent"] = $recent;
      $this->result["dishspecial"] = $dishspecial;
      $this->echoResult();
   }
}

$la = new LoadApp();
$la->echo();