<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 20:26:06
# Last Modified: 2025-05-25 20:26:10

class Translator
{
    private static $tr = null;
    private $translations = [];
    private $file;
     public static $locale;

    public function __construct($lang)
    {
        Translator::$locale = $lang;
        $this->file = __DIR__ . "/tr_{$lang}.json";
        if (file_exists($this->file)) {
            $json = file_get_contents($this->file);
            $this->translations = json_decode($json, true) ?? [];
        }
    }

    public static function t(string $key): string
    {
        if (!Translator::$tr) {
            Translator::$tr = new self(LANG);
        }

        if (isset(Translator::$tr->translations[$key])) {
            return Translator::$tr->translations[$key];
        }

        Translator::$tr->translations[$key] = "need_translate $key";

        file_put_contents(
            Translator::$tr->file,
            json_encode(Translator::$tr->translations, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE),
            LOCK_EX
        );

        return "need_translate: $key";
    }
}
