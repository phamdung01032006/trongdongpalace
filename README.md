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
- Trình biên dịch hỗ trợ C++17 (g++, clang++, MSVC)
- SQLite3 dev library:
  - Windows: cài qua msys2 mingw64 (`pacman -S mingw-w64-x86_64-sqlite3`) rồi trỏ CMake tới toolchain file của vcpkg

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
```

Chương trình sẽ tạo ra file `trong_dong_palace.db` tại thư mục chạy chương trình
(thường là thư mục `build/`). Chạy lại chương trình nhiều lần không gây lỗi vì
SQLite tự tạo file nếu chưa tồn tại và mở lại nếu đã có.

## DatabaseManager

`src/database/DatabaseManager.h/.cpp` bọc một kết nối SQLite duy nhất với 3 hàm chính:

```text
open()     — mở kết nối tới file .db, ném std::runtime_error nếu thất bại
close()    — đóng kết nối, an toàn khi gọi nhiều lần
execute()  — chạy câu SQL không trả về dòng dữ liệu (CREATE TABLE, INSERT, ...)
```

Repository ở các bước sau sẽ dùng `DatabaseManager` này để đọc/ghi dữ liệu;
Service và Domain object không được truy cập SQLite trực tiếp.
