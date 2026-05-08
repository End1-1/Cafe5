<?php

return [
    "save" => [
        "c_partners" =>   [
            "f_id" => "integer|nullable",
            "f_category" => "integer",
            "f_state" => "integer",
            "f_taxcode" => "string|max:8",
            "f_taxname" => "string|max:100",
            "f_name" => "string|max:100",
            "f_contact" => "string|max:100",
            "f_phone" => "string|max:64",
            "f_email" => "nullable|string|max:32",
            "f_address" => "nullable|string|max:128",
            "f_legal_address" => "nullable|string|max:128",
            "f_permanent_discount" => "nullable|numeric",
            "f_manager" => "nullable|integer"
        ]
    ],
    "select" => [
        "c_partners" => [
            "sql" => <<<EOD
            select p.f_id, p.f_category, p.f_state, 
            d1.f_value as f_category_name,d2.f_value as f_state_name,
            p.f_taxcode, p.f_taxname, p.f_name, p.f_phone, p.f_contact, p.f_info,
            p.f_address, p.f_legal_address
            from c_partners p
            left join l_dictionary d1 on d1.f_dict='c_partners_category' and d1.f_dict_id=p.f_category and d1.f_lang=?
            left join l_dictionary d2 on d2.f_dict='c_partners_state' and d2.f_dict_id=p.f_category and d2.f_lang=?
            where 1=1
            order by p.f_name
            EOD
        ]
    ]
];
