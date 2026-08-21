-- Ararix schema
-- Run manually against the target database.

INSERT INTO s_app (f_app, f_version)
SELECT 'ararix', '1'
WHERE NOT EXISTS (SELECT 1 FROM s_app WHERE LOWER(f_app) = 'ararix');

UPDATE s_app SET f_version = '1' WHERE LOWER(f_app) = 'ararix';

CREATE TABLE IF NOT EXISTS ararix_clients (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_country_code VARCHAR(8) NOT NULL DEFAULT '+374',
    f_phone VARCHAR(32) NOT NULL,
    f_name VARCHAR(128) NULL,
    f_email VARCHAR(128) NULL,
    f_password_hash VARCHAR(255) NULL,
    f_locale VARCHAR(8) NOT NULL DEFAULT 'en',
    f_status INT NOT NULL DEFAULT 1,
    f_avatar_url VARCHAR(255) NULL,
    f_created DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    f_updated DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uq_ararix_clients_phone (f_country_code, f_phone)
);

CREATE TABLE IF NOT EXISTS ararix_otp (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_country_code VARCHAR(8) NOT NULL,
    f_phone VARCHAR(32) NOT NULL,
    f_code VARCHAR(8) NOT NULL,
    f_channel VARCHAR(16) NOT NULL DEFAULT 'sms',
    f_purpose VARCHAR(32) NOT NULL DEFAULT 'login',
    f_expires_at DATETIME NOT NULL,
    f_used INT NOT NULL DEFAULT 0,
    f_created DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    KEY idx_ararix_otp_phone (f_country_code, f_phone, f_used)
);

CREATE TABLE IF NOT EXISTS ararix_sessions (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_client_id INT NOT NULL,
    f_token CHAR(36) NOT NULL,
    f_created DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    f_expires_at DATETIME NULL,
    UNIQUE KEY uq_ararix_sessions_token (f_token),
    KEY idx_ararix_sessions_client (f_client_id),
    CONSTRAINT fk_ararix_sessions_client FOREIGN KEY (f_client_id) REFERENCES ararix_clients(f_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS ararix_payment_cards (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_client_id INT NOT NULL,
    f_brand VARCHAR(32) NOT NULL DEFAULT 'card',
    f_last4 VARCHAR(4) NOT NULL,
    f_holder VARCHAR(128) NULL,
    f_created DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    KEY idx_ararix_cards_client (f_client_id),
    CONSTRAINT fk_ararix_cards_client FOREIGN KEY (f_client_id) REFERENCES ararix_clients(f_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS ararix_faq (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_sort INT NOT NULL DEFAULT 0,
    f_active INT NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS ararix_faq_tr (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_faq_id INT NOT NULL,
    f_lang VARCHAR(8) NOT NULL,
    f_question VARCHAR(512) NOT NULL,
    f_answer TEXT NOT NULL,
    UNIQUE KEY uq_ararix_faq_tr (f_faq_id, f_lang),
    CONSTRAINT fk_ararix_faq_tr FOREIGN KEY (f_faq_id) REFERENCES ararix_faq(f_id) ON DELETE CASCADE
);

INSERT IGNORE INTO ararix_faq (f_id, f_sort, f_active) VALUES (1, 1, 1), (2, 2, 1), (3, 3, 1);

INSERT IGNORE INTO ararix_faq_tr (f_faq_id, f_lang, f_question, f_answer) VALUES
    (1, 'en', 'Who are the couriers', 'Our couriers are trained partners who deliver your orders safely and on time.'),
    (1, 'ru', 'Кто такие курьеры', 'Наши курьеры — обученные партнёры, которые доставляют заказы безопасно и вовремя.'),
    (1, 'hy', 'Ովքե՞ր են առաքիչները', 'Մեր առաքիչները վերապատրաստված գործընկերներ են, ովքեր ապահով և ժամանակին առաքում են ձեր պատվերները։'),
    (2, 'en', 'How much does it cost', 'Delivery cost depends on distance and order size. The exact amount is shown before checkout.'),
    (2, 'ru', 'Сколько это стоит', 'Стоимость доставки зависит от расстояния и размера заказа. Точная сумма видна перед оплатой.'),
    (2, 'hy', 'Որքա՞ն է արժե', 'Առաքման արժեքը կախված է հեռավորությունից և պատվերի չափից։ Ճշգրիտ գումարը երևում է վճարումից առաջ։'),
    (3, 'en', 'How to change my phone number', 'Open Profile, Account, then Phone number and verify the new number with an OTP code.'),
    (3, 'ru', 'Как сменить номер телефона', 'Откройте Профиль, Аккаунт, затем Номер телефона и подтвердите новый номер кодом OTP.'),
    (3, 'hy', 'Ինչպե՞ս փոխել հեռախոսահամարը', 'Բացեք Պրոֆիլը, Հաշիվը, ապա Հեռախոսահամարը և հաստատեք նոր համարը OTP կոդով։');

CREATE TABLE IF NOT EXISTS ararix_restaurants (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_name VARCHAR(128) NOT NULL,
    f_score INT NOT NULL DEFAULT 0,
    f_location POINT NULL,
    f_image_url VARCHAR(255) NULL,
    f_category VARCHAR(128) NULL
);

CREATE TABLE IF NOT EXISTS ararix_goods_groups (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_name VARCHAR(128) NOT NULL,
    f_image_url VARCHAR(255) NULL,
    f_sort INT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS ararix_goods_country (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_name VARCHAR(128) NOT NULL,
    f_sort INT NOT NULL DEFAULT 0
);

INSERT IGNORE INTO ararix_goods_groups (f_id, f_name, f_sort) VALUES
    (1, 'Burger', 1),
    (2, 'Pizza', 2);

INSERT IGNORE INTO ararix_goods_country (f_id, f_name, f_sort) VALUES
    (1, 'Asian', 1),
    (2, 'Caucasus', 2),
    (3, 'Europian', 3),
    (4, 'Mexican', 4),
    (5, 'Japan', 5);
