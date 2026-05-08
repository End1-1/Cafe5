<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-21 11:51:39
# Last Modified: 2026-05-03 13:00:00
require_once __DIR__ . "/index.php";

class Hall extends Auth
{
    public function get($params)
    {
        // 1. Получаем список залов
        $sqlHalls = <<<EOD
        SELECT h.f_id, h.f_name, h.f_settings, s.f_config AS f_data
        FROM h_halls h 
        LEFT JOIN sys_json_config s ON s.f_id = h.f_settings
        EOD;
        $halls = $this->select($sqlHalls)->fetch_all(MYSQLI_ASSOC);

        // 2. Получаем список столов с активными заказами
        // Фильтр по номеру заказа — в PHP (регистронезависимо, без LIKE/collation в SQL).
        $sqlTables = <<<EOD
        SELECT 
            t.f_id, 
            t.f_hall, 
            t.f_name, 
            oh.f_staff, 
            oh.f_amounttotal AS f_amount,
            json_value(oh.f_data, '$.f_time_open') AS f_time_open,
            oh.f_prefix,
            concat(u.f_last, ' ', left(u.f_first, 1), '.') AS f_current_staff_name,
            CASE 
                WHEN cast(json_value(oh.f_data, '$.f_precheck') AS INT) > 0 THEN 3
                WHEN length(coalesce(oh.f_id,'')) > 0 THEN 2        
                ELSE 1 
            END AS f_table_state,
            t.f_special_config, 
            json_merge_patch(
                ifnull(oh.f_data, '{}'), 
                ifnull(s.f_config, '{}')
            ) AS f_data
        FROM h_tables t
        LEFT JOIN o_header oh ON oh.f_table = t.f_id AND oh.f_state = 1
        LEFT JOIN s_user u ON u.f_id = oh.f_staff
        LEFT JOIN sys_json_config s ON s.f_id = t.f_special_config
        ORDER BY t.f_id
        EOD;

        $tables = $this->select($sqlTables)->fetch_all(MYSQLI_ASSOC);

        // 3. Фильтруем в PHP, если передан номер заказа (o_header.f_prefix)
        $orderNumber = isset($params->order_number) ? trim((string)$params->order_number) : '';

        if ($orderNumber !== '') {
            $tables = array_values(array_filter($tables, function ($t) use ($orderNumber) {
                $prefix = (string)($t['f_prefix'] ?? '');
                if ($prefix === '') {
                    return false;
                }

                return mb_stripos($prefix, $orderNumber) !== false;
            }));
        }

        // 4. Отдаем результат
        $this->result["halls"] = $halls;
        $this->result["tables"] = $tables;
        $this->echoResult();
    }
}
