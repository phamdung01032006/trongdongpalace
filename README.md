# Hệ thống quản lý trung tâm sự kiện và tiệc cưới Trống Đồng Palace

Đồ án áp dụng lập trình hướng đối tượng bằng **C++17**, sử dụng **Qt 6 Widgets**
để xây dựng giao diện và **file TXT** để lưu dữ liệu. Bản refactor giữ giao diện,
luồng sử dụng, các vai trò và định dạng dữ liệu hiện tại.

## 1. Yêu cầu phần mềm

### 1.1. Mục tiêu và người sử dụng

Chương trình hỗ trợ tiếp nhận nhu cầu tổ chức sự kiện, tư vấn sảnh và dịch vụ,
chốt booking, thu tiền và theo dõi lịch sự kiện tại Trống Đồng Palace.

| Người sử dụng | Chức năng chính |
| --- | --- |
| Khách vãng lai | Xem sảnh/dịch vụ, tra cứu sảnh trống, nhập hồ sơ và gửi yêu cầu, xem yêu cầu vừa gửi trong phiên |
| Khách hàng (`Customer`) | Đăng nhập, gửi và xem yêu cầu của mình, hủy yêu cầu theo điều kiện, xem booking của mình |
| Kinh doanh (`Sales`) | Quản lý khách hàng, tư vấn yêu cầu, tạo booking, ghi nhận thanh toán |
| Quản lý (`Manager`) | Quản lý tài khoản, sảnh, dịch vụ; duyệt chiết khấu và sử dụng các chức năng nghiệp vụ |
| Vận hành (`Operation`) | Xem sự kiện, lịch và hướng dẫn chuẩn bị theo loại sự kiện |

Quyền được kiểm tra bằng `PalaceService::can()` và `require()`, ngoài việc ẩn/hiện
chức năng trên giao diện. Khách vãng lai không phải một giá trị của `Role`.

### 1.2. Yêu cầu chức năng đã hiện thực

| Mã | Yêu cầu chức năng | Lớp/hàm đảm nhiệm |
| --- | --- | --- |
| F01 | Đăng nhập, đăng xuất, tự đăng ký tài khoản khách hàng | `PalaceService::login/registerUser/logout` |
| F02 | Tạo/xóa tài khoản, đổi vai trò, khóa/mở, đổi mật khẩu | `User`, `createUser/updateUserRole/setUserActive/changePassword` |
| F03 | Thêm, sửa, xóa và tìm kiếm khách hàng | `Customer`, `addCustomer/updateCustomer/deleteCustomer/searchCustomers` |
| F04 | Quản lý tên, sức chứa, giá thuê và trạng thái sảnh | `Hall`, `addHall/updateHall/deleteHall` |
| F05 | Quản lý dịch vụ theo mã, nhóm, đơn vị và đơn giá | `ServiceItem`, `addService/updateService/deleteService` |
| F06 | Nhận yêu cầu gồm loại sự kiện, thời gian, số khách, sảnh và dịch vụ | `EventRequest`, `RequestServiceLine`, `addRequest` |
| F07 | Tư vấn, sửa, hủy/xóa và xem lịch sử yêu cầu | `updateRequest/updateRequestStatus/cancelOwnRequest/requestHistory` |
| F08 | Tra cứu sảnh theo thời gian và sức chứa, kiểm tra trùng lịch | `findAvailableHalls`, `validateBookingSlot` |
| F09 | Chốt yêu cầu thành booking, giữ chỗ 72 giờ, hủy/xóa theo điều kiện | `Booking`, `createBooking/sweepExpiredHolds/cancelBooking` |
| F10 | Tính dự toán, áp dụng giảm giá và duyệt giảm giá vượt ngưỡng | `estimateRequest`, `Booking::finalTotal`, `approveDiscount` |
| F11 | Tạo khoản thu, ghi nhận đặt cọc/thanh toán, tính đã thu và phiếu thu | `Payment`, `recordPayment/paidAmount/paymentReceipt` |
| F12 | Tạo sự kiện khi chốt booking, theo dõi trạng thái và hướng dẫn chuẩn bị | `Event`, ba lớp con, `refreshEventStatuses` |
| F13 | Lưu/nạp dữ liệu, kiểm tra file lỗi và sao lưu bản trước | `TextStorage::load/save` |

Luồng nghiệp vụ hiện tại:

```text
Khách hàng → Yêu cầu sự kiện → Tra cứu sảnh → Chọn dịch vụ/dự toán
          → Booking + Sự kiện + Khoản chờ thu → Đặt cọc/thanh toán
          → Theo dõi tổ chức → Hoàn tất và tra cứu lịch sử
```

Sự kiện được tạo **ngay khi chốt booking**. Thu cọc cập nhật trạng thái booking;
không phải đợi thu cọc mới tạo đối tượng `Event`.

### 1.3. Quy tắc nghiệp vụ

- Thời gian bắt đầu và kết thúc phải hợp lệ; `start < end`.
- Hai booking đang chiếm cùng sảnh không được giao nhau:

  ```cpp
  newStart < oldEnd && newEnd > oldStart
  ```

  Hai ca tiếp giáp, ví dụ 18:00–22:00 và 22:00–23:00, không overlap.
- Booking chờ cọc hoặc đã cọc chiếm lịch sảnh; booking hủy/hoàn tất không chiếm lịch theo mô hình hiện tại.
- Sảnh phải khả dụng; số khách khi chốt booking phải dương và không vượt sức chứa.
- Giữ sảnh chờ cọc 72 giờ; hàm quét hết hạn hủy booking chưa cọc và sự kiện tương ứng.
- Dự toán = tiền thuê sảnh + tổng số lượng × đơn giá dịch vụ. Dòng dịch vụ giữ giá/tên tại thời điểm chọn.
- Giá cuối booking = dự toán − số tiền giảm. Phần trăm và số tiền giảm là hai trường riêng trong code hiện tại.
- Chiết khấu vượt ngưỡng 10% do kinh doanh lập cần quản lý duyệt.
- Khoản chờ thu có giá trị 0; thanh toán thực tế phải lớn hơn 0, có ghi chú và ngày thu hợp lệ.
- Tổng đã thu chỉ cộng `Payment` đã thanh toán. Công nợ có thể tính bằng `finalTotal() - paidAmount(id)`; chưa có enum trạng thái công nợ cho cả booking.
- Không xóa khách hàng/sảnh/dịch vụ đang được tham chiếu theo các kiểm tra của service.
- Liên kết đối tượng bằng ID, không ghi địa chỉ con trỏ C++ vào file.

### 1.4. Phạm vi hiện thực

Tài liệu mô tả **code đang chạy**. Chưa có lớp riêng cho `Center`, `Package`,
`Quotation`, `Contract`, `Staff`, `Table`, `TableAssignment`, `EventRequirement`,
`Incident` hay `ReportService`. Dự toán nằm ở yêu cầu/booking; điều phối hiện có
lịch, trạng thái và ghi chú/hướng dẫn. Chưa có module hợp đồng, sơ đồ bàn, phân
công nhân viên hoặc báo cáo doanh thu độc lập.

Dự án dùng bốn vai trò `Customer/Sales/Manager/Operation`, chưa tách
`ADMIN/ACCOUNTANT/COORDINATOR/MANAGEMENT`. Đây là phần cần bổ sung nếu nhóm muốn
bao phủ đặc tả mở rộng; không đưa vào biểu đồ như lớp đã hiện thực.

## 2. Thiết kế lớp và đối tượng

### 2.1. Phân chia trách nhiệm

```text
MainWindow (Qt) → PalaceService → TextStorage → data/*.txt
                       ↓              ↓
                    Các model ← Database
```

- **UI:** `MainWindow` nhận dữ liệu, gọi service, hiển thị bảng/hộp thoại Qt.
- **Nghiệp vụ:** `PalaceService` kiểm tra quyền, đầu vào, lịch sảnh và điều phối thao tác quản lý.
- **Lưu trữ:** `TextStorage` đọc/ghi TXT, kiểm tra dữ liệu nạp, tạo mẫu lần đầu và sao lưu. Chưa tách các repository CRUD riêng.
- **Dữ liệu bộ nhớ:** `Database` chứa các danh sách đối tượng.
- **Model:** dữ liệu và hành vi gắn với đối tượng, như tính thành tiền và xác định booking có chiếm sảnh.

Giữ một `PalaceService` giúp theo dõi luồng gọi hàm trong quy mô bài tập.
UI không đọc/ghi TXT trực tiếp.

### 2.2. Các lớp chính

| Lớp | Thuộc tính tiêu biểu | Hành vi/trách nhiệm |
| --- | --- | --- |
| `User` | ID, username, salt/hash, họ tên, role, active, customerId | Tài khoản, liên kết hồ sơ khách |
| `Customer` | ID, tên, SĐT, email, địa chỉ, tuổi, ghi chú | Hồ sơ khách hàng |
| `Hall` | ID, tên, capacity, rentalPrice, status | Sảnh, sức chứa, trạng thái |
| `ServiceItem` | ID, mã, tên, đơn vị, giá, nhóm, active | Danh mục dịch vụ |
| `RequestServiceLine` | serviceId, tên/đơn vị chụp lại, quantity, unitPrice | `amount()` tính thành tiền |
| `EventRequest` | customerId, type, title, hallId, lịch, số khách, lines | `servicesTotal()` tính tổng dịch vụ |
| `Booking` | requestId, customerId, hallId, lịch, dự toán, giảm giá, hạn giữ chỗ | `finalTotal()`, `occupiesHall()`, `setDiscount()` |
| `Payment` | bookingId, type, amount, paidAt, method, status | Một khoản thu/chờ thu |
| `Event` | bookingId, customerId, hallId, lịch, status, specialNotes | Giao diện trừu tượng cho các loại sự kiện |
| `WeddingEvent` | Kế thừa dữ liệu `Event` | Tiệc cưới, hướng dẫn riêng |
| `BirthdayEvent` | Kế thừa dữ liệu `Event` | Sinh nhật, hướng dẫn riêng |
| `CorporateEvent` | Dữ liệu `Event`, thêm `type_` | Các loại sự kiện còn lại, giữ đúng nhãn loại |
| `Database` | Các vector model; `vector<shared_ptr<Event>>` | Gom dữ liệu làm việc |
| `TextStorage` | dataDir, backupDir, Database | `load()`, `save()`, kiểm tra và sao lưu |
| `PalaceService` | Tham chiếu storage, ID người đăng nhập | Nghiệp vụ và phân quyền |
| `MainWindow` | Tham chiếu service, các widget Qt | Form nhập, điều hướng, bảng, thông báo |
| `DateTime` | year, month, day, hour, minute | Parse, kiểm tra, so sánh, cộng giờ |
| `OpResult` | ok, message, id | Kết quả thao tác cho UI |

### 2.3. Class, object và đóng gói

`Customer` là khuôn mẫu; mỗi khách cụ thể là một object. Thuộc tính của model
để `private`, truy cập bằng getter/setter. Constructor khởi tạo bằng initializer
list, tương tự cách viết trong `giaiLaiDeGK.cpp`:

```cpp
Customer(const string& id, const string& name, const string& phone,
         const string& email, const string& address,
         const string& notes, DateTime createdAt)
    : id_(id), name_(name), phone_(phone), email_(email),
      address_(address), notes_(notes), createdAt_(createdAt) {}
```

Constructor và hành vi ngắn nằm ngay trong class. `toFields()/fromFields()`
đặt phía dưới cùng file để tách phần chuyển đổi lưu trữ khi đọc. Chuỗi truyền
bằng `const string&`; hàm getter/tính toán không sửa đối tượng dùng `const`.
Setter đơn giản không thay thế validation nghiệp vụ: UI phải gọi service.

### 2.4. Composition và association

- `TextStorage` chứa trực tiếp một `Database`: composition.
- `Database` chứa các vector model giá trị: vòng đời bản ghi do danh sách quản lý.
- `EventRequest` chứa `vector<RequestServiceLine>`: composition, các dòng thuộc yêu cầu.
- `Database` quản lý sự kiện bằng `shared_ptr<Event>`: chia sẻ quyền sở hữu; không biểu diễn như composition độc quyền.
- `PalaceService` giữ `TextStorage&`; `MainWindow` giữ `PalaceService&`: association không sở hữu, được truyền từ ngoài.
- Khách–booking, sảnh–booking, booking–payment là liên kết bằng ID; không tự suy ra sở hữu C++ hoặc tự động xóa dây chuyền.

## 3. Kế thừa

```text
Event (abstract)
├── WeddingEvent
├── BirthdayEvent
└── CorporateEvent
```

`Event` gom dữ liệu chung về khách, sảnh, booking, thời gian và trạng thái.
Lớp con kế thừa `public Event`, gọi constructor cha để khởi tạo phần chung,
rồi hiện thực hành vi riêng bằng `override`:

```cpp
class WeddingEvent : public Event {
public:
    WeddingEvent(const string& id, const string& bookingId,
                 const string& customerId, const string& hallId,
                 DateTime start, DateTime end, EventStatus status,
                 const string& notes)
        : Event(id, bookingId, customerId, hallId, start, end, status, notes) {}

    EventType getEventType() const override { return EventType::TiecCuoi; }
    string preparationGuide() const override {
        return "Chuan bi le duong, ban gia tien va kich ban don co dau chu re.";
    }
};
```

Có 10 nhãn `EventType`. Tiệc cưới và sinh nhật có lớp riêng; loại khác dùng
`CorporateEvent` với `type_` giữ nhãn cụ thể. Không có 10 lớp con.
Kế thừa còn có ở `MainWindow : QMainWindow` và cây exception tự định nghĩa.

## 4. Đa hình

### 4.1. Lớp trừu tượng và hàm ảo

```cpp
virtual EventType getEventType() const = 0;
virtual string preparationGuide() const = 0;
virtual ~Event() = default;
```

`= 0` làm `Event` thành lớp trừu tượng, không tạo trực tiếp được một `Event`.
Destructor ảo bảo đảm hủy đúng lớp con khi quản lý qua lớp cha.

### 4.2. Đa hình lúc chạy trong chương trình

`Database::events` là `vector<shared_ptr<Event>>`. `makeEvent()` tạo đúng lớp con.
Màn hình sự kiện trong `MainWindow::getRows()` gọi hàm qua `Event&`:

```cpp
for (const auto& eventPtr : service_.allEvents()) {
    const Event& event = *eventPtr;
    // Chuỗi này được đưa vào cột hướng dẫn trên bảng Qt.
    string guide = event.preparationGuide();
}
```

| Đối tượng thực | Hành vi được gọi |
| --- | --- |
| `WeddingEvent` | Chuẩn bị lễ đường, bàn gia tiên, đón cô dâu chú rể |
| `BirthdayEvent` | Chuẩn bị bánh, nến, trang trí theo độ tuổi |
| `CorporateEvent` | Chuẩn bị máy chiếu, âm thanh, bảng tên, check-in |

Đọc `events.txt` phục hồi đúng lớp con qua `Event::fromFields()` và `makeEvent()`.
Danh sách vẫn dùng kiểu cha. Kế thừa phục vụ hướng dẫn vận hành thực tế.

So với bài mẫu dùng `vector<Nguoi*>` và `delete`, dự án giữ `shared_ptr<Event>`
để tự quản lý vòng đời. Nguyên lý gọi hàm ảo qua kiểu cha vẫn tương tự.
`DateTime` nạp chồng `<`, `<=`, `==` để so sánh lịch; đây là nạp chồng toán tử,
khác với đa hình lúc chạy của `Event`.

## 5. Xử lý ngoại lệ

### 5.1. Phân cấp ngoại lệ

```text
std::runtime_error
└── AppException
    ├── ValidationException
    ├── NotFoundException
    ├── DuplicateException
    ├── BookingConflictException
    ├── AuthenticationException
    └── FileException
        └── StorageError
```

| Ngoại lệ | Ví dụ trong code |
| --- | --- |
| `ValidationException` | Sai ngày/số/bool/cột TXT, khoảng thời gian sai |
| `NotFoundException` | `validateBookingSlot()` không tìm thấy sảnh |
| `DuplicateException` | `checkTextIds()` phát hiện ID lặp |
| `BookingConflictException` | Booking cùng sảnh giao nhau về thời gian |
| `AuthenticationException` | `authenticate()` gặp tài khoản thiếu/khóa hoặc mật khẩu sai |
| `FileException` / `StorageError` | Không mở được file, dữ liệu nạp lỗi, ghi/sao lưu thất bại |

### 5.2. Throw tại nơi phát hiện lỗi, catch trước khi trả về Qt

`validateBookingSlot()` kiểm tra lịch và throw đúng loại lỗi. `hallAvailable()`
bắt `AppException`, chuyển `what()` thành lý do thất bại. Luồng tạo booking
nhận lý do và trả `OpResult::fail()`; Qt hiện hộp thoại:

```cpp
try {
    validateBookingSlot(hallId, start, end, ignoreBookingId);
    reason.clear();
    return true;
} catch (const AppException& ex) {
    reason = ex.what();
    return false;
}
```

`login()` bắt `AuthenticationException`. `TextStorage::load()` bắt lỗi parse và
kiểm tra bản ghi, bọc thành `StorageError` kèm tên file. `persist()` bắt
`std::exception` của thao tác lưu và trả kết quả lỗi. `main()` bắt lỗi khởi động,
in thông tin rồi kết thúc có kiểm soát.

Nhiều kiểm tra đầu vào vẫn trả `OpResult` để giữ cách gọi Qt hiện có; không phải
mọi lỗi đều được throw. Các luồng nghiệp vụ vừa refactor chuyển exception thành
kết quả trước khi trả về callback Qt.

Nạp dữ liệu lỗi không ghi đè file. Chỉ thay `Database` sau khi toàn bộ bản ghi
đã được đọc và kiểm tra thành công.

## 6. Cấu trúc mã và phong cách refactor

```text
src/
  main.cpp                  Khởi tạo storage → service → Qt UI
  models/                   Class, constructor, getter/setter, chuyển đổi TXT
    Event.cpp               Event và ba lớp con
    Exceptions.cpp          Cây ngoại lệ tự định nghĩa
    Database.cpp            Danh sách đối tượng
  services/palace_service.cpp
  storage/
    text_storage.cpp        Đọc/ghi, kiểm tra, sao lưu TXT
    catalog_seed.cpp        Danh mục mẫu
  ui/app_ui.cpp             MainWindow và form Qt
  utils/                    DateTime, định dạng, mật khẩu, tách/ghép cột
data/                       8 file TXT
tests/                      Kiểm thử nghiệp vụ và giao diện
docs/CLASS_DIAGRAM.md       Đặc tả lớp/quan hệ để sinh XML draw.io
docs/TESTING.md             Test cases và tiêu chí nghiệm thu
```

Mỗi `.cpp` có include guard và có thể chứa cả khai báo lẫn định nghĩa lớp,
tương tự bài mẫu. `main.cpp` include cây nguồn; CMake chỉ biên dịch file đầu vào
của từng executable. Không thêm `.cpp` đã include vào danh sách biên dịch riêng,
vì sẽ gây trùng định nghĩa.

Các điểm chỉnh theo mẫu: constructor và hành vi ngắn trong class, initializer
list trực tiếp, chuỗi `const string&`, constructor con gọi rõ constructor cha,
`virtual/override` và `try/catch` có mục đích cụ thể. Giữ tên lớp/hàm và ID hiện tại
để không làm đứt luồng UI hoặc dữ liệu cũ.

## 7. Lưu trữ file

Mỗi dòng TXT là một bản ghi UTF-8; cột ngăn bằng `|`. Escape ký tự đặc biệt:
`|` → `\|`, `\` → `\\`, xuống dòng → `\n`, carriage return → `\r`.
Tiền lưu số nguyên, bool `0/1`, ngày giờ `dd/MM/yyyy HH:mm`.

| File | Nội dung |
| --- | --- |
| `users.txt` | Tài khoản, salt/hash, vai trò, trạng thái, ID khách |
| `customers.txt` | Hồ sơ và tuổi khách hàng |
| `halls.txt` | Sảnh, sức chứa, giá, trạng thái |
| `services.txt` | Mã/tên dịch vụ, đơn vị, giá, nhóm |
| `requests.txt` | Yêu cầu và các nhóm dòng dịch vụ theo sau |
| `bookings.txt` | Booking, dự toán, giảm giá, trạng thái, hạn giữ chỗ |
| `payments.txt` | Khoản chờ thu/đã thu |
| `events.txt` | Liên kết sự kiện, thời gian, trạng thái, ghi chú, loại để phục hồi lớp con |

Chỉ tạo mẫu khi chưa có bộ TXT. Nếu đã có một phần bộ dữ liệu mà thiếu file,
sai cột/ID/liên kết thì báo lỗi. Bộ TXT rỗng hợp lệ không bị tự thêm mẫu lại.
Khi lưu, ghi file tạm và sao lưu bản trước vào `backups/previous/` trước khi thay
file chính. Cơ chế nhiều file hiện tại dành cho một tiến trình, chưa phải giao
dịch chống mất điện giữa lúc ghi. Nếu cần, khôi phục cả bộ file từ bản sao lưu.
Lần refactor này không đổi schema TXT.

## 8. Build, chạy và kiểm thử

Yêu cầu C++17, CMake, Qt 6 Widgets. Preset dành cho MSYS2 UCRT64 tại
`C:/msys64/ucrt64`; máy khác cần chỉnh đường dẫn trong `CMakePresets.json`
hoặc cấu hình compiler và `CMAKE_PREFIX_PATH` tương ứng.

```powershell
cmake --preset qt-ucrt64
cmake --build --preset qt-ucrt64 -j 2
ctest --test-dir build/qt-ucrt64 --output-on-failure
.\build\qt-ucrt64\trongdongpalace.exe
```

Kiểm tra dữ liệu không mở cửa sổ:

```powershell
.\build\qt-ucrt64\trongdongpalace.exe --check-data
```

CMake gắn đường dẫn thư mục dự án vào executable; chuyển máy cần build lại.
Tài khoản mẫu: `admin`, `kinhdoanh`, `vanhanh`, `khachhang`; mật khẩu mẫu `123456`.
Tài khoản trong dữ liệu đang dùng có thể đã được chỉnh sửa.

`core` kiểm tra nghiệp vụ, TXT, đa hình và ngoại lệ; `guest_ui` chạy Qt offscreen
để kiểm tra khách vãng lai tạo yêu cầu. Test sử dụng thư mục tạm, không sửa
`data/` thật. Chi tiết tại [docs/TESTING.md](docs/TESTING.md).

## 9. Mô tả biểu đồ lớp để tạo draw.io

Đặc tả tại [docs/CLASS_DIAGRAM.md](docs/CLASS_DIAGRAM.md) gồm danh sách lớp,
thuộc tính/phương thức, mức truy cập, kiểu dữ liệu, quan hệ, bội số, sơ đồ
Mermaid và prompt để sinh file XML `.drawio`.

Tách ba trang: **mô hình nghiệp vụ**, **UI–service–storage**, **cây ngoại lệ**.
Các lớp chưa hiện thực không đưa vào biểu đồ của phiên bản hiện tại.
