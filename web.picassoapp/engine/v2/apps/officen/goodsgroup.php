<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class Goodsgroup extends Auth
{
    private const SAVE_RULES = [
        "f_id" => "integer|nullable",
        "f_parent" => "integer|nullable",
        "f_class" => "integer|nullable",
        "f_color" => "integer|nullable",
        "f_name" => "string|max:128",
        "f_taxdept" => "integer|nullable",
        "f_adgcode" => "nullable|string|max:16",
        "f_order" => "integer|nullable",
        "f_chargevalue" => "nullable|numeric",
        "f_image" => "nullable|string|max:36",
        "f_image_data" => "nullable|string",
        "f_remove_image" => "integer|nullable",
        "f_online_sale" => "integer|nullable",
    ];

    private function rowById(int $id): ?array
    {
        $sql = <<<SQL
        SELECT
            gr.f_id,
            gr.f_parent,
            gr.f_name,
            CAST(gr.f_class AS SIGNED) AS f_class,
            gr.f_taxdept,
            gr.f_adgcode,
            gr.f_chargevalue,
            gr.f_order,
            gr.f_color,
            gr.f_image,
            CAST(COALESCE(JSON_VALUE(gr.f_data, '$.f_online_sale'), '0') AS UNSIGNED) AS f_online_sale,
            p.f_name AS f_parent_name
        FROM c_groups gr
        LEFT JOIN c_groups p ON p.f_id = gr.f_parent
        WHERE gr.f_id = ?
        SQL;

        $row = $this->select($sql, "i", [$id])->fetch_assoc();
        return $row ?: null;
    }

    private function imageBase64(?string $imageId): string
    {
        if (empty($imageId)) {
            return "";
        }
        $img = $this->select("SELECT f_data FROM s_images WHERE f_id = ?", "s", [$imageId])->fetch_assoc();
        return (string)($img["f_data"] ?? "");
    }

    private function saveImage(?string $imageId, ?string $imageData, bool $remove): ?string
    {
        if ($remove) {
            if (!empty($imageId)) {
                $this->delete("s_images", $imageId, "f_id");
            }
            return null;
        }

        if ($imageData === null || $imageData === "") {
            return $imageId ?: null;
        }

        $uuid = !empty($imageId) ? $imageId : uuid_v4();
        $this->delete("s_images", $uuid, "f_id");
        $this->insert("s_images", (object)[
            "f_id" => $uuid,
            "f_data" => $imageData,
        ]);
        return $uuid;
    }

    public function List($params)
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

        $this->result["rows"] = $this->select($sql)->fetch_all(MYSQLI_NUM);
        $this->echoResult();
    }

    public function Get($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("Group id is required");
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode("Goods group not found", 404);
        }

        $this->result["group"] = $row;
        $this->result["image"] = $this->imageBase64($row["f_image"] ?? null);
        $this->echoResult();
    }

    public function Save($params)
    {
        $data = $this->ValidateParams($params, self::SAVE_RULES);

        if (empty($data->f_name)) {
            dieWithCode("Name is required");
        }

        $removeImage = !empty($data->f_remove_image);
        $imageId = $this->saveImage(
            $data->f_image ?? null,
            $data->f_image_data ?? null,
            $removeImage
        );

        $record = (object)[
            "f_parent" => !empty($data->f_parent) ? (int)$data->f_parent : null,
            "f_class" => isset($data->f_class) && $data->f_class !== "" ? (int)$data->f_class : null,
            "f_color" => isset($data->f_color) && (int)$data->f_color >= 0 ? (int)$data->f_color : null,
            "f_name" => trim((string)$data->f_name),
            "f_taxdept" => !empty($data->f_taxdept) ? (int)$data->f_taxdept : null,
            "f_adgcode" => $data->f_adgcode ?? null,
            "f_order" => isset($data->f_order) && $data->f_order !== "" ? (int)$data->f_order : null,
            "f_chargevalue" => isset($data->f_chargevalue) && $data->f_chargevalue !== ""
                ? (float)$data->f_chargevalue
                : null,
            "f_image" => $imageId,
        ];

        $id = (int)($data->f_id ?? 0);
        if ($id <= 0) {
            $id = (int)$this->insert("c_groups", $record);
        } else {
            $this->update("c_groups", $record, $id);
        }

        $onlineSale = !empty($data->f_online_sale) ? 1 : 0;
        $this->updateJsonField("c_groups", $id, "f_data", "f_online_sale", $onlineSale);

        $row = $this->rowById($id);
        $this->result["group"] = $row;
        $this->result["image"] = $this->imageBase64($row["f_image"] ?? null);
        $this->result["f_id"] = $id;
        $this->echoResult();
    }
}
