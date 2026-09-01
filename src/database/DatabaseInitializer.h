#pragma once

class DatabaseManager;

// Tạo toàn bộ bảng cần thiết cho hệ thống bằng CREATE TABLE IF NOT EXISTS.
// Idempotent: gọi lại mỗi lần app khởi động không gây lỗi và không xóa
// dữ liệu đã có. Class này chỉ chứa DDL (định nghĩa schema), không chứa
// business logic — đúng nguyên tắc Repository/Domain không biết SQLite,
// còn Database layer không biết business rule.
class DatabaseInitializer {
public:
    // Tạo toàn bộ bảng trên connection đã được mở của DatabaseManager.
    static void initialize(DatabaseManager& db);
};
