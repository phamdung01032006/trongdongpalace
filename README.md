# Event Scheduling — Hệ thống Đặt lịch Sự kiện

Đồ án nhóm môn **Lập trình Hướng đối tượng (OOP)**: phần mềm console quản lý
việc đặt/duyệt/hủy/đổi lịch sử dụng sảnh cho Trung tâm Tiệc cưới & Sự kiện.

## Cách build & chạy

CMake chỉ dùng để **biên dịch** chương trình. Máy người dùng cuối không cần
cài CMake nếu bạn gửi file thực thi đã build. Cần build riêng cho từng hệ điều
hành; file `.exe` của Windows không chạy trực tiếp trên Linux/macOS.

### Windows (PowerShell + MinGW)

Cài [CMake](https://cmake.org/download/) và MinGW-w64 (khuyến nghị MSYS2
UCRT64), sau đó mở PowerShell tại thư mục có `CMakeLists.txt`:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
.\build\event_scheduling.exe
```

Nếu PowerShell báo không tìm thấy `cmake` hoặc `g++`, hãy thêm thư mục `bin`
của CMake và MinGW vào biến môi trường `PATH`, rồi mở lại terminal.

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build --config Release
./build/event_scheduling
```

### Gửi cho máy Windows khác không cài CMake

Sau khi build, chỉ cần gửi thư mục chứa:

```text
event_scheduling.exe
data/                 (nếu muốn giữ dữ liệu đã có)
```

Bản MinGW trong cấu hình này liên kết tĩnh thư viện GCC/C++, nên không cần gửi
thêm `libstdc++-6.dll` hay `libgcc_s_seh-1.dll`. Người dùng chạy bằng cách mở
PowerShell/cửa sổ lệnh trong thư mục đó và dùng:

```powershell
.\event_scheduling.exe
```

Nếu không gửi `data/`, chương trình sẽ tự tạo dữ liệu mẫu khi khởi động. Dữ
liệu mới được lưu trong `data/` tương đối với **thư mục hiện hành**, vì vậy nên
chạy lệnh từ thư mục phát hành chứa file `.exe`.

Với Visual Studio / CLion: mở trực tiếp `CMakeLists.txt`, IDE tự đọc cấu hình.

## Cấu trúc thư mục

```
trongdongpalace/
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
├── data/               # File dữ liệu txt sinh ra khi chạy (không commit)
├── build/              # Thư mục build CMake (không commit)
├── CMakeLists.txt
└── README.md
```

## Lưu trữ dữ liệu

Toàn bộ dữ liệu (sảnh + booking) được **tự động lưu ra file txt** trong thư mục
`data/` nằm cạnh chương trình chạy, nên dữ liệu **không bị mất khi tắt rồi chạy lại**.

> Đường dẫn `data/` là **tương đối theo thư mục làm việc**. Nếu chạy
> `build/event_scheduling.exe` từ trong `build/` thì dữ liệu nằm ở `build/data/`.

| File | Nội dung | Định dạng mỗi dòng |
|------|----------|--------------------|
| `data/sanh.txt` | Danh mục sảnh | `maSanh\|ten\|sucChua\|dangHoatDong(0/1)` |
| `data/booking.txt` | Danh sách booking | `maBooking\|maSanh\|loaiSuKien\|maSuKien\|tenSuKien\|soKhach\|gioBatDau\|gioKetThuc\|trangThai\|lyDoTuChoi\|ngayTao` |

**Đầu mỗi file đều có header** giúp người đọc hiểu cấu trúc: các dòng chú thích
bắt đầu bằng `#` (giải thích ý nghĩa từng cột, các quy ước giá trị) và một dòng
tên cột (ví dụ `MaSanh|TenSanh|SucChua|DangHoatDong`). Khi nạp dữ liệu, các dòng
này được **tự động bỏ qua**, nên file cũ (không có header) vẫn đọc bình thường.
Tất cả dữ liệu — từ dữ liệu mẫu cho đến dữ liệu người dùng tạo sau này — đều
được ghi vào file txt theo cùng một định dạng.

Quy ước giá trị:

- `loaiSuKien`: `1` = Tiệc cưới, `2` = Hội nghị, `3` = Sinh nhật
  (lấy qua hàm ảo `Event::maLoai()` — thể hiện tính đa hình).
- `trangThai`: `0` = Pending, `1` = Approved, `2` = Rejected, `3` = Cancelled.
- Thời điểm ghi ở dạng `dd/mm/yyyy hh:mm`.

Cơ chế hoạt động:

- **Khi khởi động**: `main()` gọi `ht.docDuLieuTuFile()` để nạp dữ liệu đã lưu;
  chỉ khi hệ thống còn rỗng mới sinh dữ liệu mẫu (`khoiTaoDuLieuMau()`).
- **Khi có thay đổi**: mỗi thao tác thêm/sửa/xóa sảnh và tạo/duyệt/từ chối/hủy/đổi
  lịch đều gọi `ghiDuLieuRaFile()` để lưu ngay (chống mất dữ liệu khi tắt đột ngột);
  destructor của `HeThongDatLich` cũng lưu lần cuối.
- **Bộ đếm mã**: sau khi nạp, `capNhatSoThuTu()` quét mã `SK###`/`BK###` lớn nhất
  để các mã sinh ra tiếp theo **không bị trùng** với dữ liệu cũ.
- **Dữ liệu hỏng**: từng dòng sai định dạng được bỏ qua kèm cảnh báo, không làm chết
  chương trình; ký tự phân cách `|` do người dùng nhập được thay bằng khoảng trắng.

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
