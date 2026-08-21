<?php
defined('APP') or die('Die, vampire!');

if (!defined('LANG')) {
	define('LANG', 'hy');
}

require_once __DIR__ . '/v2/worker/translator.php';

/**
 * Legacy API strings → v2 Translator (tr_hy.json / tr_ru.json).
 * Locale is set in app.php from request JSON "locale".
 */
function tr($s)
{
	return Translator::t((string)$s);
}

/**
 * Normalize client locale (UI am/ru → API hy/ru).
 */
function applyRequestLocale($params): void
{
	$locale = LANG;
	if (!empty($params) && !empty($params->locale)) {
		$locale = strtolower(trim((string)$params->locale));
	}
	if ($locale === 'am') {
		$locale = 'hy';
	}
	if (!in_array($locale, ['hy', 'ru'], true)) {
		$locale = LANG;
	}
	Translator::$locale = $locale;
}
