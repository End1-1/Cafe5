<?php

require __DIR__ . "/../app.php";

stmtall("delete from d_translator where f_mode=?", "i", [$params->mode]);
foreach ($params->translator as $t) {
    sinsert("d_translator", $t);
}
printResult(0, "ok");