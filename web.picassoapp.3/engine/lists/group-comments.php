<?php
require_once(__DIR__ . "/lists.php"); 

$comments = stmtall("select * from d_dish_comment")->fetch_all(MYSQLI_ASSOC);

printResult(1, $comments);