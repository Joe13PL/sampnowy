/*
 * DATABASE SCHEMA (no ipb_ prefixes)
 * Run this in MySQL after creating the database the server connects to.
 */

SET NAMES utf8mb4;
SET CHARACTER SET utf8mb4;
SET COLLATION_CONNECTION = 'utf8mb4_unicode_ci';

-- ============================================================================
-- ACCOUNTS (users) + CHARACTERS
-- ============================================================================

CREATE TABLE IF NOT EXISTS `users` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `username` VARCHAR(24) NOT NULL,
    `password` VARCHAR(256) NOT NULL,
    `email` VARCHAR(64) DEFAULT NULL,
    `ip` VARCHAR(16) DEFAULT NULL,
    `serial` VARCHAR(64) DEFAULT NULL,
    `admin_level` TINYINT(1) DEFAULT 0,
    `vip_level` TINYINT(1) DEFAULT 0,
    `vip_expire` INT DEFAULT 0,
    `created_at` INT DEFAULT 0,
    `last_login` INT DEFAULT 0,
    `total_playtime` INT DEFAULT 0,
    `warns` TINYINT DEFAULT 0,
    `banned` TINYINT(1) DEFAULT 0,
    `ban_reason` VARCHAR(128) DEFAULT '',
    PRIMARY KEY (`id`),
    UNIQUE KEY `uq_username` (`username`),
    INDEX `idx_ip` (`ip`),
    INDEX `idx_serial` (`serial`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `characters` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `account_id` INT NOT NULL,
    `name` VARCHAR(24) NOT NULL,
    `gender` TINYINT(1) DEFAULT 0,
    `skin` INT DEFAULT 0,
    `age` INT DEFAULT 18,
    `level` INT DEFAULT 1,
    `respect` INT DEFAULT 0,
    `money` INT DEFAULT 0,
    `bank` INT DEFAULT 0,
    `hours` INT DEFAULT 0,
    `minutes` INT DEFAULT 0,
    `health` FLOAT DEFAULT 100.0,
    `armour` FLOAT DEFAULT 0.0,
    `pos_x` FLOAT DEFAULT 1481.0,
    `pos_y` FLOAT DEFAULT -1764.0,
    `pos_z` FLOAT DEFAULT 18.7,
    `pos_angle` FLOAT DEFAULT 0.0,
    `interior` INT DEFAULT 0,
    `world` INT DEFAULT 0,
    `hunger` INT DEFAULT 100,
    `thirst` INT DEFAULT 100,
    `spawns` INT DEFAULT 0,
    `kills` INT DEFAULT 0,
    `deaths` INT DEFAULT 0,
    `driven_km` FLOAT DEFAULT 0.0,
    `jail_time` INT DEFAULT 0,
    `aj_time` INT DEFAULT 0,
    `bw_time` INT DEFAULT 0,
    `penalty_points` INT DEFAULT 0,
    `strength` INT DEFAULT 0,
    `job` INT DEFAULT 0,
    `spawn_door` INT DEFAULT 0,
    `gametime` INT DEFAULT 0,
    `group1` INT DEFAULT 0,
    `group2` INT DEFAULT 0,
    `group3` INT DEFAULT 0,
    `group4` INT DEFAULT 0,
    `group5` INT DEFAULT 0,
    `group1_perm` INT DEFAULT 0,
    `group2_perm` INT DEFAULT 0,
    `group3_perm` INT DEFAULT 0,
    `group4_perm` INT DEFAULT 0,
    `group5_perm` INT DEFAULT 0,
    `group1_salary` INT DEFAULT 0,
    `group2_salary` INT DEFAULT 0,
    `group3_salary` INT DEFAULT 0,
    `group4_salary` INT DEFAULT 0,
    `group5_salary` INT DEFAULT 0,
    `created_at` INT DEFAULT 0,
    `last_login` INT DEFAULT 0,
    `last_save` INT DEFAULT 0,
    `online` TINYINT(1) DEFAULT 0,
    PRIMARY KEY (`id`),
    UNIQUE KEY `uq_character_name` (`name`),
    INDEX `idx_account` (`account_id`),
    CONSTRAINT `fk_char_account` FOREIGN KEY (`account_id`) REFERENCES `users`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `logged_players` (
    `char_uid` INT NOT NULL,
    `login_time` INT DEFAULT 0,
    PRIMARY KEY (`char_uid`),
    CONSTRAINT `fk_logged_char` FOREIGN KEY (`char_uid`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `game_sessions` (
    `session_uid` INT NOT NULL AUTO_INCREMENT,
    `char_uid` INT NOT NULL,
    `session_start` INT DEFAULT 0,
    `session_end` INT DEFAULT 0,
    PRIMARY KEY (`session_uid`),
    INDEX `idx_session_char` (`char_uid`),
    CONSTRAINT `fk_session_char` FOREIGN KEY (`char_uid`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================================================
-- PLAYER DATA (WEAPONS / ITEMS / BACKUPS)
-- ============================================================================

CREATE TABLE IF NOT EXISTS `user_weapons` (
    `weapon_uid` INT NOT NULL AUTO_INCREMENT,
    `weapon_owner` INT NOT NULL,
    `weapon_slot` TINYINT NOT NULL,
    `weapon_id` INT DEFAULT 0,
    `weapon_ammo` INT DEFAULT 0,
    PRIMARY KEY (`weapon_uid`),
    UNIQUE KEY `uq_weapon_owner_slot` (`weapon_owner`, `weapon_slot`),
    CONSTRAINT `fk_weapon_owner` FOREIGN KEY (`weapon_owner`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `items` (
    `item_id` INT NOT NULL AUTO_INCREMENT,
    `item_owner` INT NOT NULL,
    `item_owner_type` TINYINT DEFAULT 0,
    `item_type` INT DEFAULT 0,
    `item_value` INT DEFAULT 0,
    `item_value2` INT DEFAULT 0,
    `item_name` VARCHAR(64) DEFAULT '',
    PRIMARY KEY (`item_id`),
    INDEX `idx_item_owner` (`item_owner`, `item_owner_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `user_backups` (
    `backup_id` INT NOT NULL AUTO_INCREMENT,
    `backup_uid` INT NOT NULL,
    `backup_time` INT DEFAULT 0,
    `backup_money` INT DEFAULT 0,
    `backup_bank` INT DEFAULT 0,
    `backup_health` FLOAT DEFAULT 0.0,
    `backup_pos_x` FLOAT DEFAULT 0.0,
    `backup_pos_y` FLOAT DEFAULT 0.0,
    `backup_pos_z` FLOAT DEFAULT 0.0,
    PRIMARY KEY (`backup_id`),
    INDEX `idx_backup_uid` (`backup_uid`),
    CONSTRAINT `fk_backup_user` FOREIGN KEY (`backup_uid`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================================================
-- GROUPS / VEHICLES / DOORS / AREAS
-- ============================================================================

CREATE TABLE IF NOT EXISTS `groups` (
    `group_id` INT NOT NULL AUTO_INCREMENT,
    `group_name` VARCHAR(64) NOT NULL,
    `group_tag` VARCHAR(16) DEFAULT '',
    `group_type` TINYINT(2) DEFAULT 0,
    `group_color` INT DEFAULT 0,
    `group_bank` INT DEFAULT 0,
    `group_leader` INT DEFAULT 0,
    `group_max_members` INT DEFAULT 50,
    `group_flags` INT DEFAULT 0,
    `group_rank0` VARCHAR(32) DEFAULT '',
    `group_rank1` VARCHAR(32) DEFAULT '',
    `group_rank2` VARCHAR(32) DEFAULT '',
    `group_rank3` VARCHAR(32) DEFAULT '',
    `group_rank4` VARCHAR(32) DEFAULT '',
    `group_rank5` VARCHAR(32) DEFAULT '',
    `group_rank6` VARCHAR(32) DEFAULT '',
    `group_rank7` VARCHAR(32) DEFAULT '',
    `group_rank8` VARCHAR(32) DEFAULT '',
    `group_rank9` VARCHAR(32) DEFAULT '',
    PRIMARY KEY (`group_id`),
    UNIQUE KEY `uq_group_name` (`group_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `vehicles` (
    `veh_id` INT NOT NULL AUTO_INCREMENT,
    `veh_model` INT NOT NULL,
    `veh_pos_x` FLOAT DEFAULT 0.0,
    `veh_pos_y` FLOAT DEFAULT 0.0,
    `veh_pos_z` FLOAT DEFAULT 0.0,
    `veh_pos_a` FLOAT DEFAULT 0.0,
    `veh_color1` INT DEFAULT -1,
    `veh_color2` INT DEFAULT -1,
    `veh_owner_type` TINYINT DEFAULT 0,
    `veh_owner` INT DEFAULT 0,
    `veh_fuel` INT DEFAULT 100,
    `veh_mileage` FLOAT DEFAULT 0.0,
    PRIMARY KEY (`veh_id`),
    INDEX `idx_vehicle_owner` (`veh_owner`, `veh_owner_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `doors` (
    `door_id` INT NOT NULL AUTO_INCREMENT,
    `door_name` VARCHAR(64) NOT NULL,
    `door_type` TINYINT(2) DEFAULT 0,
    `door_owner_type` TINYINT DEFAULT 0,
    `door_owner` INT DEFAULT 0,
    `door_locked` TINYINT DEFAULT 0,
    `door_pickup` INT DEFAULT 0,
    `door_bank` INT DEFAULT 0,
    `door_ext_x` FLOAT DEFAULT 0.0,
    `door_ext_y` FLOAT DEFAULT 0.0,
    `door_ext_z` FLOAT DEFAULT 0.0,
    `door_ext_a` FLOAT DEFAULT 0.0,
    `door_ext_interior` INT DEFAULT 0,
    `door_ext_vw` INT DEFAULT 0,
    `door_int_x` FLOAT DEFAULT 0.0,
    `door_int_y` FLOAT DEFAULT 0.0,
    `door_int_z` FLOAT DEFAULT 0.0,
    `door_int_a` FLOAT DEFAULT 0.0,
    `door_int_interior` INT DEFAULT 0,
    `door_int_vw` INT DEFAULT 0,
    `door_objects_limit` INT DEFAULT 0,
    `door_auto_closing` TINYINT DEFAULT 0,
    `door_car_crosing` TINYINT DEFAULT 0,
    `door_payment` INT DEFAULT 0,
    `door_map_icon` INT DEFAULT 0,
    `door_closed` TINYINT DEFAULT 0,
    `door_rentable` TINYINT DEFAULT 0,
    `door_rent` INT DEFAULT 0,
    `door_surface` FLOAT DEFAULT 0.0,
    `door_time` INT DEFAULT 0,
    `door_access` INT DEFAULT 0,
    `door_destroyed` INT DEFAULT 0,
    `door_burned` INT DEFAULT 0,
    `door_meters` INT DEFAULT 0,
    `door_audio` VARCHAR(255) DEFAULT '',
    `door_area` INT DEFAULT 0,
    `door_demolition` INT DEFAULT 0,
    PRIMARY KEY (`door_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `areas` (
    `area_id` INT NOT NULL AUTO_INCREMENT,
    `area_name` VARCHAR(64) NOT NULL,
    `area_type` TINYINT(2) DEFAULT 0,
    `area_owner` INT DEFAULT 0,
    `area_min_x` FLOAT DEFAULT 0.0,
    `area_min_y` FLOAT DEFAULT 0.0,
    `area_min_z` FLOAT DEFAULT -100.0,
    `area_max_x` FLOAT DEFAULT 0.0,
    `area_max_y` FLOAT DEFAULT 0.0,
    `area_max_z` FLOAT DEFAULT 100.0,
    `area_interior` INT DEFAULT 0,
    `area_vw` INT DEFAULT 0,
    PRIMARY KEY (`area_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================================================
-- ADMIN / SECURITY / KNOWN PLAYERS
-- ============================================================================

CREATE TABLE IF NOT EXISTS `bans` (
    `ban_id` INT NOT NULL AUTO_INCREMENT,
    `ban_player` INT DEFAULT 0,
    `ban_ip` VARCHAR(20) DEFAULT '',
    `ban_serial` VARCHAR(128) DEFAULT '',
    `ban_admin` INT DEFAULT 0,
    `ban_reason` VARCHAR(128) NOT NULL,
    `ban_time` INT DEFAULT 0,
    `ban_expire` INT DEFAULT 0,
    PRIMARY KEY (`ban_id`),
    INDEX `idx_ban_player` (`ban_player`),
    INDEX `idx_ban_ip` (`ban_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `admin_logs` (
    `log_id` INT NOT NULL AUTO_INCREMENT,
    `log_admin` INT NOT NULL,
    `log_action` VARCHAR(255) NOT NULL,
    `log_time` INT DEFAULT 0,
    PRIMARY KEY (`log_id`),
    INDEX `idx_log_admin` (`log_admin`),
    INDEX `idx_log_time` (`log_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `known_players` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `player_uid` INT NOT NULL,
    `target_uid` INT NOT NULL,
    `custom_name` VARCHAR(32) NOT NULL,
    `created_at` INT DEFAULT 0,
    PRIMARY KEY (`id`),
    UNIQUE KEY `uq_known_pair` (`player_uid`, `target_uid`),
    INDEX `idx_player` (`player_uid`),
    INDEX `idx_target` (`target_uid`),
    CONSTRAINT `fk_known_player` FOREIGN KEY (`player_uid`) REFERENCES `characters`(`id`) ON DELETE CASCADE,
    CONSTRAINT `fk_known_target` FOREIGN KEY (`target_uid`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================================================
-- PHONES AND SMS TABLES
-- ============================================================================

