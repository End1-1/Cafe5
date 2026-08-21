<?php
# © 2026 , Kudryashov Vasili

class Tables
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function GetItem($params)
    {
        return [
            "title" => Translator::t("Tables"),
        ];
    }

    public function delete($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);

        if ($id <= 0) {
            dieWithCode(Translator::t("Table id is required"));
        }

        if ($id === 1) {
            dieWithCode(Translator::t("Cannot delete default table"));
        }

        $row = $this->db->select(
            "SELECT f_id, f_name FROM h_tables WHERE f_id = ?",
            "i",
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            dieWithCode(Translator::t("Table not found"), 404);
        }

        $fallback = $this->db->select(
            "SELECT f_id FROM h_tables WHERE f_id = 1"
        )->fetch_assoc();

        if ($fallback === null) {
            dieWithCode(Translator::t("Default table (1) not found"));
        }

        $openCount = (int)$this->db->select(
            "SELECT COUNT(f_id) AS cnt FROM o_header WHERE f_table = ? AND f_state = 1",
            "i",
            [$id]
        )->fetch_assoc()["cnt"];

        if ($openCount > 0) {
            dieWithCode(Translator::t("Table has open orders. Close or transfer them first."));
        }

        $this->db->beginTransaction();

        $this->db->select(
            "UPDATE o_header SET f_table = 1 WHERE f_table = ?",
            "i",
            [$id],
            true
        );

        $this->db->delete("h_tables", $id);
        $this->db->commit();

        return [
            "message" => Translator::t("Deleted"),
        ];
    }
}
