# Trong Dong Palace — Event Management System (Console, C++)

Project skeleton — Day 2 (theo `PROJECT_CHECKLIST.md`): project structure + kết nối SQLite.
Chưa có chức năng nghiệp vụ.

## Cấu trúc thư mục

```text
src/
├── models/         # Domain entities (Customer, Hall, Event, Booking, ...)
├── services/        # Business logic (BookingService, PaymentService, ...)
├── repositories/     # Data access (SQLite), tách khỏi business logic
├── exceptions/       # AppException và các exception con
├── database/         # DatabaseManager - quản lý kết nối SQLite
├── ui/               # Console menu (MainMenu, CustomerMenu, ...)
├── utils/            # Hàm tiện ích dùng chung
└── main.cpp
```

## Yêu cầu

- CMake >= 3.10 (cài đặt qua installer chính chủ của CMake: https://cmake.org/download/)
- msys2

## Build & Run (Linux/macOS)

```bash
mkdir build
cd build
cmake ..
make
./TrongDongPalace
```

## Build & Run (Windows, MinGW + CMake)

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
.\TrongDongPalace.exe
```

Nếu dùng Visual Studio: mở thư mục project bằng "Open Folder", VS sẽ tự nhận `CMakeLists.txt`.

## Kết quả mong đợi

```text
Database connected successfully.
Database schema initialized successfully.
```

Chương trình sẽ tạo ra file `trong_dong_palace.db` tại thư mục chạy chương trình
(thường là thư mục `build/`) và tạo toàn bộ 17 bảng bên trong. Chạy lại chương
trình nhiều lần không gây lỗi (idempotent — dùng `CREATE TABLE IF NOT EXISTS`)
và không mất dữ liệu đã có.

## DatabaseManager

`src/database/DatabaseManager.h/.cpp` bọc một kết nối SQLite duy nhất với 3 hàm chính:

```text
open()     — mở kết nối tới file .db, bật PRAGMA foreign_keys = ON,
             ném std::runtime_error nếu thất bại
close()    — đóng kết nối, an toàn khi gọi nhiều lần
execute()  — chạy câu SQL không trả về dòng dữ liệu (CREATE TABLE, INSERT, ...)
```

Repository ở các bước sau sẽ dùng `DatabaseManager` này để đọc/ghi dữ liệu;
Service và Domain object không được truy cập SQLite trực tiếp.

## DatabaseInitializer

`src/database/DatabaseInitializer.h/.cpp` chứa toàn bộ DDL (`CREATE TABLE IF NOT EXISTS`)
cho 17 bảng: `users, customers, centers, halls, events, bookings, packages, services,
package_services, booking_services, quotations, quotation_items, contracts, payments,
staff_assignments, special_requests, incidents`.

Foreign key chính đã thiết lập đúng yêu cầu:

```text
events.customer_id      → customers.id
bookings.event_id       → events.id
bookings.hall_id        → halls.id
quotations.booking_id   → bookings.id
contracts.quotation_id  → quotations.id
payments.contract_id    → contracts.id
```

Ngoài ra còn có các FK phụ trợ để đảm bảo toàn vẹn dữ liệu: `halls.center_id`,
`contracts.customer_id`, `contracts.event_id`, `package_services.*`,
`booking_services.*`, `quotation_items.*`, `staff_assignments.event_id`,
`special_requests.event_id`, `incidents.event_id`.

**Lưu ý thiết kế:** `staff_assignments.staff_id` hiện tham chiếu tới `users.id`
(không có bảng `staff` riêng) vì danh sách 17 bảng không yêu cầu bảng này.
Nếu về sau cần phân biệt "nhân sự không có tài khoản đăng nhập" khỏi `users`,
có thể tách bảng `staff` riêng và cập nhật FK này.

FOREIGN KEY được enforce thật (không chỉ khai báo) nhờ `PRAGMA foreign_keys = ON`
chạy tự động trong `DatabaseManager::open()` — đã kiểm tra: insert vi phạm FK
bị SQLite từ chối với lỗi `FOREIGN KEY constraint failed`.