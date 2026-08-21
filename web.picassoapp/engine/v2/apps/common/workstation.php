<?php
# © 2025 , Kudryashov Vasili
# Created: 2026-01-15 09:44:39
# Last Modified: 2026-02-28 10:36:41
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../officen/editors/workstation_config_base.php";
require_once __DIR__ . "/../officen/editors/workstation_config_common.php";
require_once __DIR__ . "/../officen/editors/workstation_config_waiter.php";
require_once __DIR__ . "/../officen/editors/workstation_config_shop.php";

class Workstation extends Auth
{
    public function GetConfig($params)
    {
        $type = (int)$params->type;
        $handler = $this->configHandlerForType($type);
        $initialConfig = $handler ? $handler->defaultConfigJson() : '{}';

        // 1. Проверяем, есть ли уже такая настройка
        $check = $this->select(
            "select f_id from workstations where f_name = ? and f_station_account = ? and f_type = ? limit 1",
            "ssi",
            [$params->workstation, $params->station_account, $type]
        )->fetch_assoc();

        // 2. Если не нашли — создаем запись со значениями по умолчанию для типа
        if (!$check) {
            $this->select(
                "insert ignore into workstations (f_type, f_station_account, f_name, f_config) values (?, ?, ?, ?)",
                "isss",
                [$type, $params->station_account, $params->workstation, $initialConfig],
                true
            );
        }

        // 3. Финальная выборка: shop/waiter + common (type 5), одна запись common
        $sqlconf = <<<EOD
        select
            w.f_id,
            w.f_type,
            w.f_station_account,
            w.f_name,
            json_detailed(json_merge_patch(
                ifnull(d.f_config, json_object()),
                ifnull(w.f_config, json_object())
            )) as f_config
        from workstations w
        left join (
            select f_config from workstations where f_type = 5 order by f_id asc limit 1
        ) d on 1 = 1
        where w.f_name = ?
          and w.f_station_account = ?
          and w.f_type = ?
        limit 1
    EOD;

        $fiscal = $this->select("select * from fiscal_machine")->fetch_all(MYSQLI_ASSOC);

        $config = $this->select(
            $sqlconf,
            "ssi",
            [$params->workstation, $params->station_account, $type]
        )->fetch_assoc();

        if ($config) {
            $cfg = json_decode($config['f_config'] ?? '{}', true);
            if (!is_array($cfg)) {
                $cfg = [];
            }
            // Shop/waiter configs are merged with common (type 5). Apply defaults per handler
            // separately, then merge — otherwise shop/waiter withDefaults() strips common keys
            // (print_server, scale patterns, …).
            if ($handler) {
                $commonHandler = new WorkstationConfigCommon($this);
                $commonCfg = $commonHandler->withDefaults($cfg);
                if ($type === 5) {
                    $cfg = $commonCfg;
                } else {
                    $typeCfg = $handler->withDefaults($cfg);
                    $cfg = array_merge($commonCfg, $typeCfg);
                }
            }
            $cfg = $this->enrichConfigNames($cfg);
            $config['f_config'] = json_encode($cfg, JSON_UNESCAPED_UNICODE);
            $this->result = array_merge($this->result, $config, ["fiscal" => $fiscal]);
        }

        $this->echoResult();
    }

    private function configHandlerForType(int $type): ?WorkstationConfigBase
    {
        return match ($type) {
            1 => new WorkstationConfigWaiter($this),
            4 => new WorkstationConfigShop($this),
            5 => new WorkstationConfigCommon($this),
            default => null,
        };
    }

    /** @param array<string,mixed> $cfg */
    private function enrichConfigNames(array $cfg): array
    {
        $storeId = (int)($cfg['f_default_store_id'] ?? 0);
        if ($storeId > 0) {
            $row = $this->select(
                "select f_id, f_name from c_storages where f_id=?",
                "i",
                [$storeId]
            )->fetch_assoc();
            $cfg['f_default_store_name'] = $row['f_name'] ?? '';
        } else {
            $cfg['f_default_store_name'] = '';
        }

        $hallId = (int)($cfg['f_default_hall_id'] ?? 0);
        if ($hallId > 0) {
            $row = $this->select(
                "select f_id, f_name from h_halls where f_id=?",
                "i",
                [$hallId]
            )->fetch_assoc();
            $cfg['f_default_hall_name'] = $row['f_name'] ?? '';
        } else {
            $cfg['f_default_hall_name'] = '';
        }

        return $cfg;
    }
}
