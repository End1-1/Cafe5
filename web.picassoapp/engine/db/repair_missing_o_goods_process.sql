# © 2026 , Kudryashov Vasili
# Backfill o_goods_process for active o_goods lines that never got a kitchen row.
# Kitchen status times are NOT stored in o_goods.f_data — only append/print/etc. live there.
#
# Preview:
# SELECT og.f_id, og.f_header, og.f_goods,
#        JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_append_time')) AS f_append_time
# FROM o_goods og
# LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
# WHERE og.f_state = 1 AND ogp.f_id IS NULL
# LIMIT 100;

INSERT INTO o_goods_process (f_id, f_header, f_status, f_daily_number, f_data)
SELECT
    og.f_id,
    og.f_header,
    1,
    COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_daily_number')), ''), '-'),
    JSON_OBJECT(
        'f_status_1_1_time',
        COALESCE(
            NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_append_time'))), ''),
            DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
        ),
        'f_substatus', 1,
        'f_cooking_start', '1981-09-05 00:01:00',
        'f_cooking_end', DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
    )
FROM o_goods og
INNER JOIN o_header oh ON oh.f_id = og.f_header
LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
WHERE og.f_state = 1
  AND ogp.f_id IS NULL;

# After this, run repair_o_goods_process_status_times.sql for legacy $jsonTimePath keys.
