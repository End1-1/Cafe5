<?php
require_once(__DIR__. "/hotel.php");

$entry = strtotime($params->entry);
$departure = strtotime($params->departure);
$today = strtotime(date("Y-m-d", time()));

if ($entry < $today){
    exitError(tr("Invalid entry date"));
}
if ($entry > $departure) {
    exitError(tr("Date of entry cannot be greater than date of departure"));
}

// $sql = "select r.f_id, r.f_state, r.f_reserveState, r.f_room, rm.f_short, r.f_cardex, r.f_startDate, r.f_endDate, "
//     . "concat(g.f_firstName, ' ', g.f_lastName) as f_guest_name, c.f_name as f_cardex_name, r.f_invoice,  "
//     . "rs.f_en as f_reserveStateName, a.amount, coalesce(cc.credit, 0) as credit, coalesce(cd.debet, 0) as debet, "
//     . "r.f_remarks, r.f_arrangement, r.f_author, r.f_group, "
//     . "r.f_created, r.f_createTime, rm.f_donotdisturbe, r.f_chmstatus "
//     . "from f_reservation r "
//     . "left join f_room rm on rm.f_id=r.f_room "
//     . "left join f_guests g on r.f_guest=g.f_id "
//     . "inner join f_reservation_status rs on rs.f_id=r.f_reserveState "
//     . "left join f_cardex c on c.f_cardex=r.f_cardex "
//     . "left join (select f_inv, sum(f_amountAmd) as amount from m_register where f_source='AV' and f_canceled=0 and f_finance=1) a on  a.f_inv=r.f_invoice "
//     . "left join (select f_inv, sum(f_amountAmd) as credit from m_register where f_sign=1 and f_canceled=0 and f_finance=1 group by 1) cc on cc.f_inv=r.f_invoice "
//     . "left join (select f_inv, sum(f_amountAmd) as debet from m_register where f_sign=-1 and f_canceled=0 and f_finance=1 group by 1) cd on cd.f_inv=r.f_invoice "
//     . "where (r.f_state in (1,2) or (r.f_state in (4, 9, 7) and r.f_endDate>=current_date)) ";

//     $result = hstmtall($sql);
//     foreach ($result as $r) {
        
//     }


    // rg->fModel->setColumn(80, "", tr("Total"))
    //         .setColumn(80, "", tr("Confirmed"))
    //         .setColumn(80, "", tr("Guaranted"))
    //         .setColumn(80, "", tr("Tentative"))
    //         .setColumn(80, "", tr("Waiting"))
    //         .setColumn(80, "", tr("Block"))
    //         .setColumn(80, "", tr("CheckIn"))
    //         .setColumn(80, "", tr("Still to sell"));
    $sql = "select rc.f_short, rc.f_description, count(rm.f_id) "
           . "from f_room rm "
           . "left join f_room_classes rc on rc.f_id=rm.f_class "
           . "left join f_room_view rv on rv.f_id=rm.f_view "
           . "group by rc.f_short,rc.f_description";
    $map = [];
    $data = [];
    //QMap<QString, int> map;
    $rows = hqueryStr($sql);
    $mapName = "";
    while ($row = $rows->fetch_row()) {
        $onerow = [];
        $col = 0;
        $onerow[$col] = $row[$col];
        $mapName = $row[0] . $row[1];
        $col++;
        $onerow[$col] = $row[$col];
        $col++;
        $onerow[$col] = $row[$col];
        $col++;
        $onerow[$col] = 0;
        $col++;
        $onerow[$col] = 0;
       $map[$mapName] = count($data);
        array_push($data, $onerow);
    }
    
    $sql = "select rc.f_short, rc.f_description,  r.f_reserveState, r.f_state, r.f_room "
            . "from f_reservation r "
            . "left join f_room rm on rm.f_id=r.f_room "
            . "left join f_room_classes rc on rc.f_id=rm.f_class "
            . "left join f_room_view rv on rv.f_id=rm.f_view "
            . "where  r.f_state in (1,2) and "
            . "((r.f_startdate <='$params->entry' and  r.f_enddate > '$params->entry') "
            . "or (r.f_startdate >= '$params->entry' and r.f_enddate < '$params->departure')  "
            . "or (r.f_startdate <'$params->departure' and r.f_enddate>'$params->departure')) ";
    $rows = hqueryStr($sql);
    $rooms = [];
    while ($row = $rows->fetch_row()) {
        $mapName = $row[0] . $row[1];
        $col = 2;
        
        if (in_array($row[$col + 2], $rooms)) {
            continue;
        }
        array_push($rooms, $row[$col + 2]);
        $r = $map[$mapName];
        $dataCol = 0;
        if ($row[$col + 1] == 1) {
            $dataCol = 4 ;
        } else {
            $dataCol = 3  + 1;
        }
        $data[$r][$dataCol] =  $data[$r][$dataCol] + 1;
        //rg->fModel->setData(row, dataCol, rg->fModel->data(row, dataCol, Qt::DisplayRole).toDouble() + 1);
    }

    printResult(1, $data);