# Phân công nhiệm vụ — Trống Đồng Palace (bản dễ hiểu cho người mới học OOP)

## Ý chính

Mỗi người phụ trách 1 nhóm class riêng, nhưng **ai cũng phải tự làm đủ các kỹ thuật OOP** trong domain của mình (không phải chỉ 1 người làm kế thừa/đa hình, người khác chỉ làm CRUD). Vậy nên 3 phần dưới đây có **cùng 1 khung mục** để dễ so sánh.

Khung mục dùng chung:
1. Class của bạn là gì
2. Đóng gói (Class & Object) — bắt buộc
3. 1 biến/hàm static — bắt buộc
4. 1 hàm bạn (friend) — bắt buộc
5. 1 toán tử nạp chồng (operator overloading) — bắt buộc
6. Kế thừa (Inheritance) — bắt buộc
7. Đa hình (Polymorphism) — bắt buộc
8. Exception — bắt buộc
9. Đọc/ghi JSON — bắt buộc
10. Test nhanh

---

## Bảng ai làm gì (tổng quan)

| | Member 1 | Member 2 | Member 3 |
|---|---|---|---|
| Class chính | `Event`, `WeddingEvent`, `CorporateEvent`, `BirthdayEvent` | `Customer`, `Center`, `Hall`, `Package`, `Service` + 3 loại Service con | `Booking`, `Quotation`, `Contract`, `Payment` + 3 loại `PaymentMethod` |
| Kế thừa của bạn | `Event` → 3 loại sự kiện | `Service` → 3 loại dịch vụ | `PaymentMethod` → Cash/BankTransfer/Card |

---

## MEMBER 1 — Event

### 1. Class của bạn
`Event` (lớp cha, không tạo object trực tiếp được), và 3 lớp con: `WeddingEvent`, `CorporateEvent`, `BirthdayEvent`.

### 2. Đóng gói (Class & Object)
- Thuộc tính (id, ngày giờ, số khách...) để `private`.
- Có hàm `get...()` để đọc, `set...()` để sửa (không cho sửa trực tiếp từ bên ngoài).
- Có constructor (hàm tạo) để nhập dữ liệu ban đầu.
- Có destructor (`~Event()`), kể cả chỉ ghi `= default` cũng được, miễn hiểu nó dùng để làm gì.

### 3. Static (dùng chung cho cả lớp)
Đếm xem cả hệ thống đã tạo bao nhiêu Event:
```cpp
class Event {
    static int totalEvents;
public:
    static int getTotalEvents() { return totalEvents; }
};
```
Mỗi lần tạo Event mới thì `totalEvents++` trong constructor.

### 4. Hàm bạn (friend)
Viết 1 hàm không nằm trong class nhưng vẫn đọc được dữ liệu private của Event, dùng để in ra màn hình:
```cpp
friend ostream& operator<<(ostream& os, const Event& e);
```

### 5. Toán tử nạp chồng
- `operator<` : so sánh 2 Event theo ngày, để sắp xếp lịch.
- `operator==` : 2 Event bằng nhau nếu cùng `id`.

### 6. Kế thừa
```
Event (lớp cha)
 ├── WeddingEvent
 ├── CorporateEvent
 └── BirthdayEvent
```
Mỗi lớp con có thêm ít nhất 1 thuộc tính riêng (vd. Wedding có số bàn tiệc) để thấy rõ sự khác biệt, không phải kế thừa cho có.

### 7. Đa hình
```cpp
virtual double calculateBasePrice() const = 0; // hàm ảo, bắt buộc lớp con phải viết lại
```
Rồi thử:
```cpp
vector<shared_ptr<Event>> events;
for (auto& e : events) cout << e->calculateBasePrice();
```
Mỗi loại Event sẽ ra giá khác nhau dù gọi cùng 1 dòng code — đây chính là đa hình.

### 8. Exception
Khi giờ bắt đầu ≥ giờ kết thúc, hoặc số khách ≤ 0 → `throw ValidationException(...)`.

### 9. Đọc/ghi JSON
Viết `toJson()` và `fromJson()` cho Event, để `EventRepository` lưu/đọc file `data.json`.

### 10. Test nhanh
- Tạo Event hợp lệ → thành công, `totalEvents` tăng lên.
- Giờ bắt đầu sau giờ kết thúc → phải báo lỗi, không được crash.
- In 3 loại Event khác nhau ra màn hình → giá tiền phải khác nhau.

---

## MEMBER 2 — Customer, Center, Hall, Package, Service

### 1. Class của bạn
`Customer`, `Center`, `Hall`, `Package`, và `Service` (lớp cha) với 3 lớp con: `DecorationService`, `PhotographyService`, `MusicService`.

### 2. Đóng gói
Giống Member 1: thuộc tính `private`, có getter/setter, có constructor và destructor cho từng class.

### 3. Static
Đếm tổng số Hall trong hệ thống:
```cpp
class Hall {
    static int totalHalls;
public:
    static int getTotalHalls() { return totalHalls; }
};
```

### 4. Hàm bạn
Viết hàm kiểm tra 2 Customer có trùng số điện thoại không:
```cpp
friend bool isSamePhone(const Customer& a, const Customer& b);
```

### 5. Toán tử nạp chồng
- `operator==` cho `Customer`: 2 khách hàng bằng nhau nếu trùng `id`.
- `operator<<` cho `Hall`: in thông tin sảnh ra màn hình.

### 6. Kế thừa
```
Service (lớp cha)
 ├── DecorationService
 ├── PhotographyService
 └── MusicService
```
Mỗi loại Service tính giá khác nhau.

### 7. Đa hình
```cpp
virtual double calculatePrice() const = 0;
```
Test bằng `vector<shared_ptr<Service>>` rồi gọi `calculatePrice()` cho từng phần tử — kết quả phải khác nhau theo đúng loại.

### 8. Exception
- Tạo Customer/Hall trùng ID đã có → `DuplicateException`.
- Tìm ID không có → `NotFoundException`.
- Thiếu thông tin bắt buộc → `ValidationException`.

### 9. Đọc/ghi JSON
Mỗi class có Repository riêng (`CustomerRepository`, `HallRepository`...) tự đọc/ghi `data.json`, bắt lỗi khi file hỏng hoặc không tồn tại.

### 10. Test nhanh
- Tạo Customer trùng ID → phải báo lỗi.
- Hall đang "Inactive" thì không được dùng để đặt Booking.
- 3 loại Service cho ra giá khác nhau khi gọi `calculatePrice()`.

---

## MEMBER 3 — Booking, Quotation, Contract, Payment

### 1. Class của bạn
`Booking`, `Quotation`, `Contract`, `Payment`, và `PaymentMethod` (lớp cha) với 3 lớp con: `CashPayment`, `BankTransferPayment`, `CardPayment`.

### 2. Đóng gói
Giống 2 phần trên: thuộc tính `private`, getter/setter, constructor, destructor.

### 3. Static
Thuế áp dụng chung cho toàn hệ thống khi tính hoá đơn:
```cpp
class Quotation {
    static double taxRate; // ví dụ 0.1 = 10%
public:
    static void setTaxRate(double r) { taxRate = r; }
};
```

### 4. Hàm bạn
Đây là hàm quan trọng nhất của bạn — kiểm tra 2 Booking có bị trùng giờ không:
```cpp
friend bool hasTimeConflict(const Booking& a, const Booking& b) {
    return a.startTime < b.endTime && a.endTime > b.startTime;
}
```

### 5. Toán tử nạp chồng
- `operator+` cho `Payment`: cộng 2 khoản thanh toán lại để ra tổng đã trả.
- `operator<<` cho `Contract`: in thông tin hợp đồng.

### 6. Kế thừa
```
PaymentMethod (lớp cha)
 ├── CashPayment
 ├── BankTransferPayment
 └── CardPayment
```

### 7. Đa hình
```cpp
virtual void processPayment(double amount) = 0;
```
Test: cùng gọi `method->processPayment(100000)` nhưng với 3 object khác loại (Cash/BankTransfer/Card) → chạy 3 kiểu xử lý khác nhau.

### 8. Exception
- Booking bị trùng giờ với Booking khác cùng Hall → `BookingConflictException`.
- Giờ bắt đầu ≥ giờ kết thúc → `ValidationException`.
- Số tiền thanh toán âm, hoặc trả nhiều hơn còn nợ → `ValidationException`.

### 9. Đọc/ghi JSON
`BookingRepository`, `QuotationRepository`, `ContractRepository`, `PaymentRepository` — chỉ lưu ID để liên kết (không lưu con trỏ).

### 10. Test nhanh
- 2 Booking cùng Hall, giờ chồng nhau → phải báo lỗi conflict.
- Thanh toán âm tiền → phải báo lỗi.
- `Outstanding = Total - Paid` phải tính đúng, và trạng thái đúng (Unpaid / Partially Paid / Paid).

---

## Vài lưu ý chung cho cả 3 người

- Đừng throw lỗi ở màn hình Console — throw ở trong class/Service, Console chỉ `try/catch` để hiện thông báo và không bị crash.
- Đặt tên hàm Repository giống nhau: `load()`, `save()`, `add()`, `update()`, `remove()`, `findById()`, `getAll()` — để code của 3 người nhìn quen mắt như nhau.
- Nếu không chắc 1 khái niệm (vd. friend function) nên áp dụng thế nào trong phần của mình — cứ hỏi 2 bạn còn lại xem họ làm sao, vì cấu trúc bài đã được thiết kế giống nhau.
