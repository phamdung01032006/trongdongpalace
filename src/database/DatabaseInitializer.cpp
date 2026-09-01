#include "DatabaseInitializer.h"
#include "DatabaseManager.h"

#include <vector>
#include <string>

void DatabaseInitializer::initialize(DatabaseManager& db) {
    // Thứ tự tạo bảng: bảng cha trước bảng con để dễ đọc (SQLite không
    // bắt buộc thứ tự này khi dùng CREATE TABLE IF NOT EXISTS, nhưng
    // sắp theo dependency giúp schema dễ hiểu hơn).
    std::vector<std::string> statements = {

        // users — tài khoản đăng nhập, dùng cho Authentication/Authorization
        R"(CREATE TABLE IF NOT EXISTS users (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            username      TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            role          TEXT NOT NULL,
            status        TEXT NOT NULL DEFAULT 'ACTIVE'
        );)",

        // centers — tạo trước halls vì halls.center_id tham chiếu tới đây
        R"(CREATE TABLE IF NOT EXISTS centers (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL,
            address TEXT,
            phone   TEXT
        );)",

        // customers
        R"(CREATE TABLE IF NOT EXISTS customers (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            customer_code TEXT NOT NULL UNIQUE,
            name          TEXT NOT NULL,
            phone         TEXT,
            email         TEXT,
            address       TEXT
        );)",

        // halls
        R"(CREATE TABLE IF NOT EXISTS halls (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            center_id INTEGER NOT NULL,
            hall_code TEXT NOT NULL UNIQUE,
            name      TEXT NOT NULL,
            capacity  INTEGER NOT NULL,
            status    TEXT NOT NULL DEFAULT 'ACTIVE',
            FOREIGN KEY (center_id) REFERENCES centers (id)
        );)",

        // events — events.customer_id (FK theo yêu cầu)
        R"(CREATE TABLE IF NOT EXISTS events (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            event_code  TEXT NOT NULL UNIQUE,
            customer_id INTEGER NOT NULL,
            event_type  TEXT,
            event_date  TEXT NOT NULL,
            start_time  TEXT NOT NULL,
            end_time    TEXT NOT NULL,
            guest_count INTEGER NOT NULL,
            status      TEXT NOT NULL DEFAULT 'PLANNED',
            FOREIGN KEY (customer_id) REFERENCES customers (id)
        );)",

        // bookings — bookings.event_id, bookings.hall_id (FK theo yêu cầu)
        R"(CREATE TABLE IF NOT EXISTS bookings (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            booking_code   TEXT NOT NULL UNIQUE,
            event_id       INTEGER NOT NULL,
            hall_id        INTEGER NOT NULL,
            start_datetime TEXT NOT NULL,
            end_datetime   TEXT NOT NULL,
            status         TEXT NOT NULL DEFAULT 'CONFIRMED',
            FOREIGN KEY (event_id) REFERENCES events (id),
            FOREIGN KEY (hall_id) REFERENCES halls (id)
        );)",

        // packages
        R"(CREATE TABLE IF NOT EXISTS packages (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            package_code TEXT NOT NULL UNIQUE,
            name         TEXT NOT NULL,
            description  TEXT,
            base_price   REAL NOT NULL DEFAULT 0,
            status       TEXT NOT NULL DEFAULT 'ACTIVE'
        );)",

        // services
        R"(CREATE TABLE IF NOT EXISTS services (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            service_code TEXT NOT NULL UNIQUE,
            name         TEXT NOT NULL,
            description  TEXT,
            unit         TEXT,
            unit_price   REAL NOT NULL DEFAULT 0,
            status       TEXT NOT NULL DEFAULT 'ACTIVE'
        );)",

        // package_services — bảng nối many-to-many Package <-> Service
        R"(CREATE TABLE IF NOT EXISTS package_services (
            package_id       INTEGER NOT NULL,
            service_id       INTEGER NOT NULL,
            default_quantity INTEGER NOT NULL DEFAULT 1,
            PRIMARY KEY (package_id, service_id),
            FOREIGN KEY (package_id) REFERENCES packages (id),
            FOREIGN KEY (service_id) REFERENCES services (id)
        );)",

        // booking_services — Service thêm trực tiếp vào Booking (FR-SVC-003)
        R"(CREATE TABLE IF NOT EXISTS booking_services (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            booking_id INTEGER NOT NULL,
            service_id INTEGER NOT NULL,
            quantity   INTEGER NOT NULL DEFAULT 1,
            unit_price REAL NOT NULL,
            amount     REAL NOT NULL,
            FOREIGN KEY (booking_id) REFERENCES bookings (id),
            FOREIGN KEY (service_id) REFERENCES services (id)
        );)",

        // quotations — quotations.booking_id (FK theo yêu cầu)
        R"(CREATE TABLE IF NOT EXISTS quotations (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            quotation_code TEXT NOT NULL UNIQUE,
            booking_id     INTEGER NOT NULL,
            package_id     INTEGER,
            subtotal       REAL NOT NULL DEFAULT 0,
            discount       REAL NOT NULL DEFAULT 0,
            tax            REAL NOT NULL DEFAULT 0,
            total          REAL NOT NULL DEFAULT 0,
            status         TEXT NOT NULL DEFAULT 'DRAFT',
            FOREIGN KEY (booking_id) REFERENCES bookings (id),
            FOREIGN KEY (package_id) REFERENCES packages (id)
        );)",

        // quotation_items — lưu snapshot đơn giá tại thời điểm báo giá
        R"(CREATE TABLE IF NOT EXISTS quotation_items (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            quotation_id INTEGER NOT NULL,
            service_id   INTEGER NOT NULL,
            description  TEXT,
            quantity     INTEGER NOT NULL DEFAULT 1,
            unit_price   REAL NOT NULL,
            amount       REAL NOT NULL,
            FOREIGN KEY (quotation_id) REFERENCES quotations (id),
            FOREIGN KEY (service_id) REFERENCES services (id)
        );)",

        // contracts — contracts.quotation_id (FK theo yêu cầu)
        R"(CREATE TABLE IF NOT EXISTS contracts (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            contract_code TEXT NOT NULL UNIQUE,
            quotation_id  INTEGER NOT NULL,
            customer_id   INTEGER NOT NULL,
            event_id      INTEGER NOT NULL,
            total_amount  REAL NOT NULL DEFAULT 0,
            status        TEXT NOT NULL DEFAULT 'DRAFT',
            contract_date TEXT NOT NULL,
            FOREIGN KEY (quotation_id) REFERENCES quotations (id),
            FOREIGN KEY (customer_id) REFERENCES customers (id),
            FOREIGN KEY (event_id) REFERENCES events (id)
        );)",

        // payments — payments.contract_id (FK theo yêu cầu)
        R"(CREATE TABLE IF NOT EXISTS payments (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            payment_code TEXT NOT NULL UNIQUE,
            contract_id  INTEGER NOT NULL,
            amount       REAL NOT NULL,
            payment_date TEXT NOT NULL,
            method       TEXT NOT NULL,
            note         TEXT,
            FOREIGN KEY (contract_id) REFERENCES contracts (id)
        );)",

        // staff_assignments — staff_id tham chiếu users (xem ghi chú ở README)
        R"(CREATE TABLE IF NOT EXISTS staff_assignments (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            event_id        INTEGER NOT NULL,
            staff_id        INTEGER NOT NULL,
            assignment_role TEXT,
            status          TEXT NOT NULL DEFAULT 'ASSIGNED',
            FOREIGN KEY (event_id) REFERENCES events (id),
            FOREIGN KEY (staff_id) REFERENCES users (id)
        );)",

        // special_requests
        R"(CREATE TABLE IF NOT EXISTS special_requests (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            event_id    INTEGER NOT NULL,
            description TEXT NOT NULL,
            status      TEXT NOT NULL DEFAULT 'PENDING',
            note        TEXT,
            FOREIGN KEY (event_id) REFERENCES events (id)
        );)",

        // incidents
        R"(CREATE TABLE IF NOT EXISTS incidents (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            event_id    INTEGER NOT NULL,
            description TEXT NOT NULL,
            severity    TEXT NOT NULL DEFAULT 'LOW',
            status      TEXT NOT NULL DEFAULT 'OPEN',
            resolution  TEXT,
            FOREIGN KEY (event_id) REFERENCES events (id)
        );)"
    };

    for (const auto& sql : statements) {
        db.execute(sql);
    }
}
