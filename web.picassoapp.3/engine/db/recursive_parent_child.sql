WITH RECURSIVE cte AS (
    SELECT
        *,
        0 AS level
    FROM d_part2
    WHERE f_parent =0
    UNION ALL
    SELECT
        c.*,
        cte.level + 1
    FROM d_part2 c
    JOIN cte ON c.f_parent = cte.f_id
)
SELECT 
    c.f_id,
    c.f_name,
    c.f_parent,
    cc.f_name AS parent_name,
    GROUP_CONCAT(DISTINCT s.f_id) AS sinbling_ids,
    GROUP_CONCAT(DISTINCT s.f_name) AS sibling_names,
    GROUP_CONCAT(DISTINCT ch.f_id) AS children_ids,
    GROUP_CONCAT(DISTINCT ch.f_name) AS childrent_names
FROM cte c
LEFT JOIN d_part2 cc ON c.f_parent = cc.f_id
LEFT JOIN cte s ON c.level = s.level AND s.f_id != c.f_id
LEFT JOIN d_part2 ch ON ch.f_parent = c.f_id
GROUP BY c.f_id, c.f_name, c.f_parent, cc.f_name