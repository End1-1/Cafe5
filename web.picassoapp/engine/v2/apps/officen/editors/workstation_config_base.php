<?php
# © 2026 , Kudryashov Vasili

abstract class WorkstationConfigBase
{
    protected Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    abstract public function type(): int;

    /** @return list<string> */
    abstract protected function allowedKeys(): array;

    /** @return array<string,mixed> */
    abstract protected function defaultConfig(): array;

    public function load(int $id): array
    {
        $row = $this->fetchRow($id);
        if ((int)$row['f_type'] !== $this->type()) {
            dieWithCode(Translator::t('Workstation type mismatch'));
        }

        $stored = json_decode($row['f_config'] ?? '{}', true);
        if (!is_array($stored)) {
            $stored = [];
        }

        return [
            'row' => $row,
            'config' => $this->mergeDefaults($stored),
        ];
    }

    /** @param array<string,mixed> $config */
    public function save(int $id, array $config): array
    {
        $row = $this->fetchRow($id);
        if ((int)$row['f_type'] !== $this->type()) {
            dieWithCode(Translator::t('Workstation type mismatch'));
        }

        $normalized = $this->normalizeConfig($config);
        $this->db->update(
            'workstations',
            ['f_config' => json_encode($normalized, JSON_UNESCAPED_UNICODE)],
            $id
        );

        return $this->getItemPayload($id);
    }

    public function getItemPayload(int $id): array
    {
        $loaded = $this->load($id);
        $row = $loaded['row'];

        return [
            'title' => Translator::t('Workstation') . ': ' . ($row['f_name'] ?? ''),
            'f_id' => (int)$row['f_id'],
            'f_type' => (int)$row['f_type'],
            'f_name' => $row['f_name'] ?? '',
            'f_station_account' => $row['f_station_account'] ?? '',
            'config' => $loaded['config'],
            'config_kind' => $this->configKind(),
        ];
    }

    protected function configKind(): string
    {
        return match ($this->type()) {
            1 => 'waiter',
            4 => 'shop',
            5 => 'common',
            default => 'unknown',
        };
    }

    /** @return array<string,mixed> */
    protected function mergeDefaults(array $stored): array
    {
        $merged = array_merge($this->defaultConfig(), $stored);
        return $this->normalizeConfig($merged);
    }

    /** @param array<string,mixed> $config */
    protected function normalizeConfig(array $config): array
    {
        $allowed = array_flip($this->allowedKeys());
        $out = [];

        foreach ($config as $key => $value) {
            if (!isset($allowed[$key])) {
                continue;
            }
            $out[$key] = $this->normalizeValue($key, $value);
        }

        foreach ($this->defaultConfig() as $key => $defaultValue) {
            if (!array_key_exists($key, $out)) {
                $out[$key] = $defaultValue;
            }
        }

        return $out;
    }

    protected function normalizeValue(string $key, mixed $value): mixed
    {
        if ($key === 'setup_buttons') {
            return $this->normalizeSetupButtons($value);
        }

        if (is_bool($value)) {
            return $value;
        }

        if (in_array($key, [

            'f_cashbox_id',
            'f_default_hall_id',
            'f_fiscal_machine_id',
            'f_default_store_id',
            'f_quick_debt_partner_id',
            'dlgsearchmenu_hsection_size',
            'dlgsearchmenu_vsection_size',
        ], true)) {
            return (int)$value;
        }

        if (in_array($key, [
            'cost_depend_on_service_and_discount',
            'do_not_print_customer_on_receipt',
            'receipt_no_table',
            'receipt_no_service_hint',
            'receipt_no_discount_hint',
            'input_cashbox_amount_before_close',
            'bistro',
        ], true)) {
            return (bool)$value;
        }

        return is_scalar($value) ? (string)$value : $value;
    }

    /** @return array<string,bool> */
    protected function normalizeSetupButtons(mixed $value): array
    {
        $defaults = WorkstationConfigWaiter::defaultSetupButtons();
        if (!is_array($value)) {
            return $defaults;
        }

        $out = $defaults;
        foreach ($value as $key => $flag) {
            if (!is_string($key)) {
                continue;
            }
            if (array_key_exists($key, $out)
                || str_starts_with($key, 'dlg')
                || str_starts_with($key, 'payment_button_')) {
                $out[$key] = (bool)$flag;
            }
        }

        return $out;
    }

    /** @return array<string,mixed> */
    protected function fetchRow(int $id): array
    {
        $row = $this->db->select(
            'SELECT f_id, f_type, f_station_account, f_name, f_config FROM workstations WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if (!$row) {
            dieWithCode(Translator::t('Workstation not found'));
        }

        return $row;
    }
}
