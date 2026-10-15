# PHẦN MỀM QUẢN LÝ SỰ KIỆN TRỐNG ĐỒNG PALACE (C++ OOP)

Hệ thống quản lý sự kiện cho trung tâm **Trống Đồng Palace**, xây dựng bằng C++17
theo mô hình hướng đối tượng (OOP), giao diện desktop Qt Widgets và lưu trữ bền
vững bằng JSON (`nlohmann/json`). Luồng nghiệp vụ bám đặc tả: **Yêu cầu →
Booking (giữ chỗ 72h) → Thanh toán (đặt cọc) → Sự kiện** (trạng thái tự tính
theo thời gian).

---

## 1. Công nghệ & Thư viện sử dụng
- **Ngôn ngữ**: C++17.
- **Mô hình**: OOP, kiến trúc phân tầng (Models, Storage, Services, UI, Utils).
- **Giao diện**: Qt 6 Widgets (bảng dữ liệu, dropdown, date-picker, điều hướng theo vai trò).
- **Dữ liệu**: `nlohmann/json` v3.11.3, ghi nguyên tử (atomic rename) + backup tự động.
- **Build**: CMake 3.16+.
- **Bảo mật**: mật khẩu Salt ngẫu nhiên + SHA-256 (không lưu plain-text).

---

## 2. Kiến trúc & Cấu trúc thư mục
```
trongdongpalace/
├── CMakeLists.txt              # CMake (Qt 6 Widgets & nlohmann/json)
├── README.md                   # Tài liệu này
├── data/                       # JSON runtime (tự tạo khi chạy, không commit)
│   ├── users.json              # Tài khoản (4 vai trò đặc tả)
│   ├── customers.json          # Khách hàng & yêu cầu (kèm dòng dịch vụ đã chốt giá)
│   ├── venues.json             # Sảnh VEN (giá thuê) & dịch vụ FOD/DEC/MED/AV/HR/EXT
│   ├── transactions.json       # Booking (giữ chỗ 72h) & thanh toán
│   └── events.json             # Sự kiện (trạng thái tự tính theo thời gian)
├── backups/                    # Backup tự động trước mỗi lần ghi
├── src/
│   ├── main.cpp                # Nạp Storage, chạy Qt UI
│   ├── models/                 # User, Customer, EventRequest, Hall, ServiceItem,
│   │                           # Booking, Payment, Event, Enums, Database
│   ├── storage/                # JSON load/save + seed danh mục đặc tả
│   ├── services/               # PalaceService: nghiệp vụ, RBAC, trùng lịch, hết hạn giữ chỗ
│   ├── ui/                     # Qt Widgets theo actor
│   └── utils/                  # DateTime, SHA-256, tiền tệ VND
└── tests/
    └── test_core.cpp           # 14 nhóm kiểm thử (154 checks, xem mục 6)
```

---

## 3. Tài khoản mẫu
Lần chạy đầu (`data/` trống) chương trình tự seed, mật khẩu mặc định **`123456`**:

| Tên đăng nhập | Họ và tên | Vai trò | Quyền hạn chính |
| :--- | :--- | :--- | :--- |
| **`admin`** | Quản trị viên hệ thống | **Quản lý** | Toàn quyền: sảnh, dịch vụ, booking, duyệt chiết khấu >10%, tài khoản, nạp danh mục mẫu |
| **`kinhdoanh`** | Nguyễn Văn Kinh Doanh | **Nhân viên kinh doanh** | Yêu cầu (Chờ→Tư vấn→Hoàn thành→Booking), booking giữ chỗ 72h, thu tiền, tra cứu sảnh |
| **`vanhanh`** | Lê Văn Vận Hành | **Vận hành** | Xem sự kiện (Sắp/Đang diễn ra/Đã kết thúc tự tính) |
| **`khachhang`** | Nguyễn Văn An | **Khách hàng** | Chỉ tạo yêu cầu, xem yêu cầu đã gửi + đặt lịch của mình, tự hủy yêu cầu |

> Đổi mật khẩu: đăng nhập `admin` → **Người dùng** → bấm vào ô Hoạt động/Vai trò để đổi tại chỗ, nút **Cập nhật** để đổi mật khẩu.
> Khách hàng mới tự bấm **"Đăng ký tài khoản khách hàng"** ở màn hình đăng nhập
> (nhập đủ họ tên/SĐT/email/địa chỉ) — hồ sơ khách hàng tự tạo và gắn vào tài khoản.

---

## 4. Cách build & chạy

### Windows (MSYS2 UCRT64)
```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-nlohmann-json
cmake -S . -B build/qt-ucrt64 -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="$MINGW_PREFIX/bin/g++.exe" \
  -DCMAKE_MAKE_PROGRAM="$MINGW_PREFIX/bin/mingw32-make.exe" \
  -DCMAKE_PREFIX_PATH="$MINGW_PREFIX"
cmake --build build/qt-ucrt64
./build/qt-ucrt64/trongdongpalace.exe
./build/qt-ucrt64/tdp_tests.exe   # kiểm thử lõi
```
Tắt app đang mở trước khi build lại (Windows khóa file `.exe` đang chạy).
Dùng đúng thư mục `build/qt-ucrt64`; không đổi compiler trong cùng thư mục build.

### Đóng gói cho máy khác
```bash
windeployqt build/qt-ucrt64/trongdongpalace.exe
```
Gửi `.exe` + thư viện Qt do `windeployqt` tạo ra (+ thư mục `data/` nếu muốn giữ
dữ liệu). Không có `data/` thì app tự seed khi khởi động.

### VS Code
Mở thư mục dự án, chọn preset **Qt 6 Widgets (MSYS2 UCRT64)**. Yêu cầu CMake 3.20+
và MSYS2 ở `C:/msys64`.

---

## 5. Chức năng theo vai trò (bám đặc tả)

| Vai trò | Màn hình & thao tác chính |
| :--- | :--- |
| Khách hàng | **Yêu cầu đã gửi** (cột Chi phí dự kiến = tiền sảnh + dịch vụ; trạng thái 2 lớp: Đang xét duyệt/Hoàn thành/Bị hủy), **Tạo yêu cầu** (dropdown loại sự kiện/sảnh, date-picker, tích dịch vụ theo tab, số khách 10–800), **Đặt lịch của tôi** (double-click xem phiếu thu), tự hủy yêu cầu (bấm ô trạng thái) |
| Kinh doanh | Yêu cầu (sửa toàn bộ sau trao đổi, đổi trạng thái tại ô, Hoàn thành → tự sang Booking, xem số yêu cầu mới), Booking (tạo từ yêu cầu, giảm giá, Hủy/Hoàn tất tại ô), Thanh toán (ghi nhận/sửa/xóa khoản chưa thu, phiếu thu), tra cứu sảnh trống đa tiêu chí, xem sảnh/dịch vụ/khách hàng |
| Quản lý | Toàn bộ quyền kinh doanh + thêm/sửa/xóa sảnh (mã tự động, trạng thái Khả dụng/Bảo trì/Không khả dụng tại ô), thêm/sửa/xóa + bật/tắt dịch vụ theo 7 tab, **duyệt chiết khấu >10%** (bấm ô Giảm giá), quản lý tài khoản, **Nạp danh mục mẫu** |
| Vận hành | Xem danh sách sự kiện, trạng thái Sắp/Đang diễn ra/Đã kết thúc tự tính theo giờ hệ thống |

Quy tắc lõi: Booking mới ở **Đang chờ đặt cọc** (giữ sảnh 72h); thu cọc xong →
**Đã đặt cọc** + tự sinh sự kiện; quá 72h không cọc → tự hủy + nhả sảnh; sảnh
trùng lịch ở trạng thái giữ chỗ/đã cọc thì chặn lưu; Tất toán → booking Hoàn tất.

---

## 6. Kiểm thử tự động (`tests/test_core.cpp`, 14 nhóm / 154 checks)

| # | Nhóm | Điểm chính |
|---|------|------------|
| 1 | Lưu/đọc JSON | Roundtrip đầy đủ luồng, đủ 5 file |
| 2 | Seed danh mục | 100 dịch vụ, 6 sảnh VEN, ánh xạ trạng thái 2 lớp |
| 3 | Chi phí dự kiến | 20 bàn FOD01-P = 90M, chặn số khách <10 và ngày quá khứ |
| 4 | Trùng lịch + hết hạn 72h | Chặn trùng, sweep hủy booking quá hạn, nhả sảnh |
| 5 | Chiết khấu | >10% cần Quản lý duyệt, sales không tự duyệt được |
| 6 | Thanh toán | Ghi chú bắt buộc, cấm ngày tương lai, phiếu thu, Tất toán → Hoàn tất |
| 7 | Yêu cầu → Booking | Hoàn thành tự sang Booking, lịch sử gọn 10 |
| 8 | Phân quyền | Đủ 4 actor, sai mật khẩu |
| 9 | Khách hàng | Tự điền hồ sơ, tự hủy, đăng ký sinh hồ sơ + đồng bộ ngược SĐT |
| 10 | Sảnh + sự kiện | Sảnh Bảo trì bị loại, trạng thái sự kiện theo thời gian |
| 11 | JSON hỏng | Ném lỗi, giữ nguyên file |
| 12 | Nạp danh mục | Không trùng, sales bị chặn |
| 13 | Xóa an toàn | Chặn xóa khi còn liên kết, cascade booking nháp, giữ quản lý cuối |
| 14 | Sự kiện theo cọc + lọc | Sự kiện chỉ sinh khi thu cọc, sửa thanh toán, lọc sảnh theo giá/sức chứa |

---

## 7. Lưu trữ dữ liệu

- `data/` nằm theo **thư mục làm việc** khi chạy exe; mỗi lần ghi đều backup
  timestamp vào `backups/` trước (ghi nguyên tử qua file `.tmp`).
- Đơn giá dịch vụ **snapshot tại lúc chọn** vào yêu cầu nên sửa catalog không làm
  lệch đơn cũ; đọc file cũ thiếu trường thì dùng giá trị mặc định tương thích.
- Mã tự động tăng (`REQ-`, `BKG-`, `PAY-`, `EVT-`, `HAL-`, `SVC-`, `CUS-`, `USR-`).

---

## 8. Ghi chú nhóm

- `build/`, `data/*.tmp`, `backups/` đã ignores — không commit.
- Muốn cấu hình lại compiler/generator thì dùng thư mục build mới.
- Khi thêm actor/màn hình mới: thêm `Action` + nhánh trong `navigationEntries()`,
  `tableHeaders()`, `getRows()`, `addActions()` ở `src/ui/app_ui.cpp`.
