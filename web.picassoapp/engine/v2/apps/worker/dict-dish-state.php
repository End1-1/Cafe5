<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-18 16:58:17
# Last Modified: 2026-01-18 16:58:20

define("DISH_STATE_NONE", 0);
define("DISH_STATE_NORMAL", 1);
define("DISH_STATE_MISTAKE", 2);
define("DISH_STATE_VOID", 3);
define("DISH_STATE_PREORDER", 4);

return [

    "names" => [
        DISH_STATE_NONE => "Դատարկ",
        DISH_STATE_NORMAL => "Նորմալ",
        DISH_STATE_MISTAKE => "Սխալ",
        DISH_STATE_VOID => "Չեղարկում",
        DISH_STATE_PREORDER => "Նախապատվեր"
    ]
];
