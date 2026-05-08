<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-25 14:03:36
# Last modified - 2025-03-31 00:46:42
require_once __DIR__ . "/../app.php";

class CreateMedia extends PClass
{
    public function buildmenu()
    {
        $mediaPath = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/dishes";
        if (!file_exists($mediaPath)) {
            if (!mkdir($mediaPath, "777", true)) {
                $this->exitError("Cannot create media path " . $mediaPath);
            }
        }
        $this->deleteFilesInDirectory($mediaPath);
        $handle = fopen($_SERVER['DOCUMENT_ROOT'] . "/engine/media/dishes/index.html", "w");
        fclose($handle);
        $images = $this->stmtall("select * from s_images")->fetch_all(MYSQLI_ASSOC);
        foreach ($images as $i) {
            $imageData = base64_decode($i["f_data"]);
            if ($imageData === false) {
                $this->exitError("Decode error for " . $i["f_id"]);
            }
            $filePath = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/dishes/" . $i["f_id"] . ".png";
            if (file_put_contents($filePath, $imageData) !== false) {

            } else {
                $this->exitError("Cannot write file of " . $i["f_id"]);
            }
        }
        $this->echoResult();
    }

    public function buildGoods()
    {
        $mediaPath = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/goods";
        if (!file_exists($mediaPath)) {
            if (!mkdir($mediaPath, "777", true)) {
                $this->exitError("Cannot create media path " . $mediaPath);
            }
        }
        $this->deleteFilesInDirectory($mediaPath);
        $handle = fopen($_SERVER['DOCUMENT_ROOT'] . "/engine/media/goods/index.html", "w");
        fclose($handle);
        $images = $this->stmtall("select * from c_goods_images")->fetch_all(MYSQLI_ASSOC);
        foreach ($images as $i) {
            if (!empty($i["f_image"])) {
                $imageData = base64_decode($i["f_image"]);
                if ($imageData === false) {
                    $this->exitError("Decode error for " . $i["f_id"]);
                }
                $filePath = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/goods/s" . $i["f_id"] . ".jpg";
                if (file_put_contents($filePath, $imageData) !== false) {

                } else {
                    $this->exitError("Cannot write file of " . $i["f_id"]);
                }
            }
            if (!empty($i["f_bigimage"])) {
                $imageData = base64_decode($i["f_bigimage"]);
                if ($imageData === false) {
                    $this->exitError("Decode error for " . $i["f_id"]);
                }
                $filePath = $_SERVER['DOCUMENT_ROOT'] . "/engine/media/goods/b" . $i["f_id"] . ".jpg";
                if (file_put_contents($filePath, $imageData) !== false) {

                } else {
                    $this->exitError("Cannot write file of " . $i["f_id"]);
                }
            }
        }
        $this->echoResult();
    }

    private function deleteFilesInDirectory($dir)
    {
        if (!is_dir($dir)) {
            $this->exitError("$dir is not dir");
        }
        $files = scandir($dir);
        foreach ($files as $file) {
            if ($file == '.' || $file == '..') {
                continue;
            }
            $filePath = $dir . DIRECTORY_SEPARATOR . $file;
            if (is_dir($filePath)) {
                $this->deleteFilesInDirectory($filePath);
                rmdir($filePath);
            } else {
                unlink($filePath);
            }
        }
    }
}

if (!empty($params->mode)) {
    $cd = new CreateMedia();
    $cd->{$params->mode}();
}