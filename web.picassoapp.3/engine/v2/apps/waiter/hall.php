<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-21 11:51:39
# Last Modified: 2026-01-09 11:10:57
require_once __DIR__ . "/index.php";

class Hall extends Auth {
    public function get($params) {
        $sql =<<<EOD
        select h.f_id, h.f_name, h.f_settings, s.f_config as f_data
        from h_halls h 
        left join sys_json_config s on s.f_id=h.f_settings
        where 1=1
        EOD;
        $halls = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $sql =<<<EOD
        select t.f_id, t.f_hall, t.f_name, oh.f_staff, oh.f_amounttotal as f_amount,
        json_value(oh.f_data, '$.f_time_open') as f_time_open,
        concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_current_staff_name,
        case 
        when cast(json_value(oh.f_data, '$.f_precheck') AS INT)>0 then 3
        when length(coalesce(oh.f_id,''))>0 then 2        
        ELSE 1 
        end as f_table_state,
        t.f_special_config, 
        json_merge_patch(
        ifnull(oh.f_data, '{}'), 
        ifnull(s.f_config, '{}' )) as f_data
        
        from h_tables t
        left join o_header oh on oh.f_table=t.f_id and oh.f_state=1
        left join s_user u on u.f_id=oh.f_staff
        LEFT JOIN sys_json_config s ON s.f_id=t.f_special_config
        order by  t.f_id
        EOD;
        $tables = $this->select($sql)->fetch_all(MYSQLI_ASSOC);   
        $this->result["halls"] = $halls;
        $this->result["tables"] = $tables; 
        $this->echoResult();
    }
}

