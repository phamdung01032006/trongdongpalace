# TRỐNG ĐỒNG PALACE — HỆ THỐNG ĐẶT LỊCH SỰ KIỆN

Đồ án C++17 hướng đối tượng, giao diện **console** (không dùng Qt), lưu dữ liệu bằng
các file text UTF-8 trong thư mục `data/`. Chương trình mô phỏng quy trình đặt lịch
sự kiện của một trung tâm tiệc cưới & sự kiện:

```text
Yêu cầu sự kiện  →  Tư vấn / Hoàn thành  →  Booking giữ chỗ 72 giờ
                 →  Đặt cọc  →  Tất toán  →  Sự kiện
```

Mục tiêu của bản này là **gọn, dễ đọc, dễ bảo vệ**: không thư viện ngoài, không
giao diện đồ họa, toàn bộ nghiệp vụ nằm trong các lớp C++ rõ ràng.

---

## 1. Công nghệ và phạm vi

| Thành phần | Lựa chọn |
| :--- | :--- |
| Ngôn ngữ | C++17 |
| Giao diện | Console (menu nhập số, bảng text) |
| Lưu trữ | File `.txt` UTF-8, phân tách bằng `\|` |
| Build | CMake 3.16+ (khuyến nghị), hoặc g++ trực tiếp |
| Thư viện ngoài | Không có |
| Nền tảng | Windows, Linux, macOS |

> **Lưu ý bảo mật:** mật khẩu trong `data/users.txt` đang lưu **plain text** để đơn
> giản hóa phần demo học thuật. Không dùng cách này cho hệ thống thật.

---

## 2. Cấu trúc thư mục

```text
trongdongpalace-anhduy/
├── main.cpp                  # điểm khởi động, bật UTF-8 cho console Windows
├── CMakeLists.txt            # cấu hình build
├── README.md
├── .gitignore
├── include/                  # toàn bộ header (.h)
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
├── src/                      # toàn bộ cài đặt (.cpp)
│   ├── Booking.cpp
│   ├── ConsoleUI.cpp
│   ├── Customer.cpp
│   ├── DateTime.cpp
│   ├── Enums.cpp
│   ├── Event.cpp
│   ├── EventRequest.cpp
│   ├── Hall.cpp
│   ├── PalaceBooking.cpp     # sảnh, dịch vụ, booking, thanh toán, sự kiện
│   ├── PalaceRequest.cpp     # yêu cầu sự kiện
│   ├── PalaceService.cpp     # đăng nhập, tài khoản, khách hàng, lưu file
│   ├── Payment.cpp
│   ├── Person.cpp
│   ├── Service.cpp
│   ├── Store.cpp             # đọc/ghi data/*.txt, seed dữ liệu mẫu
│   ├── User.cpp
│   └── Utils.cpp
└── data/                     # dữ liệu runtime (tự tạo khi chạy lần đầu)
    ├── users.txt
    ├── customers.txt
    ├── halls.txt
    ├── services.txt
    ├── requests.txt
    ├── request_lines.txt
    ├── bookings.txt
    ├── payments.txt
    └── events.txt
```

---

## 3. Build và chạy

### 3.1. Windows — MSYS2 / MinGW (khuyến nghị)

Cài các gói cần thiết trong MSYS2 UCRT64:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
                   mingw-w64-ucrt-x86_64-cmake \
                   mingw-w64-ucrt-x86_64-make
```

Build và chạy tại thư mục gốc dự án:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
.\build\trongdongpalace.exe
```

### 3.2. Linux / macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/trongdongpalace
```

### 3.3. Windows — MSVC / Visual Studio

```powershell
cmake -S . -B build-msvc
cmake --build build-msvc --config Release
.\build-msvc\Release\trongdongpalace.exe
```

`CMakeLists.txt` tự thêm `/utf-8` cho MSVC để đọc đúng tiếng Việt trong source.

### 3.4. Build nhanh bằng g++ (không cần CMake)

Trên bash/Linux:

```bash
g++ -std=c++17 -O2 -Iinclude main.cpp src/*.cpp -o trongdongpalace
```

Trên Windows PowerShell, dùng danh sách file thay cho ký tự `*`:

```powershell
$src = Get-ChildItem src -Filter *.cpp | ForEach-Object { $_.FullName }
g++ -std=c++17 -O2 -Iinclude main.cpp @src -o trongdongpalace.exe
```

### 3.5. Đường dẫn dữ liệu và cách reset

- Chương trình tìm thư mục `data` **cạnh thư mục đang chạy**; nếu không có thì thử
  `../data`. Vì vậy chạy `.\build\trongdongpalace.exe` từ thư mục gốc hoặc chạy
  trực tiếp trong `build/` đều được.
- Lần chạy đầu, nếu `data/users.txt` chưa tồn tại, chương trình tự tạo dữ liệu mẫu:
  4 tài khoản, 2 khách hàng, 6 sảnh và 100 dịch vụ.
- Muốn reset toàn bộ: xóa các file trong `data/` rồi chạy lại.

### 3.6. Điều hướng trong chương trình

Mọi màn hình đều có đường quay về:

- Ở menu con: chọn **`0. Quay lại`** để về màn hình trước, hoặc **`9. Về menu chính`**
  để về menu vai trò đang đăng nhập.
- Ở màn hình nhập liệu / danh sách / chi tiết (có dòng gợi ý `[b]` và `[m]`):
  gõ **`b`** để quay lại màn hình trước, gõ **`m`** để về menu chính.
  Không phân biệt chữ hoa/chữ thường.
- Tại màn hình đang hiển thị kết quả, nhấn Enter để tiếp tục, hoặc gõ `b` / `m`
  để điều hướng như trên.
- Ở menu đăng nhập: chọn **`0. Thoát chương trình`**. Ở menu vai trò: chọn
  **`0. Đăng xuất`**.

---

## 4. Tài khoản mẫu

Tất cả tài khoản mẫu dùng mật khẩu **`123456`**.

| Tên đăng nhập | Vai trò | Quyền chính |
| :--- | :--- | :--- |
| `admin` | Quản lý | Toàn quyền: yêu cầu, booking, thanh toán, sảnh, dịch vụ, tài khoản, duyệt chiết khấu > 10%, nạp danh mục mẫu |
| `kinhdoanh` | Nhân viên kinh doanh | Yêu cầu, booking, thanh toán, khách hàng, tra cứu sảnh, xem danh mục |
| `vanhanh` | Vận hành | Xem danh sách sự kiện và trạng thái theo thời gian |
| `khachhang` | Khách hàng | Tạo yêu cầu cho chính mình, xem yêu cầu/booking của mình, hủy yêu cầu, tra cứu sảnh |

Khách hàng mới có thể tự đăng ký ở màn hình đăng nhập (mục **2**). Hệ thống tự
tạo hồ sơ `Customer` và gắn với tài khoản.

---

## 5. Chức năng theo vai trò

### Khách hàng

1. Tạo yêu cầu sự kiện (chọn loại, sảnh, thời gian, số khách, dịch vụ).
2. Xem danh sách yêu cầu của mình (trạng thái được rút gọn: Đang xét duyệt /
   Hoàn thành / Bị hủy).
3. Xem booking và tình trạng thanh toán của mình.
4. Tra cứu sảnh còn trống theo thời gian và số khách.
5. Hủy yêu cầu của mình khi chưa chốt.

### Nhân viên kinh doanh

- Quản lý yêu cầu: xem, tạo, sửa, đổi trạng thái, xóa, xem 10 yêu cầu gần nhất.
- Quản lý booking: xem, tạo từ yêu cầu, sửa chiết khấu, hủy, xóa booking chờ cọc.
- Quản lý thanh toán: ghi nhận đặt cọc/tất toán, sửa khoản thu, xóa khoản chưa thu,
  in phiếu thu.
- Xem danh sách khách hàng, tra cứu sảnh, xem danh mục sảnh & dịch vụ.

### Quản lý

Gồm toàn bộ quyền của nhân viên kinh doanh, cộng thêm:

- Quản lý sảnh: thêm, sửa, xóa, đổi trạng thái (Khả dụng / Bảo trì / Không khả dụng).
- Quản lý dịch vụ: thêm, sửa, xóa, bật/tắt dịch vụ.
- Quản lý tài khoản: tạo tài khoản nhân viên, đổi vai trò, khóa/mở, đổi mật khẩu.
- Xem danh sách sự kiện.
- Duyệt chiết khấu vượt ngưỡng 10% trong **Quản lý booking**.

### Vận hành

- Xem danh sách sự kiện; trạng thái **Sắp diễn ra / Đang diễn ra / Đã kết thúc**
  được tính tự động theo đồng hồ máy tính.

---

## 6. Quy tắc nghiệp vụ chính

### 6.1. Vòng đời yêu cầu sự kiện

```text
Chờ  →  Đang tư vấn  →  Hoàn thành  →  Đã chốt (tự động tạo Booking)
                                  └──→  Bị hủy
```

- Khi yêu cầu chuyển sang **Hoàn thành**, hệ thống tự kiểm tra sảnh và tạo Booking.
- Nếu sảnh yêu cầu bị trùng lịch, hệ thống báo lỗi và **không** đổi trạng thái.
- Yêu cầu đã **Đã chốt** hoặc **Bị hủy** thì không đổi trạng thái nữa.

### 6.2. Booking và giữ chỗ 72 giờ

```text
Chờ cọc  ──(thu đặt cọc)──→  Đã cọc  ──(tất toán)──→  Hoàn tất
    │
    └──(quá 72 giờ chưa cọc)──→  Đã hủy
```

- Booking mới có trạng thái **Chờ cọc**, hạn giữ chỗ = thời điểm tạo + 72 giờ
  (`kHoldHours`).
- Mỗi lần đăng nhập hoặc vào menu, hệ thống gọi `refreshSystem()` để quét booking
  quá hạn; booking quá hạn chưa cọc bị hủy và **trả yêu cầu về Hoàn thành** để có
  thể xử lý lại.
- Booking **Chờ cọc / Đã cọc / Hoàn tất** đều chiếm sảnh. Chỉ booking **Đã hủy**
  mới giải phóng sảnh.
- Xóa booking **Chờ cọc** (chưa thu tiền) sẽ xóa các khoản chưa thu liên quan và
  trả yêu cầu về **Hoàn thành**. Booking đã có tiền không được xóa; dùng chức năng
  hủy để giữ lịch sử.

### 6.3. Kiểm tra trùng lịch sảnh

Hai khoảng thời gian `[start, end)` giao nhau nếu:

```text
aStart < bEnd  và  bStart < aEnd
```

Sảnh còn trống khi:

- Sảnh tồn tại và đang **Khả dụng**.
- `start < end`.
- Không có booking nào đang chiếm sảnh bị giao thời gian.

### 6.4. Thanh toán

- Loại khoản thu: **Đặt cọc lần 1**, **Đặt cọc lần 2**, **Tất toán**.
- Hình thức: Tiền mặt, Chuyển khoản, Thẻ tín dụng.
- Không cho ghi nhận số tiền `<= 0`.
- Không cho thu vượt số tiền còn lại.
- **Tất toán bắt buộc thu đủ số còn lại**; không chấp nhận tất toán một phần.
- Khi thu đặt cọc thành công, booking chuyển **Đã cọc** và hệ thống tạo sự kiện.
- Khi tất toán đủ, booking chuyển **Hoàn tất**.

### 6.5. Chiết khấu

- Ngưỡng cần duyệt: **> 10%** tổng dự kiến, hoặc số tiền giảm **> 10%** tổng.
- Nhân viên kinh doanh nhập chiết khấu vượt ngưỡng → booking ở trạng thái
  **cần duyệt**.
- **Chưa duyệt thì chiết khấu chưa được trừ** vào `get_finalTotal()`.
- Quản lý vào **Quản lý booking → 6. Duyệt chiết khấu** để duyệt; sau đó tổng tiền mới giảm.

### 6.6. Sự kiện

- Sự kiện được tạo tự động khi booking được thu tiền lần đầu.
- Trạng thái sự kiện được tính lại theo thời gian:
  - `now < start`: Sắp diễn ra
  - `start <= now <= end`: Đang diễn ra
  - `end < now`: Đã kết thúc
- Booking bị hủy thì sự kiện tương ứng cũng bị hủy.

---

## 7. Định dạng file dữ liệu

### Quy ước chung

- File UTF-8, mỗi dòng là một bản ghi.
- Dòng bắt đầu bằng `#` là dòng chú thích/header, được bỏ qua khi đọc.
- Các trường cách nhau bằng ký tự `|`.
- Ngày giờ: `dd/MM/yyyy HH:mm` (ví dụ `25/12/2026 18:00`).
- Tiền: số nguyên VND, không dấu phân cách (ví dụ `18000000`).
- Boolean: `1` / `0`.
- Enum được ghi bằng tên tiếng Việt; khi đọc, chương trình cũng chấp nhận mã
  tiếng Anh (`Sales`, `DaCoc`, `TatToan`, ...).
- Mỗi lần lưu, chương trình ghi lại toàn bộ file. Không sửa file khi chương trình
  đang chạy.

### 7.1. `users.txt`

```text
id | username | password | hoTen | sdt | email | diaChi | vaiTro | customerId | active
```

`vaiTro`: `Khách hàng`, `Nhân viên kinh doanh`, `Quản lý`, `Vận hành`.
`customerId` chỉ có giá trị với tài khoản khách hàng.

### 7.2. `customers.txt`

```text
id | hoTen | sdt | email | diaChi | ghiChu | createdAt
```

### 7.3. `halls.txt`

```text
id | ten | sucChua | giaThue | trangThai
```

`trangThai`: `Khả dụng`, `Bảo trì`, `Không khả dụng`.

### 7.4. `services.txt`

```text
type | id | ma | ten | nhom | donVi | donGia | active
```

- `type`: `FOOD` (món ăn, tính theo bàn) hoặc `PACKAGE` (gói dịch vụ).
- `donVi` ví dụ: `VNĐ/bàn`, `VNĐ/gói`, `VNĐ/người/buổi`.
- `active`: `1` đang dùng, `0` ngừng dùng.

### 7.5. `requests.txt`

```text
id | customerId | loai | ten | tenKhach | sdt | hallId | batDau | ketThuc | soKhach | ghiChu | trangThai | createdAt
```

`loai` là tên loại sự kiện (Tiệc cưới, Gala Dinner, ...).
`hallId` có thể rỗng nếu khách chưa chọn sảnh.

### 7.6. `request_lines.txt`

```text
requestId | serviceId | ten | donVi | soLuong | donGia
```

Đây là các dòng dịch vụ của từng yêu cầu. Tên, đơn vị và đơn giá được lưu dạng
**snapshot** tại thời điểm chọn, để sửa bảng giá sau này không làm lệch yêu cầu cũ.

### 7.7. `bookings.txt`

```text
id | requestId | customerId | hallId | batDau | ketThuc | soKhach | tongDuKien |
chietKhauPhanTram | chietKhauTien | canDuyet | daDuyet | trangThai |
createdAt | holdUntil | sales | cancelReason
```

`canDuyet` / `daDuyet`: `1` / `0`.

### 7.8. `payments.txt`

```text
id | bookingId | loai | soTien | ngayThu | hinhThuc | trangThai | ghiChu | nguoiThu
```

`loai`: `Đặt cọc lần 1`, `Đặt cọc lần 2`, `Tất toán`.
`trangThai`: `Chưa thanh toán`, `Đã thanh toán`.

### 7.9. `events.txt`

```text
id | bookingId | customerId | hallId | batDau | ketThuc | trangThai | ghiChu
```

`trangThai`: `Sắp diễn ra`, `Đang diễn ra`, `Đã kết thúc`, `Đã hủy`.

---

## 8. Thiết kế hướng đối tượng

### 8.1. Sơ đồ lớp rút gọn

```text
Person (abstract)
├── User
└── Customer

Service (abstract)
├── FoodService
└── PackageService

Hall
EventRequest  ◆── RequestLine
Booking
Payment
Event

Store  ◆── Database ── vector<...> các lớp trên
PalaceService ── dùng Store, chứa toàn bộ nghiệp vụ
ConsoleUI ── gọi PalaceService, không tự sửa dữ liệu
```

### 8.2. Các khái niệm OOP trong đồ án

| Khái niệm | Nơi thể hiện |
| :--- | :--- |
| **Trừu tượng (Abstraction)** | `Person` có `roleName()` thuần ảo; `Service` có `quantityFor()`, `typeCode()`, `typeName()` thuần ảo; `PalaceService` là facade nghiệp vụ |
| **Đóng gói (Encapsulation)** | Thuộc tính `private`/`protected`, truy cập qua `get_...()`/`set_...()`; `DateTime` và `Database` cũng giấu dữ liệu bên trong |
| **Kế thừa (Inheritance)** | `User`, `Customer` kế thừa `Person`; `FoodService`, `PackageService` kế thừa `Service` |
| **Đa hình (Polymorphism)** | `vector<Service*>`; gọi `service->quantityFor(...)`, `service->toText()`; `Person::roleName()` |
| **Hàm tạo / hàm hủy** | Hàm tạo có tham số mặc định (ví dụ `DateTime(int year = 1970, ...)`); `Store::~Store()` xóa `Service*`; `Person`/`Service` có hàm hủy ảo |
| **Toán tử tải bội (Operator overloading)** | `DateTime` tải bội `<`, `<=`, `==`, `!=` và `<<`; các toán tử đối xứng khai báo `friend` |
| **Hàm thành viên static** | `DateTime::now()`, `DateTime::parse()`, `Customer::fromText()`, `Service::fromText()`... dùng chung, không cần tạo đối tượng; hằng dùng chung `PalaceService::kHoldHours`, `kDiscountApprovalPercent` khai báo `static constexpr` |
| **Template** | `findById<T>()` trong `Utils.h` dùng chung cho `User`, `Customer`, `Hall`, `EventRequest`, `Booking`, `Payment` |
| **Thành phần (Composition)** | `Store` sở hữu `Database`; `Database` sở hữu các `vector`; `EventRequest` chứa `vector<RequestLine>` |
| **Liên kết bằng khóa** | `Booking` tham chiếu `requestId`, `customerId`, `hallId`; `Payment` tham chiếu `bookingId`; `Event` tham chiếu `bookingId` |
| **Ghi file (File I/O)** | `Store::load()`, `Store::save()`, `readTextFile()`, `writeTextFile()`; mở bằng `open()`, kiểm tra `is_open()`, đóng bằng `close()` |
| **Xử lý ngoại lệ** | `BusinessException` cho lỗi nghiệp vụ, `FileException` cho lỗi file; UI bắt `const BusinessException&`, `main()` bắt `const exception&` trước `catch (...)` |
| **STL** | `vector`, `string`, `optional`, `filesystem`, `algorithm` |

### 8.3. Đa hình trong `Service`

```cpp
class Service {
public:
    virtual ~Service() = default;
    virtual int quantityFor(int guestCount, int enteredQty) const = 0;
    virtual string typeCode() const = 0;
    virtual string typeName() const = 0;

    string toText() const;  // Dung chung cho FoodService va PackageService
};

class FoodService : public Service {
public:
    int quantityFor(int guestCount, int enteredQty) const override;
    string typeCode() const override { return "FOOD"; }
};

class PackageService : public Service {
public:
    int quantityFor(int guestCount, int enteredQty) const override;
    string typeCode() const override { return "PACKAGE"; }
};
```

`Store` giữ `vector<Service*>`. Khi đọc file, `Service::fromText()` trả về
`new FoodService(...)` hoặc `new PackageService(...)`; khi tính tiền, lời gọi
`quantityFor()` được phân giải động. `Store` xóa các con trỏ trong destructor và
cấm copy để tránh double-delete.

### 8.4. Vì sao tách `Store`, `PalaceService`, `ConsoleUI`?

- `Store`: chỉ biết đọc/ghi file và giữ dữ liệu.
- `PalaceService`: chỉ chứa quy tắc nghiệp vụ và phân quyền.
- `ConsoleUI`: chỉ nhập/xuất và gọi service.

Cách tách này giúp sinh viên giải thích **single responsibility**: giao diện không
tự sửa `Database`, nghiệp vụ không phụ thuộc vào menu console.

### 8.5. Phân quyền

`PalaceService` dùng `enum class Action` và hàm `can(Action)` / `require(Action)`.
Mỗi thao tác nhạy cảm gọi `require(...)` trước khi chạy. Nhờ vậy, quy tắc phân
quyền chỉ nằm một chỗ, không rải rác trong UI.

### 8.6. Luồng ngoại lệ

- `PalaceService`/`PalaceBooking`/`PalaceRequest` ném `BusinessException` khi vi phạm
  quy tắc nghiệp vụ (sai mật khẩu, trùng lịch, không đủ quyền, tất toán thiếu...).
- `Store` ném `FileException` khi không mở/đọc/ghi được file.
- `ConsoleUI` bắt `BusinessException` tại menu và in `e.what()`; `main()` bắt
  `const exception&` trước `catch (...)` để báo lỗi khởi động.
- `OpResult` chỉ còn là kết quả **thành công** (message + id), không còn mang lỗi.

---

## 9. Kịch bản demo nhanh

1. Build và chạy chương trình.
2. Chọn **1. Đăng nhập**, nhập `kinhdoanh` / `123456`.
3. Vào **1. Quản lý yêu cầu sự kiện** → **2. Tạo yêu cầu**:
   - Chọn khách hàng `CUS-0001`.
   - Loại sự kiện: Tiệc cưới.
   - Sảnh: `HAL-0004` (sức chứa 300).
   - Bắt đầu: `25/12/2026 18:00`; kết thúc: `25/12/2026 22:00`.
   - Số khách: `200`.
   - Dịch vụ: `FOD01-P` (số lượng `0` để tự tính theo bàn), sau đó `DEC02-P`
     (số lượng `0`), rồi Enter để kết thúc.
4. Quay lại menu yêu cầu → **4. Đổi trạng thái** → nhập mã yêu cầu vừa tạo →
   chọn **Hoàn thành**. Hệ thống tạo booking `BKG-0001`.
5. Vào **3. Quản lý thanh toán** → **2. Ghi nhận thanh toán**:
   - Booking: `BKG-0001`.
   - Loại: Đặt cọc lần 1.
   - Số tiền: `50000000`.
   - Hình thức: Chuyển khoản.
   - Ngày: Enter để lấy hiện tại.
   - Ghi chú: `Dat coc lan 1`.
6. Đăng xuất, đăng nhập `vanhanh` / `123456` → **1. Danh sách sự kiện** để xem
   sự kiện vừa được tạo.
7. Đăng nhập `admin` / `123456`:
   - **Quản lý booking → 6. Duyệt chiết khấu** nếu nhân viên đã nhập chiết khấu > 10%.
   - **7/8. Quản lý sảnh/dịch vụ**, **9. Quản lý tài khoản**.
8. Đăng nhập `khachhang` / `123456` để xem yêu cầu và booking của mình.

> Mã `REQ-...`, `BKG-...` có thể khác nếu dữ liệu không còn mới. Hãy dùng mã hiển
> thị trên màn hình.

---

## 10. Gợi ý trả lời khi bảo vệ

**1. Vì sao lỗi nghiệp vụ dùng ngoại lệ?**
Lỗi nghiệp vụ (sai mật khẩu, trùng lịch, thu quá tiền) được ném dưới dạng
`BusinessException`; giao diện bắt `const BusinessException&` tại menu và in
`e.what()`. Lỗi file ném `FileException`. `OpResult` chỉ còn mang kết quả thành
công (message + id), nhờ vậy luồng xử lý lỗi không bị rải rác trong service.

**2. Cơ chế giữ chỗ 72 giờ nằm ở đâu?**
Hằng số `PalaceService::kHoldHours = 72`; khi tạo booking, `holdUntil = now + 72h`. Hàm
`sweepExpiredHolds()` quét booking `ChoCoc` quá hạn và chưa có tiền cọc, chuyển
sang `DaHuy`. Hàm `refreshSystem()` được gọi khi đăng nhập và khi vào menu.

**3. Kiểm tra trùng lịch sảnh thế nào?**
`timeOverlaps(aStart, aEnd, bStart, bEnd)` dùng điều kiện
`aStart < bEnd && bStart < aEnd`. `hallAvailable()` duyệt các booking đang chiếm
sảnh (`ChoCoc`, `DaCoc`, `HoanTat`).

**4. Vì sao chiết khấu chưa duyệt không được trừ tiền?**
`Booking::set_discount()` đặt `discountApproved_ = !needsApproval`. `get_finalTotal()`
chỉ trừ `discountAmount_` khi `discountApproved_` là true. Quản lý duyệt bằng
`approveDiscount()`.

**5. Đa hình thể hiện ở đâu?**
`Database` giữ `vector<Service*>`. Khi tính số lượng, chương trình gọi
`service->quantityFor(guestCount, enteredQty)`. Con trỏ có thể trỏ tới `FoodService` hoặc
`PackageService`, nên phương thức đúng được gọi tại runtime. Tương tự,
`Person::roleName()` được cài riêng cho `User` và `Customer`.

**6. File dữ liệu được đọc/ghi thế nào?**
`Store::load()` đọc từng file, bỏ dòng `#`, tách trường bằng `|`, gọi
`fromText()` của từng lớp. `Store::save()` ghi lại toàn bộ file với header.
`request_lines.txt` lưu dịch vụ của yêu cầu theo `requestId`.

---

## 11. Phạm vi và hướng phát triển

- Đây là ứng dụng console một người dùng, không có đăng nhập đồng thời hay mạng.
- Mật khẩu đang lưu plain text để phục vụ demo; hệ thống thật cần hash + salt.
- Có thể mở rộng: xuất hóa đơn PDF, báo cáo doanh thu, gửi email, phân trang,
  hoặc thay `Store` bằng cơ sở dữ liệu SQL.
- Có thể bổ sung kiểm thử tự động cho các quy tắc nghiệp vụ (trùng lịch, 72 giờ,
  chiết khấu, thanh toán).
