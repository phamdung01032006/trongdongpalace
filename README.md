# Event Scheduling — Hệ thống Đặt lịch Sự kiện

Đồ án nhóm môn **Lập trình Hướng đối tượng (OOP)**: phần mềm console quản lý
việc đặt/duyệt/hủy/đổi lịch sử dụng sảnh cho Trung tâm Tiệc cưới & Sự kiện.

## Cách build & chạy

Yêu cầu: **CMake >= 3.10** và bộ biên dịch **C++17** (g++ / MinGW / MSVC).

```bash
cd EventScheduling
mkdir build && cd build
cmake ..
cmake --build .
./event_scheduling        # Linux/Mac
event_scheduling.exe      # Windows
```

Với Visual Studio / CLion: mở trực tiếp `CMakeLists.txt`, IDE tự đọc cấu hình.

## Cấu trúc thư mục

```
EventScheduling/
├── include/            # Các file khai báo lớp (.h)
│   ├── Event.h             # Lớp TRỪU TƯỢNG cha của mọi loại sự kiện
│   ├── WeddingEvent.h      # Lớp con Tiệc cưới (kế thừa Event)
│   ├── ConferenceEvent.h   # Lớp con Hội nghị (kế thừa Event)
│   ├── BirthdayEvent.h     # Lớp con Sinh nhật (kế thừa Event)
│   ├── Hall.h              # Lớp sảnh
│   ├── BookingRequest.h    # Lớp yêu cầu đặt lịch (has-a Hall*, has-a Event*)
│   ├── HeThongDatLich.h    # Lớp trung tâm điều phối nghiệp vụ
│   ├── Exceptions.h        # Các lớp ngoại lệ kế thừa std::exception
│   └── Utils.h             # ThoiDiem + tiện ích nhập liệu, in bảng, màn hình
├── src/                # Cài đặt (.cpp) tương ứng từng header
│   └── main.cpp            # Menu console, bắt exception hiển thị (FR-11)
├── docs/               # Tài liệu FR
├── build/              # Thư mục build CMake (không commit)
├── CMakeLists.txt
└── README.md
```

## Chức năng (theo FR)

| FR  | Chức năng | Menu |
|-----|-----------|------|
| FR-01 | Quản lý danh mục sảnh (thêm/sửa/xóa/xem) | Quản lý sảnh → 1 |
| FR-02 | 3 loại sự kiện (Cưới/Hội nghị/Sinh nhật) | Chọn khi tạo booking |
| FR-03 | Xem lịch trống của sảnh theo ngày | Người yêu cầu → 2 |
| FR-04 | Tạo yêu cầu đặt lịch (trạng thái Pending) | Người yêu cầu → 3 |
| FR-05 | Kiểm tra trùng lịch (khung giờ mở rộng) | Tự động khi tạo/duyệt/đổi lịch |
| FR-06 | Duyệt yêu cầu | Quản lý sảnh → 2 |
| FR-07 | Từ chối yêu cầu | Quản lý sảnh → 3 |
| FR-08 | Hủy lịch đã duyệt (hạn hủy 48 giờ) | Người yêu cầu → 4 |
| FR-09 | Đổi lịch (giữ nguyên mã booking) | Người yêu cầu → 5 |
| FR-10 | Tra cứu theo trạng thái/sảnh/khoảng ngày | Cả hai → mục tra cứu |
| FR-11 | Xử lý ngoại lệ toàn hệ thống | Bọc try/catch ở mọi thao tác |

## Áp dụng nguyên lý OOP (bảng ánh xạ Mục 9)

- **Thiết kế lớp & đối tượng**: `Event`, `Hall`, `BookingRequest`, `HeThongDatLich`.
- **Kế thừa**: `WeddingEvent`/`ConferenceEvent`/`BirthdayEvent` kế thừa `Event`;
  các exception kế thừa `std::exception` (qua `BaseException`).
- **Đa hình**: khi tạo/duyệt booking, hệ thống chỉ giữ `Event*` và gọi
  `thoiGianChuanBi()`, `thoiGianDonDep()`, `kiemTraDieuKienRieng()` —
  phiên bản của lớp con tương ứng được chạy lúc runtime.
- **Xử lý ngoại lệ**: 8 lớp exception tùy chỉnh theo Mục 7, mọi thao tác
  nhập liệu đều được bọc `try/catch`.

## Ghi chú nhóm

- Khi thêm lớp sự kiện mới: tạo `.h`/`.cpp`, thêm tên file `.cpp` vào
  `add_executable(...)` trong `CMakeLists.txt`.
- `build/` đã nằm trong `.gitignore` — không commit file build lên Git.
