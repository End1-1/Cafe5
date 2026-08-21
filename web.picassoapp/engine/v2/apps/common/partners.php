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
        $this->validator = require_once __DIR__ . "/dict-validators.php";
        return parent::__construct();
    }

    public function GetPartnerData($id)
    {
        $sql = $this->validator["select"]["c_partners"]["sql"];
        $sql = str_replace("1=1", "p.f_id=?", $sql);
        return $this->select($sql, "ssi", [Translator::$locale, Translator::$locale, $id])->fetch_assoc();
    }

    public function Get($params)
    {
        $this->result["partner"] = $this->GetPartnerData($params);
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
        $this->result["partner"] = $partner;

        $notify = require __DIR__ . "/../../worker/ws-notify.php";
        $notify->notify("partners", (int)$data->f_id, $isNew);

        $this->echoResult();
    }

    /** Ensure FK target exists (shop may send buyer category=2 / group=3). */
    private function ensurePartnerCategory(int $id): void
    {
        $defaults = [
            1 => "General",
            2 => "Buyer",
            3 => "Supplier",
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
