# © 2026 , Kudryashov Vasili
# Prerequisite: rows must exist — run repair_missing_o_goods_process.sql first if needed.
#
# One-time repair: kitchen status times were stored under a wrong JSON key
# ($jsonTimePath / jsonTimePath) instead of $.f_status_{status}_{substatus}_time
#
# Run on a backup first. Safe to re-run: only fills empty target keys, then removes legacy keys.
#
# 1) Preview rows to fix:
# SELECT ogp.f_id, ogp.f_header, ogp.f_status,
#        JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_substatus')) AS f_substatus,
#        JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$."$jsonTimePath"')) AS legacy_dollar,
#        JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.jsonTimePath')) AS legacy_plain,
#        JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, CONCAT(
#            '$.f_status_', ogp.f_status, '_',
#            COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_substatus')), ''), '1'),
#            '_time'
#        ))) AS target_before
# FROM o_goods_process ogp
# WHERE ogp.f_status BETWEEN 1 AND 4
#   AND (
#       JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$."$jsonTimePath"')
#       OR JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$.jsonTimePath')
#       OR JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$.$jsonTimePath')
#   );

UPDATE o_goods_process ogp
SET ogp.f_data = JSON_REMOVE(
    JSON_SET(
        COALESCE(ogp.f_data, '{}'),
        CONCAT(
            '$.f_status_',
            ogp.f_status,
            '_',
            COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_substatus')), ''), '1'),
            '_time'
        ),
        COALESCE(
            NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(
                ogp.f_data,
                CONCAT(
                    '$.f_status_',
                    ogp.f_status,
                    '_',
                    COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_substatus')), ''), '1'),
                    '_time'
                )
            ))), ''),
            NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$."$jsonTimePath"'))), ''),
            NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.jsonTimePath'))), ''),
            NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.$jsonTimePath'))), '')
        )
    ),
    '$."$jsonTimePath"',
    '$.jsonTimePath',
    '$.$jsonTimePath'
)
WHERE ogp.f_status BETWEEN 1 AND 4
  AND (
      JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$."$jsonTimePath"')
      OR JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$.jsonTimePath')
      OR JSON_CONTAINS_PATH(COALESCE(ogp.f_data, '{}'), 'one', '$.$jsonTimePath')
  )
  AND COALESCE(
      NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(
          ogp.f_data,
          CONCAT(
              '$.f_status_',
              ogp.f_status,
              '_',
              COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_substatus')), ''), '1'),
              '_time'
          )
      ))), ''),
      NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$."$jsonTimePath"'))), ''),
      NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.jsonTimePath'))), ''),
      NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.$jsonTimePath'))), '')
  ) IS NOT NULL;
