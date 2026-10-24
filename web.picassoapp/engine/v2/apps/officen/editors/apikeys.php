<?php
# © 2026 , Kudryashov Vasili

/**
 * external_api_key rows edited from FrontDesk settings.
 * guest  — booklet QR: menu and guest orders
 * qrneto — QRNeto order tracking
 */
class ApiKeys
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        unset($params);
        $stored = $this->storedByPartner();
        $partners = array_keys($this->catalog());
        foreach (array_keys($stored) as $partner) {
            if (!in_array($partner, $partners, true)) {
                $partners[] = $partner;
            }
        }

        $rows = [];
        foreach ($partners as $partner) {
            $row = $this->present($partner, $stored[$partner] ?? null);
            $rows[] = [
                $row['f_partner'],
                $row['f_title'],
                $row['f_token'],
                (int)$row['f_enabled'] === 1 ? Translator::t('Yes') : Translator::t('No'),
            ];
        }

        return [
            'rows' => $rows,
            'toolbar' => ['new' => false, 'delete' => false, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Name'),
                Translator::t('API key'),
                Translator::t('Enabled'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $partner = $this->partnerFrom($params);
        $stored = $this->storedByPartner();
        if (!isset($this->catalog()[$partner]) && !isset($stored[$partner])) {
            dieWithCode(Translator::t('Unknown API key'), 404);
        }

        return [
            'title' => Translator::t('API key'),
            'row' => $this->present($partner, $stored[$partner] ?? null),
        ];
    }

    public function save($params)
    {
        $partner = $this->partnerFrom($params);
        $stored = $this->storedByPartner();
        $existing = $stored[$partner] ?? null;
        if (!isset($this->catalog()[$partner]) && $existing === null) {
            dieWithCode(Translator::t('Unknown API key'), 404);
        }

        $token = trim((string)($params->f_token ?? ''));
        $enabled = !empty($params->f_enabled) ? 1 : 0;
        if ($token === '') {
            if ($existing === null || trim((string)($existing['f_token_hash'] ?? '')) === '') {
                dieWithCode(Translator::t('Key is required'));
            }
            $this->db->update('external_api_key', [
                'f_enabled' => $enabled,
            ], (int)$existing['f_id']);
        } else {
            if (strlen($token) > 64) {
                dieWithCode(Translator::t('Key is too long'));
            }
            $fields = [
                'f_partner' => $partner,
                'f_token_hash' => hash('sha256', $token),
                'f_token' => $token,
                'f_enabled' => $enabled,
            ];
            if ($existing) {
                $this->db->update('external_api_key', $fields, (int)$existing['f_id']);
            } else {
                $this->db->insert('external_api_key', $fields);
            }
        }

        $stored = $this->storedByPartner();
        return [
            'f_partner' => $partner,
            'row' => $this->present($partner, $stored[$partner] ?? null),
        ];
    }

    private function catalog(): array
    {
        return [
            'guest' => [
                'title' => Translator::t('Table booklet'),
                'hint' => Translator::t('One key per table. Issue via POST /ararix/menu/issue-key with table_id (staff session). Do not use a shared restaurant key.'),
            ],
            'qrneto' => [
                'title' => Translator::t('Order tracking'),
                'hint' => Translator::t('Key for QRNeto order tracking. A new value replaces the previous key.'),
            ],
        ];
    }

    private function partnerFrom($params): string
    {
        $partner = strtolower(trim((string)($params->partner ?? $params->f_partner ?? $params->id ?? '')));
        if ($partner === '' || !preg_match('/^[a-z0-9_-]{1,64}$/', $partner)) {
            dieWithCode(Translator::t('Unknown API key'), 400);
        }
        return $partner;
    }

    /** @return array<string, array<string, mixed>> */
    private function storedByPartner(): array
    {
        $rows = $this->db->select(
            'SELECT f_id, f_partner, f_token_hash, f_token, f_enabled FROM external_api_key ORDER BY f_partner'
        )->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($rows as $row) {
            $out[(string)$row['f_partner']] = $row;
        }
        return $out;
    }

    private function present(string $partner, ?array $stored): array
    {
        $meta = $this->catalog()[$partner] ?? [
            'title' => $partner,
            'hint' => '',
        ];
        $token = trim((string)($stored['f_token'] ?? ''));
        $hasHash = trim((string)($stored['f_token_hash'] ?? '')) !== '';
        return [
            'f_id' => (int)($stored['f_id'] ?? 0),
            'f_partner' => $partner,
            'f_title' => $meta['title'],
            'f_hint' => $meta['hint'],
            'f_token' => $token,
            'f_has_key' => $hasHash ? 1 : 0,
            'f_enabled' => $stored === null ? 1 : (int)$stored['f_enabled'],
        ];
    }
}
