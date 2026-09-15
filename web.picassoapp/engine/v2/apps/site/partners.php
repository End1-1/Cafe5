<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

/**
 * Site API for c_partners.
 * Paths:
 *   POST /engine/v2/site/partners/get
 *   POST /engine/v2/site/partners/edit
 *
 * Phone format (strict): +374XXXXXXXX (e.g. +37477019107).
 * Uniqueness key: full phone string in that format.
 * Edit: if phone exists → update; otherwise → insert.
 */
class Partners extends Auth
{
    /** Strict AM mobile: +374 + 8 digits. */
    private const PHONE_PATTERN = '/^\+374\d{8}$/';

    public function Get($params)
    {
        $phone = $this->pickString($params, ["phone", "f_phone"]);
        $id = (int)($params->id ?? $params->f_id ?? 0);

        if ($id <= 0 && $phone === "") {
            dieWithCode("phone or id is required", 400);
        }

        $row = null;
        if ($id > 0) {
            $row = $this->select(
                "SELECT * FROM c_partners WHERE f_id=? LIMIT 1",
                "i",
                [$id]
            )->fetch_assoc();
        } else {
            $phone = $this->assertPhoneFormat($phone);
            $row = $this->findByPhone($phone);
        }

        if (!$row) {
            dieWithCode("Partner not found", 404);
        }

        $this->result["partner"] = $this->formatPartner($row);
        $this->echoResult();
    }

    public function Edit($params)
    {
        $phone = $this->assertPhoneFormat(
            $this->pickString($params, ["phone", "f_phone"])
        );

        $name = $this->pickString($params, ["name", "f_name", "f_taxname", "f_contact"]);
        $address = $this->pickString($params, ["address", "f_address"]);
        $legalAddress = $this->pickString($params, ["legal_address", "f_legal_address"]);
        $email = $this->pickString($params, ["email", "f_email"]);
        $taxcode = $this->pickString($params, ["taxcode", "f_taxcode"]);
        $taxname = $this->pickString($params, ["taxname", "f_taxname"]);
        $contact = $this->pickString($params, ["contact", "f_contact"]);
        $info = $this->pickString($params, ["info", "f_info"]);

        if ($taxname === "" && $name !== "") {
            $taxname = $name;
        }
        if ($contact === "" && $name !== "") {
            $contact = $name;
        }
        if ($name === "" && $taxname !== "") {
            $name = $taxname;
        }
        if ($name === "" && $contact !== "") {
            $name = $contact;
        }

        $category = (int)($params->category ?? $params->f_category ?? 0);
        $group = (int)($params->group ?? $params->f_group ?? 0);
        $state = (int)($params->state ?? $params->f_state ?? 0);
        $permanentDiscount = $params->permanent_discount ?? $params->f_permanent_discount ?? null;
        $pricePolitic = $params->price_politic ?? $params->f_price_politic ?? null;
        $manager = $params->manager ?? $params->f_manager ?? null;

        $existing = $this->findByPhone($phone);
        $created = false;

        if ($existing) {
            $partnerId = (int)$existing["f_id"];
            $upd = [
                "f_phone" => $phone,
            ];
            if ($name !== "") {
                $upd["f_name"] = $name;
            }
            if ($taxname !== "") {
                $upd["f_taxname"] = $taxname;
            }
            if ($contact !== "") {
                $upd["f_contact"] = $contact;
            }
            if ($address !== "") {
                $upd["f_address"] = $address;
            }
            if ($legalAddress !== "") {
                $upd["f_legal_address"] = $legalAddress;
            }
            if ($email !== "") {
                $upd["f_email"] = $email;
            }
            if ($taxcode !== "") {
                $upd["f_taxcode"] = $taxcode;
            }
            if ($info !== "") {
                $upd["f_info"] = $info;
            }
            if ($category > 0) {
                $upd["f_category"] = $category;
            }
            if ($group > 0) {
                $upd["f_group"] = $group;
            }
            if ($state > 0) {
                $upd["f_state"] = $state;
            }
            if ($permanentDiscount !== null && $permanentDiscount !== "") {
                $upd["f_permanent_discount"] = (float)$permanentDiscount;
            }
            if ($pricePolitic !== null && $pricePolitic !== "") {
                $upd["f_price_politic"] = (int)$pricePolitic;
            }
            if ($manager !== null && $manager !== "") {
                $upd["f_manager"] = (int)$manager;
            }
            $this->update("c_partners", $upd, $partnerId);
        } else {
            $created = true;
            if ($name === "") {
                $name = $phone;
            }
            if ($taxname === "") {
                $taxname = $name;
            }
            if ($contact === "") {
                $contact = $name;
            }
            $fields = [
                "f_phone" => $phone,
                "f_name" => $name,
                "f_taxname" => $taxname,
                "f_contact" => $contact,
                "f_address" => $address,
                "f_legal_address" => $legalAddress,
                "f_email" => $email,
                "f_taxcode" => $taxcode,
                "f_info" => $info,
                "f_category" => $category > 0 ? $category : 1,
                "f_group" => $group > 0 ? $group : 1,
                "f_state" => $state > 0 ? $state : 1,
            ];
            if ($permanentDiscount !== null && $permanentDiscount !== "") {
                $fields["f_permanent_discount"] = (float)$permanentDiscount;
            }
            if ($pricePolitic !== null && $pricePolitic !== "") {
                $fields["f_price_politic"] = (int)$pricePolitic;
            }
            if ($manager !== null && $manager !== "") {
                $fields["f_manager"] = (int)$manager;
            }
            $partnerId = (int)$this->insert("c_partners", $fields);
        }

        $row = $this->select(
            "SELECT * FROM c_partners WHERE f_id=? LIMIT 1",
            "i",
            [$partnerId]
        )->fetch_assoc();

        $this->result["created"] = $created;
        $this->result["partner"] = $this->formatPartner($row ?: ["f_id" => $partnerId]);
        $this->echoResult();
    }

    private function assertPhoneFormat(string $phone): string
    {
        $phone = trim($phone);
        if ($phone === "") {
            dieWithCode("phone is required", 400);
        }
        if (!preg_match(self::PHONE_PATTERN, $phone)) {
            dieWithCode("Invalid phone format. Expected +374XXXXXXXX (e.g. +37477019107)", 400);
        }
        return $phone;
    }

    private function findByPhone(string $phone): ?array
    {
        $row = $this->select(
            "SELECT * FROM c_partners WHERE f_phone=? LIMIT 1",
            "s",
            [$phone]
        )->fetch_assoc();
        return $row ?: null;
    }

    private function pickString(object $params, array $keys): string
    {
        foreach ($keys as $key) {
            if (!isset($params->{$key})) {
                continue;
            }
            $v = trim((string)$params->{$key});
            if ($v !== "") {
                return $v;
            }
        }
        return "";
    }

    private function formatPartner(array $row): array
    {
        return [
            "id" => (int)($row["f_id"] ?? 0),
            "category" => isset($row["f_category"]) ? (int)$row["f_category"] : null,
            "state" => isset($row["f_state"]) ? (int)$row["f_state"] : null,
            "group" => isset($row["f_group"]) ? (int)$row["f_group"] : null,
            "taxcode" => $row["f_taxcode"] ?? "",
            "taxname" => $row["f_taxname"] ?? "",
            "name" => $row["f_name"] ?? "",
            "contact" => $row["f_contact"] ?? "",
            "phone" => $row["f_phone"] ?? "",
            "email" => $row["f_email"] ?? "",
            "address" => $row["f_address"] ?? "",
            "legal_address" => $row["f_legal_address"] ?? "",
            "info" => $row["f_info"] ?? "",
            "permanent_discount" => isset($row["f_permanent_discount"])
                ? (float)$row["f_permanent_discount"]
                : null,
            "price_politic" => isset($row["f_price_politic"]) ? (int)$row["f_price_politic"] : null,
            "manager" => isset($row["f_manager"]) ? (int)$row["f_manager"] : null,
        ];
    }
}
