# PHẦN MỀM QUẢN LÝ SỰ KIỆN TRỐNG ĐỒNG PALACE (C++ OOP)

Hệ thống quản lý sự kiện và đặt lịch tiệc cưới / hội nghị cho chuỗi trung tâm **Trống Đồng Palace**, được xây dựng bằng C++17 theo mô hình lập trình hướng đối tượng (OOP), giao diện desktop Qt Widgets và lưu trữ dữ liệu bền vững bằng JSON (`nlohmann/json`).

---

## 1. Công nghệ & Thư viện sử dụng
- **Ngôn ngữ**: C++17.
- **Mô hình lập trình**: OOP (Đóng gói, Kế thừa, Đa hình, Trừu tượng hóa), kiến trúc phân tầng (Models, Storage, Services, UI, Utils).
- **Giao diện**: Qt 6 Widgets (bảng dữ liệu, biểu mẫu và điều hướng theo quyền).
- **Định dạng dữ liệu**: `nlohmann/json` v3.11.3.
- **Hệ thống build**: CMake 3.16+.
- **Bảo mật**: Mật khẩu mã hóa bằng Salt ngẫu nhiên + SHA-256 (không lưu plain-text khi xác thực).

---

## 2. Kiến trúc & Cấu trúc thư mục
```
trongdongpalace/
├── CMakeLists.txt              # Cấu hình biên dịch CMake (Qt 6 Widgets & nlohmann/json)
├── README.md                   # Tài liệu hướng dẫn sử dụng & báo cáo kỹ thuật
├── data/                       # Thư mục lưu trữ các file JSON thực tế
│   ├── users.json              # Tài khoản người dùng (4 vai trò đặc tả)
│   ├── customers.json          # Khách hàng & yêu cầu sự kiện (kèm dòng dịch vụ đã chốt giá)
│   ├── venues.json             # Sảnh VEN (giá thuê) & danh mục dịch vụ FOD/DEC/MED/AV/HR/EXT
│   ├── transactions.json       # Booking (giữ chỗ 72h) & lịch sử thanh toán
│   └── events.json             # Sự kiện vận hành (trạng thái tự tính theo thời gian)
├── backups/                    # Bản sao lưu dữ liệu tự động trước mỗi lần ghi JSON
├── src/
│   ├── main.cpp                # Nạp Storage và khởi chạy ứng dụng
│   ├── models/                 # Thực thể dữ liệu C++ OOP
│   ├── storage/                # Đọc/ghi JSON nguyên tử (atomic rename), backup & seed
│   ├── services/               # Xử lý business logic, RBAC, kiểm tra trùng lịch
│   ├── ui/                     # Giao diện Qt Widgets
│   └── utils/                  # DateTime, SHA-256, format tiền tệ VND
└── tests/
    └── test_core.cpp           # Bộ 10 nhóm bài kiểm thử tự động toàn diện (130 test cases)
```

---

## 3. Danh sách tài khoản mẫu & Thay đổi thông tin
Khi chạy lần đầu và thư mục `data/` chưa có dữ liệu, chương trình tự động tạo các tài khoản mẫu sau với mật khẩu mặc định là **`123456`**:

| Tên đăng nhập | Mật khẩu | Họ và tên | Vai trò | Quyền hạn chính |
| :--- | :--- | :--- | :--- | :--- |
| **`admin`** | `123456` | Quản trị viên hệ thống | **Quản lý** | Toàn quyền: sảnh, dịch vụ, booking, duyệt chiết khấu, tài khoản |
| **`kinhdoanh`** | `123456` | Nguyễn Văn Kinh Doanh | **Nhân viên kinh doanh** | Yêu cầu, booking (giữ chỗ 72h), ghi nhận thanh toán, tra cứu sảnh |
| **`vanhanh`** | `123456` | Lê Văn Vận Hành | **Vận hành** | Xem danh sách sự kiện (trạng thái tự tính theo thời gian) |
| **`khachhang`** | `123456` | Nguyễn Văn An | **Khách hàng** | Chỉ tạo yêu cầu sự kiện, xem yêu cầu đã gửi và đặt lịch của mình |

> **Cách thay đổi thông tin đăng nhập hoặc mật khẩu**:
> - Đăng nhập tài khoản `admin` -> Vào mục **"Người dùng (quản trị)"** -> Chọn tài khoản -> Nhập mật khẩu mới -> Nhấn **"Đổi mật khẩu"**.
> - Hoặc khách hàng tự bấm **"Đăng ký tài khoản khách hàng"** ngay tại màn hình đăng nhập ban đầu.

---

# Event Scheduling — Hệ thống Đặt lịch Sự kiện

Đồ án nhóm môn **Lập trình Hướng đối tượng (OOP)**: ứng dụng Qt Widgets quản lý
việc đặt/duyệt/hủy/đổi lịch sử dụng sảnh cho Trung tâm Tiệc cưới & Sự kiện.

## Cách build & chạy

CMake chỉ dùng để **biên dịch** chương trình. Máy người dùng cuối không cần
cài CMake nếu bạn gửi file thực thi đã build. Cần build riêng cho từng hệ điều
hành; file `.exe` của Windows không chạy trực tiếp trên Linux/macOS.

### Windows

Dùng **MSYS2 UCRT64** để có C++ compiler và CMake cùng một bộ công cụ:

1. Cài [MSYS2](https://www.msys2.org/), sau đó mở ứng dụng **MSYS2 UCRT64**.
2. Cài các công cụ cần thiết bằng lệnh:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-nlohmann-json
```

3. Trong cửa sổ MSYS2 UCRT64, chuyển đến thư mục dự án, tức thư mục có
  `CMakeLists.txt`, rồi chạy:

```bash
cmake -S . -B build/qt-ucrt64 -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="$MINGW_PREFIX/bin/g++.exe" \
  -DCMAKE_MAKE_PROGRAM="$MINGW_PREFIX/bin/mingw32-make.exe" \
  -DCMAKE_PREFIX_PATH="$MINGW_PREFIX"
cmake --build build/qt-ucrt64
./build/qt-ucrt64/trongdongpalace.exe
```

Qt 6 Widgets cũng cần được cài trên máy chạy ứng dụng. Khi đóng gói, dùng
`windeployqt` của MSYS2 UCRT64 để chuẩn bị các thư viện Qt đi kèm.

Dùng đúng thư mục `build/qt-ucrt64` như trên để không đụng cấu hình cũ
trong `build/`. CMake không cho đổi compiler hoặc generator trong cùng một
thư mục build; nếu muốn cấu hình lại, hãy chọn một thư mục build mới.

#### VS Code

Mở thư mục dự án bằng VS Code và chọn preset **Qt 6 Widgets (MSYS2 UCRT64)**
nếu được hỏi. `.vscode/settings.json` sẽ yêu cầu CMake Tools cấu hình preset
này và cung cấp compile flags cho IntelliSense, bao gồm Qt include paths.
Preset yêu cầu CMake 3.20 trở lên và MSYS2 được cài ở `C:/msys64`.

### Linux / macOS

Cài Qt 6 Widgets và CMake bằng trình quản lý gói của hệ điều hành, sau đó chạy:

```bash
cmake -S . -B build
cmake --build build --config Release
./build/trongdongpalace
```

### Chạy chương trình trên máy Windows khác

Máy nhận không cần cài CMake hay MSYS2. Trước khi gửi ứng dụng, chạy `windeployqt`
trong MSYS2 UCRT64 để chép các thư viện Qt và plugin cần thiết vào thư mục build:

```bash
windeployqt build/qt-ucrt64/trongdongpalace.exe
```

Gửi file `build/qt-ucrt64/trongdongpalace.exe` cùng các thư viện/plugin Qt
do `windeployqt` tạo ra; nếu muốn giữ dữ liệu hiện có thì gửi kèm thư mục `data/`.
Các thư viện GCC/C++ vẫn được liên kết tĩnh.

Mở PowerShell trong thư mục chứa file `.exe` rồi chạy:

```text
.\trongdongpalace.exe
```

Nếu không gửi `data/`, chương trình sẽ tạo dữ liệu mẫu khi khởi động. Dữ liệu
mới được lưu trong `data/` theo thư mục PowerShell hiện tại, nên hãy chạy lệnh
khi đang đứng trong thư mục phát hành.

Với Visual Studio / CLion: mở trực tiếp `CMakeLists.txt`, IDE tự đọc cấu hình;
hãy đặt `CMAKE_PREFIX_PATH` trỏ tới thư mục cài Qt 6 nếu IDE không tự tìm thấy Qt.

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
> `build/trongdongpalace.exe` từ trong `build/` thì dữ liệu nằm ở `build/data/`.

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
