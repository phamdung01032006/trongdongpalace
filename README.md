# Trống Đồng Palace — Hệ thống đặt lịch sự kiện

> Đồ án C++17 · Giao diện console · Lưu dữ liệu bằng file text · Không dùng thư viện ngoài

Chương trình mô phỏng quy trình đặt lịch của một trung tâm tiệc cưới & sự kiện, đi từ
**yêu cầu của khách hàng** đến **booking giữ chỗ**, **thanh toán** và **sự kiện diễn ra**.
Toàn bộ nghiệp vụ nằm trong lớp `PalaceService`; giao diện console chỉ nhập/xuất và gọi
service, không tự sửa dữ liệu.

```text
Yêu cầu sự kiện → Tư vấn / Hoàn thành → Booking giữ chỗ 72 giờ
                → Đặt cọc → Tất toán → Sự kiện
```

---

## Mục lục

1. [Dự án làm gì?](#1-dự-án-làm-gì)
2. [Tính năng chính](#2-tính-năng-chính)
3. [Công nghệ và yêu cầu](#3-công-nghệ-và-yêu-cầu)
4. [Bắt đầu nhanh](#4-bắt-đầu-nhanh)
5. [Tài khoản mẫu](#5-tài-khoản-mẫu)
6. [Hướng dẫn sử dụng theo vai trò](#6-hướng-dẫn-sử-dụng-theo-vai-trò)
7. [Quy tắc nghiệp vụ](#7-quy-tắc-nghiệp-vụ)
8. [Dữ liệu được lưu ở đâu?](#8-dữ-liệu-được-lưu-ở-đâu)
9. [Cấu trúc dự án](#9-cấu-trúc-dự-án)
10. [Kiểm thử](#10-kiểm-thử)
11. [Cấu hình môi trường](#11-cấu-hình-môi-trường)
12. [Xử lý sự cố](#12-xử-lý-sự-cố)
13. [Tài liệu chi tiết](#13-tài-liệu-chi-tiết)
14. [Điểm nhấn OOP (gợi ý bảo vệ)](#14-điểm-nhấn-oop-gợi-ý-bảo-vệ)
15. [Giới hạn và hướng phát triển](#15-giới-hạn-và-hướng-phát-triển)

---

## 1. Dự án làm gì?

Hệ thống có **4 vai trò**:

| Vai trò | Viết tắt trong code | Việc chính |
| :--- | :--- | :--- |
| Khách hàng | `Role::Customer` | Tự đăng ký, tạo/hủy yêu cầu của mình, xem booking & thanh toán, tra cứu sảnh trống |
| Nhân viên kinh doanh | `Role::Sales` | Quản lý yêu cầu, booking, thanh toán, khách hàng; tra cứu sảnh; xem danh mục |
| Quản lý | `Role::Manager` | Toàn quyền, thêm quản lý sảnh/dịch vụ/tài khoản, xem danh sách sự kiện |
| Vận hành | `Role::Operation` | Xem danh sách sự kiện và trạng thái theo thời gian thực |

Luồng nghiệp vụ cốt lõi:

1. Khách gửi **yêu cầu sự kiện** (loại sự kiện, sảnh mong muốn, thời gian, số khách, dịch vụ).
2. Kinh doanh tư vấn, chỉnh sửa và chuyển yêu cầu sang **Hoàn thành**.
3. Hệ thống tự kiểm tra sảnh, tạo **Booking** trạng thái **Chờ cọc** và giữ chỗ **72 giờ**.
4. Khi khách đặt cọc, booking chuyển **Đã cọc**, hệ thống tự tạo **Sự kiện**.
5. Khi tất toán đủ, booking chuyển **Hoàn tất**; sự kiện hiển thị theo thời gian thực.

---

## 2. Tính năng chính

- **Đăng nhập / phân quyền** theo 4 vai trò; tài khoản có thể bị khóa.
- **Đăng ký khách hàng**: nhập đầy đủ họ tên, SĐT, email, địa chỉ; hệ thống tự tạo hồ sơ
  `Customer` và gắn với tài khoản.
- **Quản lý yêu cầu sự kiện**: tạo, sửa, đổi trạng thái, xóa, xem lịch sử 10 yêu cầu gần nhất.
- **Quản lý booking**: tạo từ yêu cầu, hủy, xóa booking chưa cọc; kiểm tra trùng lịch sảnh.
- **Quản lý thanh toán**: ghi nhận đặt cọc 1/2, tất toán, sửa khoản thu, xóa khoản chưa thu,
  in phiếu thu.
- **Tra cứu sảnh trống** theo khoảng thời gian, số khách và giá thuê tối đa.
- **Quản lý danh mục**: sảnh (sức chứa, giá thuê, trạng thái) và dịch vụ (nhóm, đơn vị,
  đơn giá, bật/tắt).
- **Quản lý tài khoản**: tạo tài khoản nhân viên, đổi vai trò, khóa/mở, đổi mật khẩu.
- **Danh sách sự kiện** cho cả Quản lý và Vận hành; trạng thái tự tính theo đồng hồ máy.
- **Tự động hóa**: hết 72 giờ chưa cọc thì hủy booking và trả yêu cầu về Hoàn thành;
  đặt cọc thì tự tạo sự kiện; trạng thái sự kiện tự cập nhật.

> **Không còn các chức năng:** nạp danh mục mẫu thủ công và quản lý/duyệt chiết khấu.

---

## 3. Công nghệ và yêu cầu

| Thành phần | Lựa chọn |
| :--- | :--- |
| Ngôn ngữ | C++17 |
| Giao diện | Console (menu nhập số, bảng text) |
| Lưu trữ | File `.txt` UTF-8, mỗi dòng một bản ghi, phân tách bằng `\|` |
| Build | CMake 3.16+ (khuyến nghị) hoặc g++ trực tiếp |
| Thư viện ngoài | Không có |
| Nền tảng | Windows, Linux, macOS |

**Cần cài:**

- Trình biên dịch hỗ trợ C++17 (g++ ≥ 9, clang++ ≥ 10, MSVC 2019+).
- CMake ≥ 3.16.
- (Khuyến nghị trên Windows) MSYS2 UCRT64 với `g++`, `cmake`, `mingw32-make`.

Hướng dẫn cài đặt chi tiết: [docs/SETUP.md](docs/SETUP.md).

---

## 4. Bắt đầu nhanh

### 4.1. Windows — MSYS2 UCRT64 (khuyến nghị)

Mở **MSYS2 UCRT64** và cài gói:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
                   mingw-w64-ucrt-x86_64-cmake \
                   mingw-w64-ucrt-x86_64-make
```

Tại thư mục gốc dự án, trong cửa sổ MSYS2 UCRT64:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/trongdongpalace.exe
```

Nếu dùng đúng bộ công cụ UCRT64 nhưng CMake không tự tìm thấy trình biên dịch:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe \
      -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe \
      -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
cmake --build build
```

Sau khi build, có thể chạy bằng PowerShell từ thư mục gốc:

```powershell
.\build\trongdongpalace.exe
```

### 4.2. Linux / macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/trongdongpalace
```

### 4.3. Windows — MSVC / Visual Studio

```powershell
cmake -S . -B build-msvc
cmake --build build-msvc --config Release
.\build-msvc\Release\trongdongpalace.exe
```

`CMakeLists.txt` tự thêm `/utf-8` cho MSVC để đọc đúng tiếng Việt trong source.

### 4.4. Build nhanh bằng g++ (không cần CMake)

Linux/macOS:

```bash
g++ -std=c++17 -O2 -Iinclude main.cpp src/*.cpp -o trongdongpalace
```

Windows PowerShell:

```powershell
$src = Get-ChildItem src -Filter *.cpp | ForEach-Object { $_.FullName }
g++ -std=c++17 -O2 -Iinclude main.cpp @src -o trongdongpalace.exe
```

### 4.5. Chạy và thoát

Chạy `trongdongpalace.exe` từ **thư mục gốc dự án** hoặc từ thư mục `build/`.
Chương trình tự tìm `data/` cạnh thư mục đang chạy; nếu không có sẽ thử `../data`.

- Menu đăng nhập: chọn **0** để thoát.
- Menu vai trò: chọn **0** để đăng xuất.
- Trong màn hình nhập liệu: gõ **`b`** để quay lại, **`m`** để về menu chính.

---

## 5. Tài khoản mẫu

Mật khẩu chung: **`123456`**.

| Tên đăng nhập | Vai trò | Quyền chính |
| :--- | :--- | :--- |
| `admin` | Quản lý | Toàn quyền; quản lý sảnh/dịch vụ/tài khoản; xem sự kiện |
| `kinhdoanh` | Nhân viên kinh doanh | Yêu cầu, booking, thanh toán, khách hàng, tra cứu sảnh, xem danh mục |
| `vanhanh` | Vận hành | Xem danh sách sự kiện |
| `khachhang` | Khách hàng | Tạo/hủy yêu cầu của mình, xem booking & thanh toán, tra cứu sảnh |

Khách mới có thể tự đăng ký ở màn hình đăng nhập (mục **2**). Hệ thống tự tạo hồ sơ
`Customer` với mã `CUS-xxxx` và gắn vào tài khoản.

> **Lưu ý bảo mật:** mật khẩu đang lưu **plain text** trong `data/users.txt` để phục vụ
> đồ án. Hệ thống thật phải băm mật khẩu (hash + salt).

---

## 6. Hướng dẫn sử dụng theo vai trò

### Khách hàng

1. **Tạo yêu cầu sự kiện**: chọn loại sự kiện, nhập tên sự kiện, thời gian, số khách,
   chọn sảnh (có thể để trống để trung tâm tư vấn), chọn dịch vụ. Bảng dịch vụ đang
   cung cấp hiện ngay trước khi nhập mã; gõ `?` để xem lại bảng.
2. **Yêu cầu của tôi**: xem trạng thái (Chờ / Đang tư vấn / Hoàn thành / Đã chốt / Bị hủy).
3. **Đặt lịch của tôi**: xem booking và các khoản thanh toán.
4. **Tra cứu sảnh trống**: nhập thời gian, số khách, giá tối đa.
5. **Hủy yêu cầu của tôi** khi yêu cầu chưa chốt.

### Nhân viên kinh doanh

- **Quản lý yêu cầu**: xem tất cả, tạo, sửa, đổi trạng thái, xóa, xem 10 yêu cầu gần nhất.
- **Quản lý booking**: xem, tạo từ yêu cầu, hủy, xóa booking chưa cọc.
- **Quản lý thanh toán**: xem khoản thu theo booking, ghi nhận, sửa, xóa khoản chưa thu,
  in phiếu thu.
- **Danh sách khách hàng**, **tra cứu sảnh**, **danh mục sảnh & dịch vụ**.

### Quản lý

Toàn bộ quyền của kinh doanh, cộng thêm:

- **Quản lý sảnh**: thêm, sửa, xóa, đổi trạng thái (Khả dụng / Bảo trì / Không khả dụng).
- **Quản lý dịch vụ**: thêm, sửa, xóa, bật/tắt.
- **Quản lý tài khoản**: tạo tài khoản nhân viên, đổi vai trò, khóa/mở, đổi mật khẩu, xóa.
- **Danh sách sự kiện** (mục 10).

### Vận hành

- **Danh sách sự kiện**: xem mã sự kiện, booking, khách hàng, sảnh, thời gian, trạng thái.
- Trạng thái **Sắp diễn ra / Đang diễn ra / Đã kết thúc** được tính tự động theo giờ máy.

---

## 7. Quy tắc nghiệp vụ

### 7.1. Vòng đời yêu cầu sự kiện

```text
Chờ ──→ Đang tư vấn
 │           │
 ├───────────┴──→ Hoàn thành ──(tự tạo Booking)──→ Đã chốt
 │
 └──→ Bị hủy

Đã chốt ──(xóa booking chưa cọc / hết hạn giữ chỗ)──→ Hoàn thành
```

- Tên sự kiện, họ tên khách, số điện thoại hợp lệ là bắt buộc.
- Thời gian: `start < end` và không được ở quá khứ (cho phép trễ 1 giờ).
- Số khách: từ **10** đến sức chứa sảnh lớn nhất.
- Chuyển sang **Hoàn thành** sẽ tự tạo booking; trạng thái lưu của yêu cầu là **Đã chốt**.
- Nếu yêu cầu **chưa chọn sảnh** (hoặc sảnh đã chọn không còn tồn tại), hệ thống tự chọn
  sảnh trống đầu tiên đủ sức chứa; nếu không còn sảnh, báo lỗi và **không** đổi trạng thái.
- Nếu sảnh đã chọn bị trùng lịch, hệ thống **báo lỗi** và không tự đổi sang sảnh khác;
  cần sửa yêu cầu hoặc tạo booking với sảnh khác.
- Yêu cầu **Đã chốt** hoặc **Bị hủy** là trạng thái kết thúc, không đổi tiếp — ngoại lệ:
  **Đã chốt** quay về **Hoàn thành** nếu booking chưa cọc bị xóa hoặc hết hạn giữ chỗ.

### 7.2. Booking và giữ chỗ 72 giờ

```text
Chờ cọc ──(thu đặt cọc)──→ Đã cọc ──(tất toán)──→ Hoàn tất
    │
    └──(quá 72 giờ chưa cọc)──→ Đã hủy
```

- Booking mới: **Chờ cọc**, hạn giữ chỗ = lúc tạo + **72 giờ** (`PalaceService::kHoldHours`).
- Booking **Chờ cọc / Đã cọc / Hoàn tất** đều chiếm lịch sảnh. Chỉ **Đã hủy** giải phóng sảnh.
- Mỗi lần vào menu, `refreshSystem()` quét booking quá hạn chưa cọc:
  hủy booking, trả yêu cầu về **Hoàn thành**, hủy sự kiện liên quan (nếu có).
- Chỉ được **xóa** booking **Chờ cọc** (hoặc booking đã hủy) và chưa có khoản thu đã
  thanh toán. Booking đã cọc/hoàn tất phải dùng **Hủy** để giữ lịch sử.
- Hủy booking: đặt **Đã hủy**, lưu lý do, hủy sự kiện tương ứng, giải phóng sảnh.

### 7.3. Kiểm tra trùng lịch sảnh

Hai khoảng `[start, end)` giao nhau khi:

```text
aStart < bEnd  và  bStart < aEnd
```

Sảnh còn trống khi:

- Sảnh tồn tại và đang **Khả dụng**.
- `start < end`.
- Không có booking đang chiếm sảnh bị giao thời gian.

### 7.4. Thanh toán

- Loại khoản thu: **Đặt cọc lần 1**, **Đặt cọc lần 2**, **Tất toán**.
- Hình thức: Tiền mặt, Chuyển khoản, Thẻ tín dụng.
- Số tiền phải **> 0**, ghi chú bắt buộc, ngày thu không được ở tương lai.
- Không thu vượt số còn lại.
- **Tất toán phải thu đủ** số còn lại, không chấp nhận tất toán một phần.
- Khi tạo booking, hệ thống tự tạo sẵn khoản **Đặt cọc lần 1** (số tiền 0, chưa thanh toán)
  để nhân viên chỉ việc điền số tiền và ghi chú.
- Thu đặt cọc (lần 1 hoặc lần 2 khi booking còn **Chờ cọc**) → booking **Đã cọc** và tự
  tạo **Sự kiện**.
- Tất toán đủ → booking **Hoàn tất**.
- Tổng tiền booking = giá thuê sảnh + tổng tiền dịch vụ (theo đơn giá đã chốt); **không có
  chiết khấu**.

### 7.5. Sự kiện

- Sự kiện sinh ra tự động khi booking được thu tiền lần đầu (đặt cọc hoặc tất toán).
- Trạng thái tính theo giờ máy:
  - `now < start`: **Sắp diễn ra**
  - `start ≤ now ≤ end`: **Đang diễn ra**
  - `end < now`: **Đã kết thúc**
- Khi booking bị hủy, sự kiện liên quan chuyển **Đã hủy** và giữ nguyên trạng thái đó.

---

## 8. Dữ liệu được lưu ở đâu?

Dữ liệu nằm trong thư mục `data/` ở thư mục gốc dự án:

| File | Nội dung |
| :--- | :--- |
| `users.txt` | Tài khoản đăng nhập |
| `customers.txt` | Hồ sơ khách hàng |
| `halls.txt` | Sảnh |
| `services.txt` | Dịch vụ |
| `requests.txt` | Yêu cầu sự kiện |
| `request_lines.txt` | Các dòng dịch vụ của yêu cầu |
| `bookings.txt` | Booking |
| `payments.txt` | Khoản thu |
| `events.txt` | Sự kiện |

Quy ước:

- File UTF-8, mỗi dòng một bản ghi, các trường cách nhau bằng `|`.
- Dòng bắt đầu bằng `#` là header/chú thích, được bỏ qua khi đọc.
- Ngày giờ: `dd/MM/yyyy HH:mm`; tiền: số nguyên VND; boolean: `1`/`0`.
- Mỗi thao tác thay đổi dữ liệu đều ghi lại **toàn bộ 9 file** (không chỉ file bị ảnh hưởng).

**Bộ dữ liệu kèm repo** có sẵn 6 sảnh, 100 dịch vụ, 4 tài khoản và 3 khách hàng để chạy
demo ngay.

**Reset dữ liệu:**

1. Đóng chương trình.
2. Sao lưu (nếu cần) rồi xóa các file trong `data/`.
3. Chạy lại — chương trình tạo tài khoản/khách mẫu; sảnh và dịch vụ cần thêm qua menu
   Quản lý (admin) hoặc khôi phục `data/` từ git.

Định dạng chi tiết từng file: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md#8-lưu-trữ-dữ-liệu).

---

## 9. Cấu trúc dự án

```text
trongdongpalace/
├── main.cpp                  # điểm khởi động, bật UTF-8 cho console Windows
├── CMakeLists.txt            # cấu hình build
├── README.md
├── CONTRIBUTING.md
├── .gitignore
├── include/                  # header (.h)
│   ├── Booking.h
│   ├── ConsoleUI.h
│   ├── Customer.h
│   ├── DateTime.h
│   ├── Enums.h
│   ├── Event.h
│   ├── EventRequest.h
│   ├── Hall.h
│   ├── PalaceService.h
│   ├── Payment.h
│   ├── Person.h
│   ├── Service.h
│   ├── Store.h
│   ├── User.h
│   └── Utils.h
├── src/                      # cài đặt (.cpp)
│   ├── Booking.cpp
│   ├── ConsoleUI.cpp         # toàn bộ giao diện console
│   ├── Customer.cpp
│   ├── DateTime.cpp
│   ├── Enums.cpp
│   ├── Event.cpp
│   ├── EventRequest.cpp
│   ├── Hall.cpp
│   ├── PalaceBooking.cpp     # sảnh, dịch vụ, booking, thanh toán, sự kiện
│   ├── PalaceRequest.cpp     # yêu cầu sự kiện, tính giá, chọn dịch vụ
│   ├── PalaceService.cpp     # đăng nhập, phân quyền, tài khoản, khách hàng, lưu file
│   ├── Payment.cpp
│   ├── Person.cpp
│   ├── Service.cpp
│   ├── Store.cpp             # đọc/ghi data/*.txt, seed dữ liệu mẫu
│   ├── User.cpp
│   └── Utils.cpp
├── docs/
│   ├── ARCHITECTURE.md       # kiến trúc, sơ đồ lớp, luồng nghiệp vụ
│   ├── SETUP.md              # cài đặt môi trường chi tiết
│   └── TESTING.md            # checklist kiểm thử thủ công
└── data/                     # dữ liệu runtime (được commit kèm demo)
```

---

## 10. Kiểm thử

Hiện dự án **chưa có bộ test tự động**; kiểm thử được thực hiện thủ công theo checklist.

**Smoke test nhanh:**

```powershell
cmake --build build
.\build\trongdongpalace.exe
```

Kỳ vọng: hiện menu đăng nhập, đăng nhập `admin / 123456` thành công, xem được danh mục
6 sảnh và 100 dịch vụ, thoát không lỗi.

**Checklist đầy đủ:** [docs/TESTING.md](docs/TESTING.md) — bao gồm đăng nhập/phân quyền,
đăng ký, yêu cầu, booking 72 giờ, trùng lịch sảnh, thanh toán, sự kiện, lưu/đọc dữ liệu.

**Kịch bản demo nhanh:**

1. Đăng nhập `kinhdoanh / 123456`.
2. **1 → 2**: tạo yêu cầu cho `CUS-0001`, sảnh `HAL-0004`, thời gian tương lai, 200 khách,
   chọn vài dịch vụ.
3. **1 → 4**: đổi yêu cầu sang **Hoàn thành** → hệ thống tạo `BKG-...`.
4. **3 → 2**: ghi nhận **Đặt cọc lần 1** → booking **Đã cọc**, tự tạo sự kiện.
5. Đăng nhập `admin` hoặc `vanhanh` → **Danh sách sự kiện** để xem.
6. Đăng nhập `khachhang / 123456` → xem yêu cầu và booking của mình.

---

## 11. Cấu hình môi trường

- **Không dùng biến môi trường** và **không có file `.env`**. Cấu hình duy nhất là thư mục
  dữ liệu `data/` (được xác định trong `main.cpp`).
- Không cần tạo database, không cần cài server, không cần mạng.
- Trên Windows, chương trình tự bật console UTF-8 để hiển thị tiếng Việt.
- Build mặc định dùng `-Wall -Wextra -Wpedantic` (GCC/Clang) hoặc `/W4 /utf-8` (MSVC).
- Trên MinGW, exe được liên kết tĩnh (`-static -static-libgcc -static-libstdc++`) để có thể
  copy sang máy khác chạy mà không cần cài DLL.

---

## 12. Xử lý sự cố

| Hiện tượng | Nguyên nhân thường gặp | Cách xử lý |
| :--- | :--- | :--- |
| `Permission denied` khi build | `trongdongpalace.exe` đang chạy | Đóng app rồi build lại |
| `cmake: command not found` | Chưa cài CMake hoặc chưa mở đúng terminal MSYS2 | Cài `mingw-w64-ucrt-x86_64-cmake`, mở **MSYS2 UCRT64** |
| CMake không tìm thấy compiler | Chưa chọn toolchain | Truyền `-DCMAKE_CXX_COMPILER=...` như mục 4.1 |
| Chữ tiếng Việt bị lỗi font | Console dùng bảng mã cũ | Chạy đúng exe (đã bật UTF-8) trong Windows Terminal |
| `Dong bookings.txt khong du truong` | File dữ liệu sai định dạng | Khôi phục `data/bookings.txt` từ git hoặc xóa dòng lỗi |
| Không thấy sảnh/dịch vụ | Đã xóa `data/` | Khôi phục `data/` từ git hoặc thêm mới bằng tài khoản admin |
| Không đăng nhập được | Sai mật khẩu / tài khoản bị khóa | Dùng `123456`; admin mở khóa trong Quản lý tài khoản |

---

## 13. Tài liệu chi tiết

| Tài liệu | Nội dung |
| :--- | :--- |
| [docs/SETUP.md](docs/SETUP.md) | Cài đặt môi trường Windows/Linux/macOS, VS Code, build, chạy, reset dữ liệu |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Kiến trúc phân lớp, sơ đồ lớp, sơ đồ dữ liệu, luồng nghiệp vụ, định dạng file |
| [docs/TESTING.md](docs/TESTING.md) | Checklist kiểm thử thủ công theo module và vai trò |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Quy ước code, commit, quy trình đóng góp |

---

## 14. Điểm nhấn OOP (gợi ý bảo vệ)

| Khái niệm | Nơi thể hiện |
| :--- | :--- |
| **Trừu tượng** | `Person` có `roleName()` thuần ảo; `Service` có `quantityFor()`, `typeCode()`, `typeName()` thuần ảo; `PalaceService` là facade nghiệp vụ |
| **Đóng gói** | Thuộc tính `private`/`protected`, truy cập qua getter/setter; `DateTime`, `Database` giấu dữ liệu |
| **Kế thừa** | `User`, `Customer` kế thừa `Person`; `FoodService`, `PackageService` kế thừa `Service` |
| **Đa hình** | `vector<Service*>`; gọi `service->quantityFor(...)` và `service->toText()`; `Person::roleName()` |
| **Hàm tạo / hàm hủy** | Tham số mặc định; `Store::~Store()` xóa `Service*`; hủy ảo ở lớp cơ sở |
| **Nạp chồng toán tử** | `DateTime` nạp chồng `<`, `<=`, `==`, `!=`, `<<` |
| **Hàm static** | `DateTime::now()`, `DateTime::parse()`, `Customer::fromText()`, `PalaceService::kHoldHours` |
| **Template** | `findById<T>()` dùng chung cho nhiều kiểu thực thể |
| **Composition** | `Store` sở hữu `Database`; `Database` sở hữu các `vector`; `EventRequest` chứa `vector<RequestLine>` |
| **Liên kết bằng khóa** | `Booking` → `requestId`, `customerId`, `hallId`; `Payment` → `bookingId`; `Event` → `bookingId` |
| **Ngoại lệ** | `BusinessException` cho lỗi nghiệp vụ, `FileException` cho lỗi file |
| **STL / File I/O** | `vector`, `string`, `optional`, `filesystem`, `fstream` |

Giải thích chi tiết và sơ đồ: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md#9-thiết-kế-hướng-đối-tượng).

---

## 15. Giới hạn và hướng phát triển

- Ứng dụng console **một người dùng**, không có mạng, không đăng nhập đồng thời.
- Lưu file text, ghi lại toàn bộ file mỗi lần thay đổi — phù hợp đồ án, không phù hợp tải lớn.
- Mật khẩu plain text; cần hash + salt nếu triển khai thật.
- Chưa có test tự động; có thể bổ sung unit test cho `PalaceService` (trùng lịch, 72 giờ,
  thanh toán, phân quyền).
- Hướng mở rộng: xuất hóa đơn PDF, báo cáo doanh thu, gửi email, phân trang, hoặc thay
  `Store` bằng SQLite/CSDL thật.
