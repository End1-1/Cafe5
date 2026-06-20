<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-04-25 00:26:28

require_once __DIR__ . '/fmt.php';

function money_fmt_php($val)
{
    return money_fmt($val);
}

/** Translate template key and replace placeholders; never store need_translate in DB. */
function translate_template(string $key, array $replacements = []): string
{
    $text = Translator::t($key);
    if (str_starts_with($text, 'need_translate')) {
        $text = $key;
    }

    foreach ($replacements as $from => $to) {
        $text = str_replace($from, (string)$to, $text);
    }

    return $text;
}

/** Normalize legacy/auto-generated transfer comments for display. */
function translate_cash_operation_comment(string $comment): string
{
    $comment = trim($comment);
    if ($comment === '') {
        return $comment;
    }

    $patterns = [
        '/^(?:need_translate:?\s*)?Transfer to cashbox (\d+)(.*)$/u' => 'Transfer to cashbox %cashbox',
        '/^(?:need_translate:?\s*)?Transfer from cashbox (\d+)(.*)$/u' => 'Transfer from cashbox %cashbox',
    ];

    foreach ($patterns as $regex => $templateKey) {
        if (!preg_match($regex, $comment, $m)) {
            continue;
        }

        return translate_template($templateKey, ['%cashbox' => $m[1]]) . ($m[2] ?? '');
    }

    return $comment;
}
