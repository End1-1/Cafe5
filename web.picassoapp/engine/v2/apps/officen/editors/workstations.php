<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 13:33:09
# Last Modified: 2026-01-17 13:33:14

require_once __DIR__ . '/workstation_config_base.php';
require_once __DIR__ . '/workstation_config_common.php';
require_once __DIR__ . '/workstation_config_waiter.php';
require_once __DIR__ . '/workstation_config_shop.php';

class Workstations
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<EOD
        SELECT t.f_id, t.f_type,  d.f_value, t.f_station_account, t.f_name
        from workstations t
        LEFT JOIN l_dictionary d ON d.f_dict_id=t.f_type AND d.f_dict='workstations_type' AND d.f_lang=?
        ORDER BY t.f_type, t.f_name
        EOD;

        return [
            'rows' => $this->db->select($sql, 's', [Translator::$locale])->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => false, 'delete' => true, 'reload' => true],
            'headers' => [Translator::t('Id'), Translator::t('Id of type'), Translator::t('Name of type'), Translator::t('Station account'), Translator::t('Workstation')],
            'hidden_columns' => [1],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Missing id'));
        }

        return $this->configHandlerForId($id)->getItemPayload($id);
    }

    public function save($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Missing id'));
        }

        $rawConfig = $params->config ?? new stdClass();
        $config = json_decode(json_encode($rawConfig), true);
        if (!is_array($config)) {
            dieWithCode(Translator::t('Invalid config'));
        }

        return $this->configHandlerForId($id)->save($id, $config);
    }

    private function configHandlerForId(int $id): WorkstationConfigBase
    {
        $row = $this->db->select(
            'SELECT f_type FROM workstations WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if (!$row) {
            dieWithCode(Translator::t('Workstation not found'));
        }

        return $this->configHandlerForType((int)$row['f_type']);
    }

    private function configHandlerForType(int $type): WorkstationConfigBase
    {
        switch ($type) {
            case 1:
                return new WorkstationConfigWaiter($this->db);
            case 4:
                return new WorkstationConfigShop($this->db);
            case 5:
                return new WorkstationConfigCommon($this->db);
            default:
                dieWithCode(Translator::t('Settings editor is not available for this workstation type'));
        }
    }
}
