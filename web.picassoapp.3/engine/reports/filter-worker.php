<?php
defined('APP') or die('Die, vampire!'); 

function validateFilter(&$filter, $types) {
	foreach ($types as $t) {
		if (!property_exists($filter, $t["field"])) {
			switch ($t["type"]) {
				case "date":
				case "date1":
				case "date2":
				$filter->{$t["field"]} = date("Y-m-d");
				break;
				case "int":
				case "combo":
				$filter->{$t["field"]} = 0;
				break;
				case "string":
				$filter->{$t["field"]} = "";
				break;
				case "keyvalue":
				$filter->{$t["field"]} = 0;
				break;
			}
		}
	}
}