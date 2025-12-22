-- Migration 0001: Add extra columns to `doors` table
-- Run this on existing database to add the missing columns introduced in the refactor.

ALTER TABLE `doors`
  ADD COLUMN IF NOT EXISTS `door_int_vw` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_objects_limit` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_auto_closing` TINYINT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_car_crosing` TINYINT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_payment` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_map_icon` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_closed` TINYINT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_rentable` TINYINT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_rent` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_surface` FLOAT DEFAULT 0.0,
  ADD COLUMN IF NOT EXISTS `door_time` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_access` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_destroyed` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_burned` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_meters` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_audio` VARCHAR(255) DEFAULT '',
  ADD COLUMN IF NOT EXISTS `door_area` INT DEFAULT 0,
  ADD COLUMN IF NOT EXISTS `door_demolition` INT DEFAULT 0;