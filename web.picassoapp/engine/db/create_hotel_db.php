<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-07 23:39:57
# Last Modified: 2025-12-07 23:40:00

$sql =<<<EOD
create table hs_room_building (f_id integer primary key auto_increment, f_name tinytext);
CREATE TABLE hs_room_view (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_building int, f_short CHAR(2), f_name TINYTEXT);
CREATE TABLE hs_room_floor (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_building int, f_name TINYTEXT);
CREATE TABLE hs_room_category (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_short CHAR(2), f_name TINYTEXT);
CREATE TABLE hs_rooms (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_building int, f_floor INTEGER, f_category INTEGER, f_view INT, f_number INT);
EOD;