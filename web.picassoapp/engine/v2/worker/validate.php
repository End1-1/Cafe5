<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 20:17:20
# Last Modified: 2025-05-25 20:17:23

function validateParams($params, array $requiredFields): void
{
    foreach ($requiredFields as $field) {
        if (!isset($params->$field)) {
            if (DEBUG) {
            dieWithCode("Missing required field: $field in request to {$_SERVER['REQUEST_URI']}", 400);
        } else {
            error_log("Missing required field: $field in request to {$_SERVER['REQUEST_URI']}");
            dieWithCode("Validation error", 400);
        }
        }
    }
}
