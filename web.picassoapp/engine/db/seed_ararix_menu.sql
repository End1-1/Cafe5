-- Ararix menu seed (idempotent): dish types, restaurant group sets, demo dishes.
-- Safe to re-run: skips restaurants that already have groups / dishes.

CREATE TABLE IF NOT EXISTS ararix_restaurant_groups (
    f_restaurant_id INT NOT NULL,
    f_group_id INT NOT NULL,
    f_sort INT NOT NULL DEFAULT 0,
    PRIMARY KEY (f_restaurant_id, f_group_id),
    INDEX idx_ararix_rg_group (f_group_id)
);

CREATE TABLE IF NOT EXISTS ararix_menu (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_restaurant_id INT NOT NULL,
    f_group_id INT NOT NULL,
    f_name VARCHAR(128) NOT NULL,
    f_description TEXT NULL,
    f_price DECIMAL(12,2) NOT NULL DEFAULT 0,
    f_image_url VARCHAR(255) NULL,
    f_state TINYINT NOT NULL DEFAULT 1,
    f_sort INT NOT NULL DEFAULT 0,
    f_source_goods_id INT NULL,
    f_source_db VARCHAR(64) NULL,
    f_data JSON NULL,
    INDEX idx_ararix_menu_rest_state (f_restaurant_id, f_state),
    INDEX idx_ararix_menu_group (f_group_id)
);

INSERT INTO ararix_goods_groups (f_id, f_name, f_sort) VALUES
    (1, 'Բուրգեր', 1),
    (2, 'Պիցցա', 2),
    (3, 'Սուշի', 3),
    (4, 'Սթեյք', 4),
    (5, 'BBQ', 5),
    (6, 'Ծովամթերք', 6),
    (7, 'Ֆասթֆուդ', 7),
    (8, 'Street Food', 8),
    (9, 'Վեգան', 9),
    (10, 'Վեգետարիանական', 10),
    (11, 'Առողջ սնունդ', 11),
    (12, 'Դեսերտներ / Հացաբուլկեղեն', 12),
    (13, 'Սրճարան / Coffee & Dessert', 13)
ON DUPLICATE KEY UPDATE f_name = VALUES(f_name), f_sort = VALUES(f_sort);

-- nationality_id (0 = default) → dish groups
DROP TEMPORARY TABLE IF EXISTS tmp_nat_groups;
CREATE TEMPORARY TABLE tmp_nat_groups (
    f_nationality_id INT NOT NULL,
    f_group_id INT NOT NULL,
    f_sort INT NOT NULL,
    PRIMARY KEY (f_nationality_id, f_group_id)
);

INSERT INTO tmp_nat_groups (f_nationality_id, f_group_id, f_sort) VALUES
-- default
(0, 1, 1), (0, 2, 2), (0, 7, 3), (0, 12, 4), (0, 13, 5),
-- 1 Chinese
(1, 7, 1), (1, 8, 2), (1, 6, 3), (1, 11, 4), (1, 12, 5),
-- 2 Japanese
(2, 3, 1), (2, 6, 2), (2, 11, 3), (2, 12, 4), (2, 13, 5),
-- 3 Indian
(3, 7, 1), (3, 8, 2), (3, 9, 3), (3, 10, 4), (3, 12, 5),
-- 4 European
(4, 4, 1), (4, 7, 2), (4, 10, 3), (4, 11, 4), (4, 12, 5), (4, 13, 6),
-- 5 Italian
(5, 2, 1), (5, 7, 2), (5, 10, 3), (5, 12, 4), (5, 13, 5),
-- 6 Armenian
(6, 4, 1), (6, 5, 2), (6, 7, 3), (6, 12, 4), (6, 13, 5),
-- 7 Georgian
(7, 4, 1), (7, 5, 2), (7, 7, 3), (7, 12, 4), (7, 13, 5),
-- 8 Lebanese
(8, 8, 1), (8, 10, 2), (8, 11, 3), (8, 12, 4), (8, 13, 5),
-- 9 Arabic
(9, 5, 1), (9, 7, 2), (9, 8, 3), (9, 12, 4), (9, 13, 5),
-- 10 Turkish
(10, 5, 1), (10, 7, 2), (10, 8, 3), (10, 12, 4), (10, 13, 5),
-- 11 Persian
(11, 4, 1), (11, 7, 2), (11, 10, 3), (11, 12, 4), (11, 13, 5),
-- 12 Mediterranean
(12, 6, 1), (12, 10, 2), (12, 11, 3), (12, 12, 4), (12, 13, 5),
-- 13 American
(13, 1, 1), (13, 4, 2), (13, 5, 3), (13, 7, 4), (13, 8, 5),
-- 14 Mexican
(14, 1, 1), (14, 5, 2), (14, 7, 3), (14, 8, 4), (14, 12, 5);

INSERT IGNORE INTO ararix_restaurant_groups (f_restaurant_id, f_group_id, f_sort)
SELECT r.f_id, t.f_group_id, t.f_sort
FROM ararix_restaurants r
INNER JOIN tmp_nat_groups t ON t.f_nationality_id = COALESCE(r.f_nationality_id, 0)
WHERE NOT EXISTS (
    SELECT 1 FROM ararix_restaurant_groups rg WHERE rg.f_restaurant_id = r.f_id
);

-- dish name pool per group (7 names → pick 3..7 per restaurant)
DROP TEMPORARY TABLE IF EXISTS tmp_dishes;
CREATE TEMPORARY TABLE tmp_dishes (
    f_group_id INT NOT NULL,
    f_idx INT NOT NULL,
    f_name VARCHAR(128) NOT NULL,
    f_price DECIMAL(12,2) NOT NULL,
    PRIMARY KEY (f_group_id, f_idx)
);

INSERT INTO tmp_dishes (f_group_id, f_idx, f_name, f_price) VALUES
(1, 1, 'Classic Burger', 2200),
(1, 2, 'Cheese Burger', 2500),
(1, 3, 'Double Burger', 3200),
(1, 4, 'Bacon Burger', 2900),
(1, 5, 'Chicken Burger', 2400),
(1, 6, 'Veggie Burger', 2300),
(1, 7, 'Smash Burger', 2700),
(2, 1, 'Margherita', 2800),
(2, 2, 'Pepperoni', 3200),
(2, 3, 'Quattro Formaggi', 3500),
(2, 4, 'Capricciosa', 3400),
(2, 5, 'Hawaiian', 3100),
(2, 6, 'Diavola', 3300),
(2, 7, 'Funghi', 3000),
(3, 1, 'Philadelphia Roll', 4200),
(3, 2, 'California Roll', 3800),
(3, 3, 'Salmon Nigiri', 2800),
(3, 4, 'Tuna Roll', 4000),
(3, 5, 'Dragon Roll', 4800),
(3, 6, 'Tempura Roll', 3900),
(3, 7, 'Eel Roll', 4500),
(4, 1, 'Ribeye Steak', 8900),
(4, 2, 'Sirloin Steak', 7500),
(4, 3, 'Filet Mignon', 9800),
(4, 4, 'T-Bone Steak', 9200),
(4, 5, 'Pepper Steak', 8200),
(4, 6, 'BBQ Steak', 8000),
(4, 7, 'Steak Frites', 7800),
(5, 1, 'BBQ Ribs', 6500),
(5, 2, 'Pulled Pork', 4800),
(5, 3, 'Smoked Brisket', 7200),
(5, 4, 'BBQ Chicken', 4200),
(5, 5, 'BBQ Wings', 3500),
(5, 6, 'BBQ Platter', 8500),
(5, 7, 'Sausage Grill', 3900),
(6, 1, 'Grilled Salmon', 6200),
(6, 2, 'Shrimp Pasta', 5400),
(6, 3, 'Fish & Chips', 4200),
(6, 4, 'Seafood Soup', 3800),
(6, 5, 'Calamari', 4100),
(6, 6, 'Lobster Roll', 8900),
(6, 7, 'Mussels Pot', 4700),
(7, 1, 'Combo Meal', 2900),
(7, 2, 'Chicken Nuggets', 2200),
(7, 3, 'Fries Box', 1200),
(7, 4, 'Hot Dog', 1800),
(7, 5, 'Chicken Wrap', 2500),
(7, 6, 'Onion Rings', 1400),
(7, 7, 'Kids Meal', 2000),
(8, 1, 'Shawarma', 2000),
(8, 2, 'Falafel Wrap', 1800),
(8, 3, 'Street Tacos', 2400),
(8, 4, 'Bao Bun', 2200),
(8, 5, 'Corn Dog', 1500),
(8, 6, 'Loaded Fries', 1900),
(8, 7, 'Flatbread', 2100),
(9, 1, 'Buddha Bowl', 3200),
(9, 2, 'Vegan Burger', 2800),
(9, 3, 'Tofu Stir Fry', 3000),
(9, 4, 'Vegan Pasta', 3100),
(9, 5, 'Chickpea Salad', 2600),
(9, 6, 'Avocado Toast', 2400),
(9, 7, 'Vegan Curry', 2900),
(10, 1, 'Caprese', 2800),
(10, 2, 'Veggie Pizza', 3000),
(10, 3, 'Mushroom Risotto', 3500),
(10, 4, 'Greek Salad', 2500),
(10, 5, 'Eggplant Bake', 2900),
(10, 6, 'Cheese Platter', 4200),
(10, 7, 'Veggie Wrap', 2300),
(11, 1, 'Quinoa Bowl', 3200),
(11, 2, 'Grilled Chicken Salad', 3400),
(11, 3, 'Poke Bowl', 4500),
(11, 4, 'Detox Smoothie Bowl', 2800),
(11, 5, 'Turkey Wrap', 2700),
(11, 6, 'Steamed Fish', 4800),
(11, 7, 'Green Salad', 2200),
(12, 1, 'Cheesecake', 2200),
(12, 2, 'Brownie', 1800),
(12, 3, 'Napoleon', 2000),
(12, 4, 'Baklava', 1600),
(12, 5, 'Tiramisu', 2400),
(12, 6, 'Fruit Tart', 2100),
(12, 7, 'Croissant', 1200),
(13, 1, 'Cappuccino', 1200),
(13, 2, 'Latte', 1300),
(13, 3, 'Americano', 900),
(13, 4, 'Espresso', 700),
(13, 5, 'Hot Chocolate', 1400),
(13, 6, 'Iced Coffee', 1500),
(13, 7, 'Tea Set', 1100);

INSERT INTO ararix_menu
    (f_restaurant_id, f_group_id, f_name, f_description, f_price, f_state, f_sort)
SELECT
    rg.f_restaurant_id,
    rg.f_group_id,
    d.f_name,
    '',
    d.f_price + ((rg.f_restaurant_id * 17 + rg.f_group_id * 13 + d.f_idx * 50) % 400),
    1,
    d.f_idx
FROM ararix_restaurant_groups rg
INNER JOIN tmp_dishes d ON d.f_group_id = rg.f_group_id
WHERE d.f_idx <= (3 + ((rg.f_restaurant_id + rg.f_group_id) % 5))
  AND NOT EXISTS (
      SELECT 1
      FROM ararix_menu m
      WHERE m.f_restaurant_id = rg.f_restaurant_id
        AND m.f_group_id = rg.f_group_id
  );

DROP TEMPORARY TABLE IF EXISTS tmp_dishes;
DROP TEMPORARY TABLE IF EXISTS tmp_nat_groups;
