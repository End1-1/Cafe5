<?php
# © 2026 , Kudryashov Vasili

class GoodsGroup
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT
            gr.f_id,
            COALESCE(p.f_name, '') AS f_parent,
            gr.f_name,
            CAST(gr.f_class AS SIGNED) AS f_class,
            gr.f_taxdept,
            gr.f_adgcode,
            gr.f_chargevalue,
            COALESCE(g.qnt, 0) AS qnt,
            gr.f_order
        FROM c_groups gr
        LEFT JOIN c_groups p ON p.f_id = gr.f_parent
        LEFT JOIN (
            SELECT f_group, COUNT(f_id) AS qnt
            FROM c_goods
            GROUP BY f_group
        ) g ON g.f_group = gr.f_id
        ORDER BY COALESCE(gr.f_order, 9999), gr.f_name
        SQL;

        return [
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["new" => true, "delete" => true, "reload" => true],
            "headers" => [
                Translator::t("Code"),
                Translator::t("Parent"),
                Translator::t("Name"),
                Translator::t("Class"),
                Translator::t("Tax dept"),
                Translator::t("ADG code"),
                Translator::t("Charge value"),
                Translator::t("Qty"),
                Translator::t("Queue"),
            ],
        ];
    }

    public function GetItem($params)
    {
        return [
            "title" => Translator::t("Goods group"),
        ];
    }

    public function delete($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        $moveTo = (int)($params->move_to ?? $params->f_move_to ?? 0);

        if ($id <= 0) {
            dieWithCode(Translator::t("Group id is required"));
        }

        $row = $this->db->select(
            "SELECT f_id, f_parent, f_image FROM c_groups WHERE f_id = ?",
            "i",
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            dieWithCode(Translator::t("Goods group not found"), 404);
        }

        $this->db->beginTransaction();

        $goodsCount = (int)$this->db->select(
            "SELECT COUNT(f_id) AS cnt FROM c_goods WHERE f_group = ?",
            "i",
            [$id]
        )->fetch_assoc()["cnt"];

        if ($goodsCount > 0) {
            if ($moveTo <= 0) {
                dieWithCode(Translator::t("Group contains goods. Select target group to move them."));
            }
            if ($moveTo === $id) {
                dieWithCode(Translator::t("Target group must differ from deleted group"));
            }

            $target = $this->db->select(
                "SELECT f_id FROM c_groups WHERE f_id = ?",
                "i",
                [$moveTo]
            )->fetch_assoc();

            if ($target === null) {
                dieWithCode(Translator::t("Target group not found"), 404);
            }

            $this->db->select(
                "UPDATE c_goods SET f_group = ? WHERE f_group = ?",
                "ii",
                [$moveTo, $id],
                true
            );
        }

        if (!empty($row["f_parent"])) {
            $this->db->select(
                "UPDATE c_groups SET f_parent = ? WHERE f_parent = ?",
                "ii",
                [(int)$row["f_parent"], $id],
                true
            );
        } else {
            $this->db->select(
                "UPDATE c_groups SET f_parent = NULL WHERE f_parent = ?",
                "i",
                [$id],
                true
            );
        }

        if (!empty($row["f_image"])) {
            $this->db->delete("s_images", $row["f_image"], "f_id");
        }

        $this->db->delete("c_groups", $id);
        $this->db->commit();

        return [
            "message" => Translator::t("Deleted"),
            "goods_moved" => $goodsCount,
        ];
    }
}
