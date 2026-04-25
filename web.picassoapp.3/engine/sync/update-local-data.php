<?php
require_once (__DIR__ . "/sync.php");

foreach ($params->data as $d) {
    switch ($d->f_op) {
        case 1:
            $data = (array)$d->row;
			if (!empty($data)) {
                try {
				    sinsertnoerror("$params->db.$d->f_table", $data);
                } catch(Exception $e) {
                    
                }
			}
            break;
        case 2:
            $data = (array)$d->row;
			if (!empty($data)) {
				supdate("$params->db.$d->f_table", $data, $d->f_recid);
			}
            break;
        case 3:
            stmtall("delete from $params->db.$d->f_table where f_id='{$d->f_recid}'");
            break;
    }
    $params->id = max($params->id, $d->f_id);
}

stmtall("update $params->db.s_db set f_syncin=$params->id");

printResult(1, ["id" => $params->id]);