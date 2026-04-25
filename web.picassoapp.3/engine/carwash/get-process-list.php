<?php
require_once("carwash.php");

function getProcessListData($where, $orderBy)
{
    $sql = "
        SELECT 
            o.f_id, 
            ohp.f_state AS progress, 
            o.f_state, 
            ohp.f_startwash AS f_washdate,
            ohp.f_washtime,
            ohp.f_startdry AS f_drydate,
            ohp.f_drytime,
            ohp.f_parking AS f_parkingdate,
            ohp.f_freeparking AS f_parkingtime,
            o.f_table, 
            t.f_name AS f_tablename, 
            COALESCE(c.f_govnumber, '') AS f_carnumber, 
            o.f_amounttotal,
            o.f_amountcash,
            o.f_amountcard,
            o.f_amountidram,
            -- Подзапрос для сбора айтемов (услуг) в JSON
            (SELECT 
                JSON_ARRAYAGG(
                    JSON_OBJECT(
                        'f_id', b.f_id,
                        'f_part1name', p1.f_name,
                        'f_part2name', p2.f_name,
                        'f_dishname', d.f_name,
                        'f_cookingtime', bp.f_cookingtime,
                        'f_begin', bp.f_begin
                    )
                )
             FROM o_body b
             LEFT JOIN o_body_process bp ON bp.f_id = b.f_id
             LEFT JOIN d_dish d ON d.f_id = b.f_dish
             LEFT JOIN d_part2 p2 ON p2.f_id = d.f_part
             LEFT JOIN d_part1 p1 ON p1.f_id = p2.f_part
             WHERE b.f_header = o.f_id
            ) AS f_items
        FROM o_header o
        LEFT JOIN o_header_progress ohp ON ohp.f_id = o.f_id
        LEFT JOIN h_tables t ON t.f_id = o.f_table
        LEFT JOIN b_car c ON c.f_id = o.f_id
        WHERE $where
        ORDER BY $orderBy";

    $res = queryStr($sql);
    $data = [];

    while ($row = $res->fetch_assoc()) {
        // Декодируем JSON айтемов, который собрала БД
        $row['f_items'] = json_decode($row['f_items'] ?? '[]', true);

        // Приведение типов для чисел (опционально, но полезно для API)
        $row['f_amounttotal'] = (float)$row['f_amounttotal'];
        $row['f_amountcash'] = (float)$row['f_amountcash'];
        $row['f_amountcard'] = (float)$row['f_amountcard'];
        $row['f_amountidram'] = (float)$row['f_amountidram'];

        $data[] = $row;
    }
    return $data;
}

// --- ОСНОВНАЯ ЛОГИКА ---

// Условие для первого списка (@result1)
$where1 = "ohp.f_state BETWEEN 1 AND 5";
$order1 = "ohp.f_state ASC, o.f_dateopen DESC, o.f_timeopen DESC";
$result1 = getProcessListData($where1, $order1);

// Условие для второго списка (@result2)
$where2 = "((o.f_state IN (1, 5)) OR (o.f_state = 2 AND (o.f_amountcash + o.f_amountcard + o.f_amountidram) < o.f_amounttotal)) AND ohp.f_state = 1";
$order2 = "ohp.f_state ASC, o.f_dateopen DESC, o.f_timeopen ASC";
$result2 = getProcessListData($where2, $order2);

// Склеиваем результаты (аналог JSON_MERGE)
$finalData = array_merge($result1, $result2);

// Вывод результата
printResult(1, $finalData);
