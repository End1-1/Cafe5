-- Generic key/JSON storage for app data blobs.
-- f_type = 1: inventory blank (Menu review)

CREATE TABLE IF NOT EXISTS sys_data (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_type INT NOT NULL,
    f_data JSON NOT NULL,
    KEY idx_sys_data_type (f_type)
);
