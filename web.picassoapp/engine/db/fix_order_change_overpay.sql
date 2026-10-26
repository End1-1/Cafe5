-- Fix overpaid cash on closed orders (last 2 cash sessions).
-- Example: bill 4000, note 10000, change 6000 → store cash=4000, keep tendered in f_amount_cash_tendered.
-- Run step by step in HeidiSQL. Review SELECT before UPDATE.
-- cash_operations were posted as net on close — do not touch them.

-- 1) Preview last 2 sessions
SELECT f_id, f_cashbox_id, f_date_open, f_date_close, f_state
FROM cash_session
ORDER BY f_id DESC
LIMIT 2;

-- 2) Preview orders that need fix (change > 0 and cash > change)
SELECT
    oh.f_id,
    oh.f_prefix,
    oh.f_cash_session_id,
    oh.f_amounttotal AS due,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)) AS cash_now,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)) AS change_amt,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2))
      - CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)) AS cash_net
FROM o_header oh
WHERE oh.f_state = 2
  AND oh.f_cash_session_id IN (
        SELECT f_id FROM (
            SELECT f_id FROM cash_session ORDER BY f_id DESC LIMIT 2
        ) t
      )
  AND COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)), 0) > 0.009
  AND COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)), 0)
      > COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)), 0) + 0.009
ORDER BY oh.f_cash_session_id DESC, oh.f_id;

-- 3) APPLY: set cash = cash - change, keep tendered, set paid = due
--    (assumes change is fully taken from cash — normal Waiter case)
UPDATE o_header oh
SET oh.f_data = JSON_SET(
        oh.f_data,
        '$.f_amount_cash_tendered',
            CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)),
        '$.f_amount_cash',
            CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2))
          - CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)),
        '$.f_amount_paid',
            oh.f_amounttotal
    )
WHERE oh.f_state = 2
  AND oh.f_cash_session_id IN (
        SELECT f_id FROM (
            SELECT f_id FROM cash_session ORDER BY f_id DESC LIMIT 2
        ) t
      )
  AND COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)), 0) > 0.009
  AND COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)), 0)
      > COALESCE(CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)), 0) + 0.009;

-- 4) Check after update
SELECT
    oh.f_id,
    oh.f_prefix,
    oh.f_amounttotal AS due,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash_tendered')) AS DECIMAL(14,2)) AS cash_tendered,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_cash')) AS DECIMAL(14,2)) AS cash_net,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_change')) AS DECIMAL(14,2)) AS change_amt,
    CAST(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_amount_paid')) AS DECIMAL(14,2)) AS paid
FROM o_header oh
WHERE oh.f_state = 2
  AND oh.f_cash_session_id IN (
        SELECT f_id FROM (
            SELECT f_id FROM cash_session ORDER BY f_id DESC LIMIT 2
        ) t
      )
  AND JSON_EXTRACT(oh.f_data, '$.f_amount_cash_tendered') IS NOT NULL
ORDER BY oh.f_cash_session_id DESC, oh.f_id;
