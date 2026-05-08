<?php
require_once(__DIR__ . "/carwash.php");

queryStr("select sf_unknown_tr('{\"f_en\":\"$params->f_en\"}')");

printResult(1, "");