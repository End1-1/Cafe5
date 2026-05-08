<?php
define("AUTH",1);
require_once("../../app.php");
require_once("filters.php");

$cols=["X", tr("Code"), tr("Name"), tr("Position")];
$sql = "select '0' as x, u.f_id, concat_ws(' ', u.f_last, u.f_first) as f_name, g.f_name as f_groupname "
    . "from s_user u "
    . "left join c_groups g on g.f_id=u.f_group "
    . "order by 4, 3";

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);