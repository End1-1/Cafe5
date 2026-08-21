<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Faq extends ArarixAuth
{
    public function List($params)
    {
        $locale = strtolower(trim((string)($params->locale ?? Translator::$locale ?: "en")));
        if (!in_array($locale, ["en", "ru", "hy"], true)) {
            $locale = "en";
        }

        $sql = <<<EOD
        SELECT f.f_id AS id,
               COALESCE(tr.f_question, en.f_question) AS question,
               COALESCE(tr.f_answer, en.f_answer) AS answer
        FROM ararix_faq f
        LEFT JOIN ararix_faq_tr tr ON tr.f_faq_id = f.f_id AND tr.f_lang = ?
        LEFT JOIN ararix_faq_tr en ON en.f_faq_id = f.f_id AND en.f_lang = 'en'
        WHERE f.f_active = 1
        ORDER BY f.f_sort, f.f_id
        EOD;

        $this->result["items"] = $this->select($sql, "s", [$locale])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
