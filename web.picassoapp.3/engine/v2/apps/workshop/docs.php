<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-20 12:56:20
# Last Modified: 2025-12-20 12:56:24
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class Docs extends Auth
{

    public function CreateComplect($params)
    {
        $product = $this->select("select * from m_goal_product where f_id=?", "i", [$params->product])->fetch_assoc();
        $sizes = [34, 36, 38, 40, 42, 44, 46];
        $sizes_exists = [];
        $size_count = 0;
        foreach ($sizes as $s) {
            if ((int)($product["f_$s"] ?? 0) === 1) {
                $size_count++;
                $sizes_exists[] = $s;
            }
        }
        $product_group = $this->select("select f_goods_group from mf_actions_group where f_id=?", "i", [$product["f_product"]])->fetch_assoc();
        $materials_with_code = $this->select("select * from m_goal_product_material where f_product=? and length(f_code)>0 and f_reason=1 ", "i", [$params->product])->fetch_all(MYSQLI_ASSOC);
        $materials_without_code = $this->select("select * from m_goal_product_material where f_product=? and length(coalesce(f_code, ''))=0 and f_reason=1", "i", [$params->product])->fetch_all(MYSQLI_ASSOC);
        $materials_brak = $this->select("select m.*, rn.f_name as f_reasonname from m_goal_product_material m left join m_goal_product_reason rn on rn.f_id=m.f_reason where f_product=? and f_reason>1", "i", [$params->product])->fetch_all(MYSQLI_ASSOC);
        $totalqty = $this->select("select sum(f_colorqty)  as totalqty from m_goal_product_material where f_product=? and length(f_code)>0 and f_reason=1 ", "i", [$params->product])->fetch_assoc()["totalqty"];

        $complete_codes = [];
        $barcode_done = [];
        foreach ($materials_with_code as $mwc) {
            foreach ($sizes_exists as $size) {
                $barcode = (string) ( $size . $mwc["f_code"] . $mwc["f_color"] . $product_group["f_goods_group"]);
                $complete_codes[] = $barcode;
                $data = new stdClass();
                $data->add = [];
                
                $goods = [[
                    "goods"=> $mwc["f_material"],
                    "qty"=> $mwc["f_qtyperone"] * ($mwc["f_colorqty"] / $size_count),
                    "comment"=>""
                ]];
                foreach ($materials_without_code as $mc) {
                    $goods[] = [
                        "goods"=>$mc["f_material"],
                        "qty"=> $mc["f_colorqty"] / $totalqty,
                        "comment"=>"",
                    ];
                }
                foreach ($materials_brak as $mc) {
                    $goods[] = [
                        "goods"=>$mc["f_material"],
                        "qty"=> $mc["f_colorqty"] / $totalqty,
                        "comment" => $mc["f_reasonname"]
                    ];
                }
                $data->goods = $goods;
                $barcode_done[$barcode] = $this->CreateDoc($barcode, $mwc["f_colorqty"] / $size_count, $data);
            }
        }
        $this->result["completed_codes"] = $complete_codes;
        $this->result["barcode_done"] = $barcode_done;
        $this->echoResult();
    }

    private function CreateDoc($code, $qtyout, $params)
    {
        $goods = $this->select("select f_id from c_goods where f_scancode=?", "s", [$code])->fetch_assoc();
        if (empty($goods)) {
            return false;
        }

        $jdoc = [];
        $fInternalId = uuid_v4();
        $fCashDocUuid = uuid_v4();
        $state = 2;
        $fDocType = 6;
        $docnum = "---";

        /* ---------- HEADER ---------- */
        $jdoc['header'] = [
            'f_id'        => $fInternalId,
            'f_userid'    => $docnum,
            'f_state'     => $state,
            'f_type'      => $fDocType,
            'f_date'      => date('Y-m-d'),
            'f_operator'  => $this->userid,
            'f_partner'   => 0,
            'f_amount'    => 0,
            'f_currency'  => 1,
            'f_storein'   => 0,
            'f_storeout'  => 0,
            'f_reason'    => 8,
            'f_comment'   => Translator::t("Complectation") . " " . $code,
            'f_payment'   => null,
            'f_paid'      => 0
        ];

        /* ---------- BODY ---------- */
        $jbody = [
            'f_accepted'          => $this->userid,
            'f_passed'            => $this->userid,
            'f_invoice'           => null,
            'f_invoicedate'       => date('Y-m-d'),
            'f_reason'            => 8,
            'f_storein'           => 0,
            'f_storeout'          => 0,
            'f_complectationcode' => $goods["f_id"],
            'f_complectationqty'  => (float)$qtyout,
            'f_cashdoc'           => $fCashDocUuid,
            'f_basedonsale'       => 0
        ];

        /* ---------- ADD (tblAdd) ---------- */
        $jadd = [];
        foreach ($params->add as $row) {
            $jadd[] = [
                'f_name'   => (string)$row['name'],
                'f_amount' => (float)$row['amount'],
                'f_row'    => (int)$row["row"]
            ];
        }
        $jdoc['add'] = $jadd;

        /* ---------- COMPLECT ---------- */
        $jdoc['complect'] = [
            'f_goods' => $goods["f_id"],
            'f_qty'   => (float)$qtyout
        ];

        /* ---------- GOODS + REMAINS ---------- */
        $jgoods   = [];
        $jremains = [];

        foreach ($params->goods as $i => $g) {
            $jgoods[] = [
                'f_goods'   => (int)$g['goods'],
                'f_qty'     => (float)$g['qty'],
                'f_price'   => 0,
                'f_validto' => null,
                'f_comment' => $g["comment"],
                'f_row'     => $i
            ];

            //$jremains[(string)$g['goods']] = (float)$g['remain'];
        }

        $jbody['remains'] = $jremains;
        $jdoc['goods']    = $jgoods;

        /* ---------- PARTNER ---------- */
        $jdoc['partner'] = [
            'partner' => 0,
            'paid'    => 0,
            'cash'    => 0
        ];

        /* ---------- FINAL ---------- */
        $jdoc['body']    = $jbody;
        $jdoc['session'] = $session = uuid_v4();

        /* ---------- JSON ---------- */
        $json = json_encode($jdoc, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);


        $out = $this->callJsonProcedure(
            'sf_create_store_document',
            $json,true
        );


        if ($out === false) {
            throw new Exception($result['message'] ?? 'Unknown error');
        }

        $res = $this->select(
            "SELECT f_result FROM a_result WHERE f_session=?",
            "s",
            [$session]
        )->fetch_assoc();

        if (empty($res)) {
            throw new Exception("No result returned");
        }

        $result = json_decode($res['f_result'], true);

        if (($result['status'] ?? 0) == 0) {
            throw new Exception($result['message'] ?? 'Unknown error');
        }
        return true;
    }
}
