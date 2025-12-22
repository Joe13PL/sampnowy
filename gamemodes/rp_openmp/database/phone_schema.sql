-- Table for managing phone numbers
-- Added `state` to track whether a number's device is online/off/offline/destroyed
CREATE TABLE IF NOT EXISTS phone_numbers (
    id INT AUTO_INCREMENT PRIMARY KEY,
    phone_number VARCHAR(15) NOT NULL UNIQUE,
    state ENUM('on','off','destroyed') DEFAULT 'on',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Optional mapping of player accounts (uid) to primary phone numbers
CREATE TABLE IF NOT EXISTS phone_owners (
    id INT AUTO_INCREMENT PRIMARY KEY,
    owner_uid INT NOT NULL,
    phone_number VARCHAR(15) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY (owner_uid, phone_number),
    FOREIGN KEY (phone_number) REFERENCES phone_numbers(phone_number)
);

-- Optional mapping of item devices to phone numbers (for phones as items)
CREATE TABLE IF NOT EXISTS phone_devices (
    id INT AUTO_INCREMENT PRIMARY KEY,
    phone_number VARCHAR(15) NOT NULL,
    item_id INT NOT NULL,
    owner_uid INT NOT NULL,
    owner_type INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY (phone_number),
    FOREIGN KEY (phone_number) REFERENCES phone_numbers(phone_number)
);

-- Table for storing SMS messages (offline-capable)
CREATE TABLE IF NOT EXISTS phone_sms (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sender_number VARCHAR(15) NOT NULL,
    receiver_number VARCHAR(15) NOT NULL,
    message TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_read TINYINT(1) DEFAULT 0,
    FOREIGN KEY (sender_number) REFERENCES phone_numbers(phone_number),
    FOREIGN KEY (receiver_number) REFERENCES phone_numbers(phone_number)
);

-- Table for storing contacts (per phone number)
CREATE TABLE IF NOT EXISTS phone_contacts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    phone_number VARCHAR(15) NOT NULL,
    contact_number VARCHAR(15) NOT NULL,
    contact_name VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (phone_number) REFERENCES phone_numbers(phone_number)
);

-- Table for storing call history
CREATE TABLE IF NOT EXISTS phone_calls (
    id INT AUTO_INCREMENT PRIMARY KEY,
    caller_number VARCHAR(15) NOT NULL,
    callee_number VARCHAR(15) NOT NULL,
    start_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    end_time TIMESTAMP NULL,
    status ENUM('missed', 'answered', 'rejected') NOT NULL,
    FOREIGN KEY (caller_number) REFERENCES phone_numbers(phone_number),
    FOREIGN KEY (callee_number) REFERENCES phone_numbers(phone_number)
);