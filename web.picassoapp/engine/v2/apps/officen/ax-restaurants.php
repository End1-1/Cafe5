<?php
# © 2026 , Kudryashov Vasili
# CRUD for Ararix restaurants (FrontDesk dashboard editor).

require_once __DIR__ . '/index.php';

class AxRestaurants extends Auth
{
    private const MEDIA_REL = '/engine/media/ararix';
    private const MAX_IMAGE_BYTES = 2500000;

    private const SAVE_RULES = [
        'f_id' => 'integer|nullable',
        'f_name' => 'required|string|max:128',
        'f_score' => 'integer|nullable',
        'f_category' => 'nullable|string|max:128',
        'f_nationality_id' => 'integer|nullable',
        'f_image_url' => 'nullable|string|max:255',
        'f_image_data' => 'nullable|string',
        'f_remove_image' => 'integer|nullable',
        'f_lat' => 'nullable|numeric',
        'f_lng' => 'nullable|numeric',
    ];

    private function nationalityList(): array
    {
        $rows = $this->select(
            'SELECT f_id, f_name FROM ararix_restaurant_nationality ORDER BY f_sort, f_name'
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r['f_id'] = (int)$r['f_id'];
        }
        unset($r);
        return $rows;
    }

    private function mediaDir(): string
    {
        $root = rtrim((string)($_SERVER['DOCUMENT_ROOT'] ?? ''), '/\\');
        if ($root === '') {
            $root = dirname(__DIR__, 3); // web.picassoapp
        }
        return $root . str_replace('/', DIRECTORY_SEPARATOR, self::MEDIA_REL);
    }

    private function publicUrl(string $filename): string
    {
        return self::MEDIA_REL . '/' . ltrim($filename, '/');
    }

    private function absoluteUrl(?string $url): string
    {
        $url = trim((string)$url);
        if ($url === '') {
            return '';
        }
        if (preg_match('#^https?://#i', $url)) {
            return $url;
        }
        $scheme = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') ? 'https' : 'http';
        $host = (string)($_SERVER['HTTP_HOST'] ?? '');
        if ($host === '') {
            return $url;
        }
        return $scheme . '://' . $host . (str_starts_with($url, '/') ? $url : '/' . $url);
    }

    private function isManagedMedia(?string $url): bool
    {
        $url = trim((string)$url);
        if ($url === '') {
            return false;
        }
        $path = parse_url($url, PHP_URL_PATH);
        if (!is_string($path) || $path === '') {
            $path = $url;
        }
        return str_starts_with($path, self::MEDIA_REL . '/');
    }

    private function deleteManagedFile(?string $url): void
    {
        if (!$this->isManagedMedia($url)) {
            return;
        }
        $path = parse_url($url, PHP_URL_PATH);
        if (!is_string($path) || $path === '') {
            $path = $url;
        }
        $name = basename($path);
        if ($name === '' || $name === '.' || $name === '..') {
            return;
        }
        $full = $this->mediaDir() . DIRECTORY_SEPARATOR . $name;
        if (is_file($full)) {
            @unlink($full);
        }
    }

    private function saveUploadedImage(?string $oldUrl, ?string $imageData, bool $remove): ?string
    {
        if ($remove) {
            $this->deleteManagedFile($oldUrl);
            return null;
        }

        if ($imageData === null || $imageData === '') {
            return $oldUrl !== null && $oldUrl !== '' ? $oldUrl : null;
        }

        $bin = base64_decode($imageData, true);
        if ($bin === false || $bin === '') {
            dieWithCode(Translator::t('Invalid image data'));
        }
        if (strlen($bin) > self::MAX_IMAGE_BYTES) {
            dieWithCode(Translator::t('Image is too large'));
        }

        $finfo = new finfo(FILEINFO_MIME_TYPE);
        $mime = (string)$finfo->buffer($bin);
        $ext = match ($mime) {
            'image/jpeg' => 'jpg',
            'image/png' => 'png',
            'image/webp' => 'webp',
            'image/gif' => 'gif',
            default => '',
        };
        if ($ext === '') {
            dieWithCode(Translator::t('Unsupported image type'));
        }

        $dir = $this->mediaDir();
        if (!is_dir($dir) && !mkdir($dir, 0775, true) && !is_dir($dir)) {
            dieWithCode(Translator::t('Cannot create media folder'));
        }

        $filename = 'rest_' . date('YmdHis') . '_' . bin2hex(random_bytes(4)) . '.' . $ext;
        $full = $dir . DIRECTORY_SEPARATOR . $filename;
        if (file_put_contents($full, $bin) === false) {
            dieWithCode(Translator::t('Cannot save image'));
        }

        $this->deleteManagedFile($oldUrl);
        return $this->publicUrl($filename);
    }

    private function rowById(int $id): ?array
    {
        $row = $this->select(
            "SELECT f_id,
                    f_name,
                    f_score,
                    COALESCE(f_category, '') AS f_category,
                    f_nationality_id,
                    COALESCE(f_image_url, '') AS f_image_url,
                    IF(f_location IS NULL, NULL, ST_Y(f_location)) AS f_lat,
                    IF(f_location IS NULL, NULL, ST_X(f_location)) AS f_lng
             FROM ararix_restaurants
             WHERE f_id = ?",
            'i',
            [$id]
        )->fetch_assoc();

        if (!$row) {
            return null;
        }

        $row['f_id'] = (int)$row['f_id'];
        $row['f_score'] = (int)$row['f_score'];
        $natId = isset($row['f_nationality_id']) ? (int)$row['f_nationality_id'] : 0;
        $row['f_nationality_id'] = $natId > 0 ? $natId : 0;
        $row['f_lat'] = isset($row['f_lat']) ? (float)$row['f_lat'] : null;
        $row['f_lng'] = isset($row['f_lng']) ? (float)$row['f_lng'] : null;
        $row['f_image_url'] = $this->absoluteUrl($row['f_image_url'] ?? '');
        return $row;
    }

    public function Get($params)
    {
        $this->result['nationalities'] = $this->nationalityList();

        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            $this->result['row'] = [
                'f_id' => 0,
                'f_name' => '',
                'f_score' => 0,
                'f_category' => '',
                'f_nationality_id' => 0,
                'f_image_url' => '',
                'f_lat' => null,
                'f_lng' => null,
            ];
            $this->echoResult();
            return;
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->result['row'] = $row;
        $this->echoResult();
    }

    public function Save($params)
    {
        $data = $this->ValidateParams($params, self::SAVE_RULES);

        $name = trim((string)$data->f_name);
        if ($name === '') {
            dieWithCode(Translator::t('Name is required'));
        }

        $score = (int)($data->f_score ?? 0);
        $category = trim((string)($data->f_category ?? ''));
        $categoryDb = $category !== '' ? $category : null;

        $nationalityId = (int)($data->f_nationality_id ?? 0);
        $nationalityDb = null;
        if ($nationalityId > 0) {
            $nat = $this->select(
                'SELECT f_id FROM ararix_restaurant_nationality WHERE f_id = ?',
                'i',
                [$nationalityId]
            )->fetch_assoc();
            if ($nat === null) {
                dieWithCode(Translator::t('Nationality not found'));
            }
            $nationalityDb = $nationalityId;
        }

        $latRaw = $data->f_lat ?? null;
        $lngRaw = $data->f_lng ?? null;
        $hasLat = $latRaw !== null && $latRaw !== '';
        $hasLng = $lngRaw !== null && $lngRaw !== '';
        if ($hasLat xor $hasLng) {
            dieWithCode(Translator::t('Latitude and longitude must be set together'));
        }

        $id = (int)($data->f_id ?? 0);
        $existing = $id > 0 ? $this->rowById($id) : null;
        if ($id > 0 && $existing === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $oldUrl = '';
        if ($existing) {
            // rowById absolutizes; recover stored relative when managed
            $raw = $this->select(
                'SELECT COALESCE(f_image_url, \'\') AS f_image_url FROM ararix_restaurants WHERE f_id = ?',
                'i',
                [$id]
            )->fetch_assoc();
            $oldUrl = (string)($raw['f_image_url'] ?? '');
        }

        $removeImage = !empty($data->f_remove_image);
        $imageData = isset($data->f_image_data) ? (string)$data->f_image_data : '';
        $manualUrl = trim((string)($data->f_image_url ?? ''));

        if ($removeImage) {
            $imageDb = $this->saveUploadedImage($oldUrl, null, true);
        } elseif ($imageData !== '') {
            $imageDb = $this->saveUploadedImage($oldUrl, $imageData, false);
        } elseif ($manualUrl !== '') {
            // External / pasted URL — drop previous managed file if replaced
            if ($this->isManagedMedia($oldUrl) && !$this->isManagedMedia($manualUrl)
                && $oldUrl !== $manualUrl && parse_url($manualUrl, PHP_URL_PATH) !== parse_url($oldUrl, PHP_URL_PATH)) {
                $this->deleteManagedFile($oldUrl);
            }
            $path = parse_url($manualUrl, PHP_URL_PATH);
            $imageDb = (is_string($path) && str_starts_with($path, self::MEDIA_REL . '/'))
                ? $path
                : $manualUrl;
        } else {
            $imageDb = $oldUrl !== '' ? $oldUrl : null;
        }

        if ($id <= 0) {
            $id = (int)$this->insert('ararix_restaurants', (object)[
                'f_name' => $name,
                'f_score' => $score,
                'f_category' => $categoryDb,
                'f_nationality_id' => $nationalityDb,
                'f_image_url' => $imageDb,
            ]);
        } else {
            $this->update('ararix_restaurants', (object)[
                'f_name' => $name,
                'f_score' => $score,
                'f_category' => $categoryDb,
                'f_nationality_id' => $nationalityDb,
                'f_image_url' => $imageDb,
            ], $id);
        }

        if ($hasLat && $hasLng) {
            $lat = (float)$latRaw;
            $lng = (float)$lngRaw;
            $this->select(
                "UPDATE ararix_restaurants
                 SET f_location = ST_PointFromText(CONCAT('POINT(', ?, ' ', ?, ')'))
                 WHERE f_id = ?",
                'ddi',
                [$lng, $lat, $id],
                true
            );
        } else {
            $this->select(
                "UPDATE ararix_restaurants SET f_location = NULL WHERE f_id = ?",
                'i',
                [$id],
                true
            );
        }

        $this->result['row'] = $this->rowById($id);
        $this->result['f_id'] = $id;
        $this->echoResult();
    }

    public function Remove($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Id is required'));
        }

        $raw = $this->select(
            'SELECT COALESCE(f_image_url, \'\') AS f_image_url FROM ararix_restaurants WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();
        if ($raw === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->deleteManagedFile($raw['f_image_url'] ?? '');
        $this->delete('ararix_restaurants', $id);
        $this->echoResult();
    }
}
