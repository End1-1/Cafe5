<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 20:26:06
# Last Modified: 2025-05-25 20:26:10

class Translator
{
    private static $tr = null;
    private $translations = [];
    private $file;
    private $lang;
    public static $locale;

    public function __construct($lang)
    {
        $this->lang = $lang;
        Translator::$locale = $lang;
        $this->file = __DIR__ . "/tr_{$lang}.json";
        $this->reload();
    }

    private function reload(): void
    {
        $this->translations = [];
        if (!file_exists($this->file)) {
            return;
        }

        $json = file_get_contents($this->file);
        $this->translations = json_decode($json, true) ?? [];
    }

    private function resolve(string $key): ?string
    {
        if (isset($this->translations[$key])) {
            $value = $this->translations[$key];
            if ($value !== '' && !str_starts_with($value, 'need_translate ')) {
                return $value;
            }
        }

        // File may have been edited after this worker cached translations.
        $this->reload();
        if (!isset($this->translations[$key])) {
            return null;
        }

        $value = $this->translations[$key];
        if ($value === '' || str_starts_with($value, 'need_translate ')) {
            return null;
        }

        return $value;
    }

    public static function t(string $key): string
    {
        $locale = Translator::$locale ?? LANG;

        if (!Translator::$tr || Translator::$tr->lang !== $locale) {
            Translator::$tr = new self($locale);
        }

        $value = Translator::$tr->resolve($key);
        if ($value !== null) {
            return $value;
        }

        Translator::$tr->translations[$key] = "need_translate $key";

        file_put_contents(
            Translator::$tr->file,
            json_encode(
                Translator::$tr->translations,
                JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES
            ),
            LOCK_EX
        );

        return "need_translate: $key";
    }
}
