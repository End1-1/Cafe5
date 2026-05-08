<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-04 14:03:43
# Last Modified: 2025-12-12 15:00:00

require_once "report.php";

class MGoalProduct extends Report
{

    public function widget()
    {
        return [
            "title" => Translator::t("Product"),
            "icon" => "template.png",
            "version" => 1
        ];
    }

    public function  sumColumns()
    {
        return [["8" => 0], ["7" => 0]];
    }

    public function hiddenCols()
    {
        return [];
    }

    public function handler()
    {
        return [
            'cac01eeb-b965-11f0-a072-8a884be02f31',
            'cac01eeb-b965-11f0-a072-8a884be02f31'
        ];
    }

    public function columns()
    {
        return [
            Translator::t("ID"),
            Translator::t("Date"),
            Translator::t("Status"),
            Translator::t("Name"),
            Translator::t("Width"),
            Translator::t("Height"),
            Translator::t("34"),
            Translator::t("36"),
            Translator::t("38"),
            Translator::t("40"),
            Translator::t("42"),
            Translator::t("44"),
            Translator::t("46"),
        ];
    }

    public function filter()
    {
        return [
            ["type" => "date", "title" => Translator::t("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => Translator::t("Date of end"), "field" => "date2"]
        ];
    }

    public function rows()
    {
        if (empty($this->params->date1)) {
            $this->params->date1 = date('Y-m-d');
            $this->params->date2 = date('Y-m-d');
        }
        $where =   " where  g.f_date between '{$this->params->date1}' and '{$this->params->date2}' ";
        if (!empty($this->params->status)) {
            $where .= " and g.f_status={$this->params->status}";
        }
        if (!empty($this->params->product)) {
            $where .= " and g.f_product={$this->params->product}";
        }
        if (!empty($this->params->showincomplete)) {
            $where .= " or (g.f_status in (1,2,3,4))";
        }

        $sql = <<<EOD
        SELECT g.f_id, g.f_date, gn.f_name, mf.f_name,
        g.f_width, g.f_height, 
        g.f_34, g.f_36, g.f_38, g.f_40, g.f_42, g.f_44, g.f_46
        FROM m_goal_product g
        left join mf_actions_group mf on mf.f_id=g.f_product
        LEFT JOIN m_goal_product_status gn ON gn.f_id=g.f_status
        $where 
        EOD;
        $this->debugi = $sql;
        return $this->select($sql)->fetch_all();
    }

    public function StatusList($params)
    {
        $data = $this->select("select * from m_goal_product_status")->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->echoResult();
    }

    public function ReasonList($params)
    {
        $data = $this->select("select * from m_goal_product_reason where f_id>1")->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = $data;
        $this->echoResult();
    }

    public function Put($params)
    {

        $image = $_FILES['image'] ?? null;
        $this->result["image"] = $image ? "yes" : "null";
        if ($image && $image['tmp_name']) {
            $dir = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/production/";
            $this->result["dir"] = $dir;
            if (!is_dir($dir)) {
                $this->result["mkdir"] = mkdir($dir, 0777, true);
            }
            $ext = pathinfo($image['name'], PATHINFO_EXTENSION);
            $filename = "prod_" . time() . "_" . rand(1000, 9999) . "." . $ext;
            $targetPath = $dir . $filename;
            move_uploaded_file($image['tmp_name'], $targetPath);
            $params->f_image_url = "/engine/media/production/" . $filename;
        }

        $v = clone $params;
        unset($v->_files);
        unset($v->f_name);
        unset($v->materials);
        unset($v->other);
        unset($v->_imageFile);
        if ($params->f_id > 0) {
            $this->update("m_goal_product", $v, $params->f_id);
        } else {
            $this->insert("m_goal_product", $params);
        }
        $this->delete("m_goal_product_material", $params->f_id, "f_product");
        $materials = $params->materials;
        foreach ($materials as $m) {
            unset($m->f_name);
            unset($m->f_materialname);
            unset($m->f_reasonname);
            $m->f_product = $params->f_id;
            $m->f_reason = 1;
            $this->insert("m_goal_product_material", $m);
        }
        $others = $params->other;
        foreach ($others as $m) {
            unset($m->f_materialname);
            unset($m->f_reasonname);
            $m->f_product = $params->f_id;
            $this->insert("m_goal_product_material", $m);
        }
        $this->echoResult();
    }

    public function Edit($params)
    {
        $scheme = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') ? "https" : "http";
        $host = $_SERVER['HTTP_HOST'];
        $baseUrl = "$scheme://$host";

        $sql = <<<EOD
        SELECT g.f_id, g.f_date, gn.f_name, mf.f_name,
        g.f_width, g.f_height, 
        g.f_34, g.f_36, g.f_38, g.f_40, g.f_42, g.f_44, g.f_46,
        concat('$baseUrl', f_image_url) as f_image_url
        FROM m_goal_product g
        left join mf_actions_group mf on mf.f_id=g.f_product
        LEFT JOIN m_goal_product_status gn ON gn.f_id=g.f_status
        where g.f_id=?
        EOD;
        $data = $this->select($sql, "i", [$params->id])->fetch_assoc();
        if (empty($data)) {
            dieWithCode(Translator::t("No data"));
        }
        $this->result["data"] = $data;
        $sql = <<<EOD
        select mm.f_id, mm.f_material, ma.f_name as f_materialname, mm.f_code, mm.f_color, mm.f_qty1, mm.f_qty2, mm.f_totalqty, mm.f_qtyperone, mm.f_colorqty,
        mm.f_row
        from m_goal_product_material mm
        left join c_goods ma on ma.f_id=mm.f_material 
        where mm.f_product=? and f_reason=1
        order by mm.f_row
        EOD;
        $this->result["materials"] = $this->select($sql, "i", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $sql = <<<EOD
        select mm.f_id,mm.f_reason, mn.f_name as f_reasonname, mm.f_material, ma.f_name as f_materialname, mm.f_code, mm.f_color, mm.f_qty1, mm.f_qty2, mm.f_totalqty, mm.f_qtyperone, mm.f_colorqty,
        mm.f_row
        from m_goal_product_material mm
        left join c_goods ma on ma.f_id=mm.f_material 
        left join m_goal_product_reason mn on mn.f_id=mm.f_reason
        where mm.f_product=? and f_reason<>1
        order by mm.f_row
        EOD;
        $this->result["other"] = $this->select($sql, "i", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
