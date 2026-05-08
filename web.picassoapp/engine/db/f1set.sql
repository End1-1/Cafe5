DELIMITER $$
# ©2025, Kudryashov Vasili,
# Created - 16/05/2025 
DROP PROCEDURE if EXISTS sp_del_bclientsdebts $$
CREATE PROCEDURE sp_del_bclientsdebts (cond CHAR (36) COLLATE LATIN1_GENERAL_CI, delmode INT) 
BEGIN 
DECLARE c INT DEFAULT 0; 
IF delmode = 1 THEN
SELECT COUNT(f_id) INTO c
FROM b_clients_debts
WHERE f_order=cond; ELSEIF delmode = 2 THEN
  SELECT COUNT(f_id) INTO c
FROM b_clients_debts
WHERE f_order=cond;
ELSEIF delmode = 3 THEN
  SELECT COUNT(f_id) INTO c
FROM b_clients_debts
WHERE f_order=cond; 
ELSE SIGNAL SQLSTATE '45000'
  SET MESSAGE_TEXT = 'UNKNOWN MODE OF REMOVE';
END IF;
IF c > 1 THEN 
SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'REMOVE MORE THAN 1 ROW PROHIBITED'; 
END IF; 
IF delmode = 1 THEN
DELETE
FROM b_clients_debts
WHERE f_order=cond; 
ELSEIF delmode = 2 THEN
  DELETE
  FROM b_clients_debts WHERE f_order=cond; 
ELSEIF delmode = 3 THEN
  DELETE
  FROM b_clients_debts WHERE f_order=cond; 
ELSE SIGNAL SQLSTATE '45000'
  SET MESSAGE_TEXT = 'UNKNOWN MODE OF REMOVE';
END IF;
END$$
DELIMITER ;