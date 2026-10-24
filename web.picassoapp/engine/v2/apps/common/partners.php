<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-06 17:03:39
# Last Modified: 2026-02-09 11:35:01

require_once __DIR__ . "/index.php";

class Partners extends Auth
{

    private $validator;
    public function __construct()
    {
        // require (not require_once): require_once returns true if file was already loaded.
        $this->validator = require __DIR__ . "/dict-validators.php";
        return parent::__construct();
    }

    public function GetPartnerData($id)
    {
        $partnerId = (int)$id;
        if ($partnerId <= 0 || !is_array($this->validator)) {
            return null;
        }
        $sql = $this->validator["select"]["c_partners"]["sql"] ?? "";
        if ($sql === "") {
            return null;
        }
        $sql = str_replace("1=1", "p.f_id=?", $sql);
        $locale = Translator::$locale ?? "hy";
        return $this->select($sql, "ssi", [$locale, $locale, $partnerId])->fetch_assoc() ?: null;
    }

    public function Get($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        $partner = $this->GetPartnerData($id);
        if (!$partner) {
            dieWithCode("Partner not found");
        }
        $this->result["status"] = 1;
        $this->result["partner"] = $partner;
        $this->echoResult();
    }

    public function GetByTin($params)
    {
        $tin = trim((string)($params->f_taxcode ?? ""));
        if ($tin === "") {
            dieWithCode("f_taxcode is required");
        }
        $row = $this->select(
            "select f_id from c_partners where f_taxcode=? and f_state>0 limit 1",
            "s",
            [$tin]
        )->fetch_assoc();
        if (!$row) {
            dieWithCode("Partner not found");
        }
        $this->result["partner"] = $this->GetPartnerData((int)$row["f_id"]);
        $this->echoResult();
    }

    public function Save($params)
    {

        if (!is_array($this->validator) || !isset($this->validator["save"]["c_partners"])) {
            dieWithCode("Partner validators not loaded");
        }
        $data = $this->ValidateParams($params, $this->validator["save"]["c_partners"]);
        $this->ensurePartnerCategory((int)($data->f_category ?? 0));
        $this->ensurePartnerGroup((int)($data->f_group ?? 0));
        $isNew = empty($data->f_id);
        if ($isNew) {
            $data->f_id = $this->insert("c_partners", $data);
        } else {
            $this->update("c_partners", $data, $data->f_id);
        }
        $partner = $this->GetPartnerData($data->f_id);
        if (!$partner) {
            dieWithCode("Partner saved but could not be reloaded");
        }
        $this->result["status"] = 1;
        $this->result["partner"] = $partner;

        try {
            $notify = require __DIR__ . "/../../worker/ws-notify.php";
            if (is_object($notify) && method_exists($notify, "notify")) {
                $notify->notify("partners", (int)$data->f_id, $isNew);
            }
        } catch (Throwable $e) {
            // WS notify must not fail partner save
        }

        $this->echoResult();
    }

    /** Ensure FK target exists. Categories match c_partners_category: 1 main, 2 supplier, 3 customer. */
    private function ensurePartnerCategory(int $id): void
    {
        $defaults = [
            1 => "General",
            2 => "Supplier",
            3 => "Customer",
        ];
        $this->ensureLookupRow("c_partners_category", $id, $defaults, "f_category");
    }

    private function ensurePartnerGroup(int $id): void
    {
        $defaults = [
            1 => "Main",
            2 => "Retail",
            3 => "Shop",
        ];
        $this->ensureLookupRow("c_partners_group", $id, $defaults, "f_group");
    }

    private function ensureLookupRow(string $table, int $id, array $defaults, string $field): void
    {
        if ($id <= 0) {
            dieWithCode("$field is required");
        }
        $row = $this->select("select f_id from `$table` where f_id=?", "i", [$id])->fetch_assoc();
        if ($row) {
            return;
        }
        if (!isset($defaults[$id])) {
            dieWithCode("Unknown $field: $id");
        }
        $this->select(
            "insert ignore into `$table` (f_id, f_name) values (?, ?)",
            "is",
            [$id, $defaults[$id]],
            true
        );
    }
}
