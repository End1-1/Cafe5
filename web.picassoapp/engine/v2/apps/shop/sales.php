<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Sales extends Auth
{
    private const ORDER_STATE_CLOSE = 2;

    public function GetOrders($params)
    {
        $hall = (int)($params->hall ?? 0);
        $date1 = (string)($params->date1 ?? "");
        $date2 = (string)($params->date2 ?? "");
        $state = self::ORDER_STATE_CLOSE;

        $sql = <<<SQL
            SELECT
                oh.f_id,
                oh.f_saletype,
                u.f_login,
                os.f_name AS f_saletype_name,
                oh.f_prefix,
                CONCAT(oh.f_prefix, oh.f_hallid) AS f_number,
                COALESCE(JSON_VALUE(oh.f_data, '$.f_fiscal.rseq'), '') AS f_fiscal,
                oh.f_datecash,
                COALESCE(
                    NULLIF(CAST(oh.f_timeclose AS CHAR), ''),
                    JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_time_close')),
                    ''
                ) AS f_timeclose,
                oh.f_amounttotal,
                CONCAT(COALESCE(c.f_taxname, ''), ' ', COALESCE(c.f_contact, '')) AS f_client,
                CONCAT_WS(' ', dm.f_last, dm.f_first) AS f_deliverman,
                COALESCE(NULLIF(oh.f_comment, ''), JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_comment')), '') AS f_comment
            FROM o_header oh
            LEFT JOIN o_header_options oo ON oo.f_id = oh.f_id
            LEFT JOIN c_partners c ON c.f_id = oh.f_partner
            LEFT JOIN o_sale_type os ON os.f_id = oh.f_saletype
            LEFT JOIN s_user u ON u.f_id = oh.f_staff
            LEFT JOIN s_user dm ON dm.f_id = oo.f_deliveryman
            WHERE oh.f_datecash BETWEEN ? AND ?
              AND oh.f_state = {$state}
              AND oh.f_hall = ?
            ORDER BY oh.f_datecash DESC,
                     COALESCE(
                         NULLIF(CAST(oh.f_timeclose AS CHAR), ''),
                         JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_time_close')),
                         ''
                     ) DESC
        SQL;

        $rows = [];
        $total = 0.0;
        $result = $this->select($sql, "ssi", [$date1, $date2, $hall]);
        while ($row = $result->fetch_assoc()) {
            $amount = (float)$row["f_amounttotal"];
            $total += $amount;
            $rows[] = [
                "f_id" => $row["f_id"],
                "f_saletype" => (int)$row["f_saletype"],
                "f_login" => $row["f_login"],
                "f_saletype_name" => $row["f_saletype_name"],
                "f_prefix" => $row["f_prefix"],
                "f_number" => $row["f_number"],
                "f_fiscal" => $row["f_fiscal"],
                "f_datecash" => $row["f_datecash"],
                "f_timeclose" => $row["f_timeclose"],
                "f_amounttotal" => $amount,
                "f_client" => trim((string)$row["f_client"]),
                "f_deliverman" => $row["f_deliverman"],
                "f_comment" => $row["f_comment"],
            ];
        }

        $this->result["rows"] = $rows;
        $this->result["total"] = $total;
        $this->echoResult();
    }

    public function GetItems($params)
    {
        $hall = (int)($params->hall ?? 0);
        $date1 = (string)($params->date1 ?? "");
        $date2 = (string)($params->date2 ?? "");
        $state = self::ORDER_STATE_CLOSE;

        $sql = <<<SQL
            SELECT
                oh.f_id,
                oh.f_saletype,
                u.f_login,
                os.f_name AS f_saletype_name,
                oh.f_prefix,
                CONCAT(oh.f_prefix, oh.f_hallid) AS f_number,
                COALESCE(JSON_VALUE(oh.f_data, '$.f_fiscal.rseq'), '') AS f_fiscal,
                oh.f_datecash,
                COALESCE(
                    NULLIF(CAST(oh.f_timeclose AS CHAR), ''),
                    JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_time_close')),
                    ''
                ) AS f_timeclose,
                g.f_scancode,
                g.f_name AS f_goodsname,
                og.f_qty,
                og.f_price,
                og.f_total,
                COALESCE(NULLIF(oh.f_comment, ''), JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_comment')), '') AS f_comment
            FROM o_goods og
            INNER JOIN o_header oh ON oh.f_id = og.f_header
            INNER JOIN c_goods g ON g.f_id = og.f_goods
            LEFT JOIN o_sale_type os ON os.f_id = oh.f_saletype
            LEFT JOIN s_user u ON u.f_id = oh.f_staff
            WHERE oh.f_datecash BETWEEN ? AND ?
              AND oh.f_state = {$state}
              AND oh.f_hall = ?
            ORDER BY oh.f_datecash DESC,
                     COALESCE(
                         NULLIF(CAST(oh.f_timeclose AS CHAR), ''),
                         JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_time_close')),
                         ''
                     ) DESC,
                     og.f_row
        SQL;

        $rows = [];
        $total = 0.0;
        $totalQty = 0.0;
        $result = $this->select($sql, "ssi", [$date1, $date2, $hall]);
        while ($row = $result->fetch_assoc()) {
            $qty = (float)$row["f_qty"];
            $amount = (float)$row["f_total"];
            $total += $amount;
            $totalQty += $qty;
            $rows[] = [
                "f_id" => $row["f_id"],
                "f_saletype" => (int)$row["f_saletype"],
                "f_login" => $row["f_login"],
                "f_saletype_name" => $row["f_saletype_name"],
                "f_prefix" => $row["f_prefix"],
                "f_number" => $row["f_number"],
                "f_fiscal" => $row["f_fiscal"],
                "f_datecash" => $row["f_datecash"],
                "f_timeclose" => $row["f_timeclose"],
                "f_scancode" => $row["f_scancode"],
                "f_goodsname" => $row["f_goodsname"],
                "f_qty" => $qty,
                "f_price" => (float)$row["f_price"],
                "f_total" => $amount,
                "f_comment" => $row["f_comment"],
            ];
        }

        $this->result["rows"] = $rows;
        $this->result["total"] = $total;
        $this->result["total_qty"] = $totalQty;
        $this->echoResult();
    }

    /** Aggregated closed sales by o_header.f_staff (sales assistant). */
    public function GetByStaff($params)
    {
        $hall = (int)($params->hall ?? 0);
        $date1 = (string)($params->date1 ?? "");
        $date2 = (string)($params->date2 ?? "");
        $state = self::ORDER_STATE_CLOSE;

        $sql = <<<SQL
            SELECT
                oh.f_staff,
                TRIM(CONCAT(COALESCE(u.f_last, ''), ' ', COALESCE(u.f_first, ''))) AS f_staff_name,
                COALESCE(u.f_login, '') AS f_login,
                COUNT(oh.f_id) AS f_count,
                SUM(oh.f_amounttotal) AS f_amounttotal,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)), 0)) AS f_amount_cash,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_card')) AS DECIMAL(14,2)), 0)) AS f_amount_card,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_idram')) AS DECIMAL(14,2)), 0)) AS f_amount_idram,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_telcell')) AS DECIMAL(14,2)), 0)) AS f_amount_telcell,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_bank')) AS DECIMAL(14,2)), 0)) AS f_amount_bank,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_debt')) AS DECIMAL(14,2)), 0)) AS f_amount_debt,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_prepaid')) AS DECIMAL(14,2)), 0)) AS f_amount_prepaid
            FROM o_header oh
            LEFT JOIN s_user u ON u.f_id = oh.f_staff
            WHERE oh.f_datecash BETWEEN ? AND ?
              AND oh.f_state = {$state}
              AND oh.f_hall = ?
            GROUP BY oh.f_staff, u.f_last, u.f_first, u.f_login
            ORDER BY f_amounttotal DESC, f_staff_name ASC
        SQL;

        $rows = [];
        $total = 0.0;
        $totalCount = 0;
        $result = $this->select($sql, "ssi", [$date1, $date2, $hall]);
        while ($row = $result->fetch_assoc()) {
            $amount = (float)$row["f_amounttotal"];
            $count = (int)$row["f_count"];
            $total += $amount;
            $totalCount += $count;
            $name = trim((string)$row["f_staff_name"]);
            if ($name === "") {
                $name = trim((string)$row["f_login"]);
            }
            if ($name === "" && (int)$row["f_staff"] <= 0) {
                $name = "-";
            }
            $rows[] = [
                "f_staff" => (int)$row["f_staff"],
                "f_staff_name" => $name,
                "f_login" => $row["f_login"],
                "f_count" => $count,
                "f_amounttotal" => $amount,
                "f_amount_cash" => (float)$row["f_amount_cash"],
                "f_amount_card" => (float)$row["f_amount_card"],
                "f_amount_idram" => (float)$row["f_amount_idram"],
                "f_amount_telcell" => (float)$row["f_amount_telcell"],
                "f_amount_bank" => (float)$row["f_amount_bank"],
                "f_amount_debt" => (float)$row["f_amount_debt"],
                "f_amount_prepaid" => (float)$row["f_amount_prepaid"],
            ];
        }

        $this->result["rows"] = $rows;
        $this->result["total"] = $total;
        $this->result["total_count"] = $totalCount;
        $this->echoResult();
    }

    /** Day revenue totals by payment method for closed shop orders. */
    public function GetDailyByPayment($params)
    {
        $hall = (int)($params->hall ?? 0);
        $date1 = (string)($params->date1 ?? "");
        $date2 = (string)($params->date2 ?? "");
        $state = self::ORDER_STATE_CLOSE;

        $sql = <<<SQL
            SELECT
                COUNT(oh.f_id) AS f_count_id,
                SUM(oh.f_amounttotal) AS f_amount_total,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)), 0)) AS f_amount_cash,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_card')) AS DECIMAL(14,2)), 0)) AS f_amount_card,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_idram')) AS DECIMAL(14,2)), 0)) AS f_amount_idram,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_telcell')) AS DECIMAL(14,2)), 0)) AS f_amount_telcell,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_bank')) AS DECIMAL(14,2)), 0)) AS f_amount_bank,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_debt')) AS DECIMAL(14,2)), 0)) AS f_amount_debt,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_prepaid')) AS DECIMAL(14,2)), 0)) AS f_amount_prepaid,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_complimentary')) AS DECIMAL(14,2)), 0)) AS f_amount_complimentary,
                SUM(COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_other')) AS DECIMAL(14,2)), 0)) AS f_amount_other
            FROM o_header oh
            WHERE oh.f_datecash BETWEEN ? AND ?
              AND oh.f_state = {$state}
              AND oh.f_hall = ?
        SQL;

        $row = $this->select($sql, "ssi", [$date1, $date2, $hall])->fetch_assoc();
        if (!$row) {
            $row = [];
        }

        $this->result["f_count_id"] = (int)($row["f_count_id"] ?? 0);
        $this->result["f_amount_total"] = (float)($row["f_amount_total"] ?? 0);
        $this->result["f_amount_cash"] = (float)($row["f_amount_cash"] ?? 0);
        $this->result["f_amount_card"] = (float)($row["f_amount_card"] ?? 0);
        $this->result["f_amount_idram"] = (float)($row["f_amount_idram"] ?? 0);
        $this->result["f_amount_telcell"] = (float)($row["f_amount_telcell"] ?? 0);
        $this->result["f_amount_bank"] = (float)($row["f_amount_bank"] ?? 0);
        $this->result["f_amount_debt"] = (float)($row["f_amount_debt"] ?? 0);
        $this->result["f_amount_prepaid"] = (float)($row["f_amount_prepaid"] ?? 0);
        $this->result["f_amount_complimentary"] = (float)($row["f_amount_complimentary"] ?? 0);
        $this->result["f_amount_other"] = (float)($row["f_amount_other"] ?? 0);
        $this->result["date1"] = $date1;
        $this->result["date2"] = $date2;
        $this->echoResult();
    }
}
