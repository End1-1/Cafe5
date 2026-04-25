<?php
# © 2025 , Kudryashov Vasili
# Created: 2026-01-15 09:44:39
# Last Modified: 2026-02-28 10:36:41
require_once __DIR__ . "/index.php";

class Workstation extends Auth
{
    public function GetConfig($params)
    {
        // 1. Проверяем, есть ли уже такая настройка
        $check = $this->select(
            "select f_id from workstations where f_name = ? and f_station_account = ? and f_type = ? limit 1",
            "ssi",
            [$params->workstation, $params->station_account, $params->type]
        )->fetch_assoc();

        // 2. Если не нашли — создаем запись
        if (!$check) {
            $this->select(
                "insert ignore into workstations (f_type, f_station_account, f_name, f_config) values (?, ?, ?, '{}')",
                "iss",
                [$params->type, $params->station_account, $params->workstation],
                true
            );
        }

        // 3. Финальная выборка: берем нашу запись и подмешиваем к ней конфиг от типа 5
        // Используем LEFT JOIN, чтобы гарантированно получить основную запись, даже если типа 5 нет в базе
        $sqlconf = <<<EOD
        select
            w.f_id,
            w.f_type,
            w.f_station_account,
            w.f_name,
            json_detailed(json_merge_patch(
                ifnull(d.f_config, json_object()), 
                w.f_config
            )) as f_config
        from workstations w
        left join workstations d on d.f_type = 5 
        where w.f_name = ? 
          and w.f_station_account = ? 
          and w.f_type = ?
        limit 1
    EOD;

        $fiscal = $this->select("select * from fiscal_machine")->fetch_all(MYSQLI_ASSOC);

        $config = $this->select(
            $sqlconf,
            "ssi",
            [$params->workstation, $params->station_account, $params->type]
        )->fetch_assoc();

        if ($config) {
            $this->result = array_merge($this->result, $config, ["fiscal" => $fiscal]);
        }

        $this->echoResult();
    }
}
