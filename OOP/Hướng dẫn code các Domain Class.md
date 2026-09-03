Được, mình sẽ hướng dẫn từng bước — bạn tự gõ code vào file, mình chỉ giải thích và đưa code mẫu để bạn theo. Không dùng tool tạo file nào trong phần này.

## Quy tắc chung trước khi bắt đầu

**Vị trí file:** mỗi class một file header, đặt tại `src/models/TenClass.h`. Vì class chỉ có attribute + constructor + getter/setter (không có method phức tạp), ta viết **inline hết trong header** — không cần file `.cpp` riêng, không cần sửa `CMakeLists.txt`.

**Cấu trúc cố định cho mọi class:**

```text
#pragma once
#include <string>

[enum class nếu có]

class TenClass {
private:
    // attributes

public:
    // constructor
    // getters
    // setters
};
```

**2 quy tắc nhỏ mình sẽ áp dụng xuyên suốt (để bạn không phải tự hỏi mỗi lần):**

1. `id` **không có setter** — id là định danh, không nên đổi sau khi tạo. Các field "code" (`customerCode`, `hallCode`, ...) cũng vậy vì đóng vai trò tương tự id.
2. Chỉ dùng `enum class` khi tài liệu FR **liệt kê rõ các trạng thái** (VD: Hall Active/Inactive, Event Planned/Preparing/...). Những field status mà FR không định nghĩa rõ giá trị, mình để `std::string` — tránh tự bịa business rule.

---

## Bước 1 — `User` (ví dụ đầy đủ, giải thích từng phần)

Tạo file `src/models/User.h`:

```cpp
#pragma once

#include <string>

enum class UserRole {
    ADMIN,
    SALES,
    ACCOUNTANT,
    COORDINATOR,
    OPERATIONS,
    MANAGEMENT
};

enum class UserStatus {
    ACTIVE,
    INACTIVE
};

class User {
private:
    int id;
    std::string username;
    std::string passwordHash;
    UserRole role;
    UserStatus status;

public:
    User(int id,
         const std::string& username,
         const std::string& passwordHash,
         UserRole role,
         UserStatus status)
        : id(id),
          username(username),
          passwordHash(passwordHash),
          role(role),
          status(status) {}

    // Getters
    int getId() const { return id; }
    const std::string& getUsername() const { return username; }
    const std::string& getPasswordHash() const { return passwordHash; }
    UserRole getRole() const { return role; }
    UserStatus getStatus() const { return status; }

    // Setters (không có setId — xem quy tắc ở trên)
    void setUsername(const std::string& newUsername) { username = newUsername; }
    void setPasswordHash(const std::string& newPasswordHash) { passwordHash = newPasswordHash; }
    void setRole(UserRole newRole) { role = newRole; }
    void setStatus(UserStatus newStatus) { status = newStatus; }
};
```

**Giải thích từng phần:**

- `#pragma once` — header guard, tránh include file này 2 lần trong cùng 1 translation unit.
- `enum class` thay vì `enum` thường — để tránh xung đột tên (VD: nếu sau này có `HallStatus::ACTIVE` và `UserStatus::ACTIVE`, `enum class` bắt buộc phải viết `UserStatus::ACTIVE`, không bị đụng độ).
- Constructor dùng **initializer list** (`: id(id), username(username), ...`) thay vì gán trong thân hàm — đây là cách chuẩn trong C++, hiệu quả hơn vì không tạo object rồi gán lại.
- Getter trả về `const std::string&` (tham chiếu hằng) thay vì `std::string` (copy) — tránh copy chuỗi không cần thiết mỗi lần gọi getter. Với `int`/`enum`, trả bằng giá trị bình thường là đủ (không cần reference vì chúng nhỏ, copy rẻ).
- Getter có `const` ở cuối — nghĩa là hàm này không sửa object, cho phép gọi trên object `const`.
- Setter nhận tham số qua `const std::string&` — tránh copy chuỗi truyền vào trước khi gán.

Bạn gõ file này vào `src/models/User.h`, sau đó tiếp tục các class bên dưới theo đúng pattern này.

---

## Bước 2 — `Customer`

`src/models/Customer.h`:

```cpp
#pragma once

#include <string>

class Customer {
private:
    int id;
    std::string customerCode;
    std::string name;
    std::string phone;
    std::string email;
    std::string address;

public:
    Customer(int id,
             const std::string& customerCode,
             const std::string& name,
             const std::string& phone,
             const std::string& email,
             const std::string& address)
        : id(id),
          customerCode(customerCode),
          name(name),
          phone(phone),
          email(email),
          address(address) {}

    int getId() const { return id; }
    const std::string& getCustomerCode() const { return customerCode; }
    const std::string& getName() const { return name; }
    const std::string& getPhone() const { return phone; }
    const std::string& getEmail() const { return email; }
    const std::string& getAddress() const { return address; }

    // Không có setCustomerCode — customerCode đóng vai trò như id
    void setName(const std::string& newName) { name = newName; }
    void setPhone(const std::string& newPhone) { phone = newPhone; }
    void setEmail(const std::string& newEmail) { email = newEmail; }
    void setAddress(const std::string& newAddress) { address = newAddress; }
};
```

Class này không có enum — mọi field đều là dữ liệu tự do, không có trạng thái cố định.

---

## Bước 3 — `Center`

`src/models/Center.h`:

```cpp
#pragma once

#include <string>

class Center {
private:
    int id;
    std::string name;
    std::string address;
    std::string phone;

public:
    Center(int id,
           const std::string& name,
           const std::string& address,
           const std::string& phone)
        : id(id), name(name), address(address), phone(phone) {}

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getAddress() const { return address; }
    const std::string& getPhone() const { return phone; }

    void setName(const std::string& newName) { name = newName; }
    void setAddress(const std::string& newAddress) { address = newAddress; }
    void setPhone(const std::string& newPhone) { phone = newPhone; }
};
```

Class đơn giản nhất trong 14 class — dùng để luyện lại pattern cho quen tay.

---

## Bước 4 — `Hall`

FR-HALL-004 nói rõ Hall có 2 trạng thái `Active`/`Inactive` → dùng `enum class`.

`src/models/Hall.h`:

```cpp
#pragma once

#include <string>

enum class HallStatus {
    ACTIVE,
    INACTIVE
};

class Hall {
private:
    int id;
    int centerId;
    std::string hallCode;
    std::string name;
    int capacity;
    HallStatus status;

public:
    Hall(int id,
         int centerId,
         const std::string& hallCode,
         const std::string& name,
         int capacity,
         HallStatus status)
        : id(id),
          centerId(centerId),
          hallCode(hallCode),
          name(name),
          capacity(capacity),
          status(status) {}

    int getId() const { return id; }
    int getCenterId() const { return centerId; }
    const std::string& getHallCode() const { return hallCode; }
    const std::string& getName() const { return name; }
    int getCapacity() const { return capacity; }
    HallStatus getStatus() const { return status; }

    void setCenterId(int newCenterId) { centerId = newCenterId; }
    void setName(const std::string& newName) { name = newName; }
    void setCapacity(int newCapacity) { capacity = newCapacity; }
    void setStatus(HallStatus newStatus) { status = newStatus; }
};
```

---

## Bước 5 — `Event`

FR-EVENT-005 liệt kê rõ 5 trạng thái → `enum class`.

`src/models/Event.h`:

```cpp
#pragma once

#include <string>

enum class EventStatus {
    PLANNED,
    PREPARING,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

class Event {
private:
    int id;
    std::string eventCode;
    int customerId;
    std::string eventType;
    std::string eventDate;   // ISO format, VD: "2026-09-20"
    std::string startTime;   // VD: "18:00"
    std::string endTime;     // VD: "22:00"
    int guestCount;
    EventStatus status;

public:
    Event(int id,
          const std::string& eventCode,
          int customerId,
          const std::string& eventType,
          const std::string& eventDate,
          const std::string& startTime,
          const std::string& endTime,
          int guestCount,
          EventStatus status)
        : id(id),
          eventCode(eventCode),
          customerId(customerId),
          eventType(eventType),
          eventDate(eventDate),
          startTime(startTime),
          endTime(endTime),
          guestCount(guestCount),
          status(status) {}

    int getId() const { return id; }
    const std::string& getEventCode() const { return eventCode; }
    int getCustomerId() const { return customerId; }
    const std::string& getEventType() const { return eventType; }
    const std::string& getEventDate() const { return eventDate; }
    const std::string& getStartTime() const { return startTime; }
    const std::string& getEndTime() const { return endTime; }
    int getGuestCount() const { return guestCount; }
    EventStatus getStatus() const { return status; }

    void setCustomerId(int newCustomerId) { customerId = newCustomerId; }
    void setEventType(const std::string& newEventType) { eventType = newEventType; }
    void setEventDate(const std::string& newEventDate) { eventDate = newEventDate; }
    void setStartTime(const std::string& newStartTime) { startTime = newStartTime; }
    void setEndTime(const std::string& newEndTime) { endTime = newEndTime; }
    void setGuestCount(int newGuestCount) { guestCount = newGuestCount; }
    void setStatus(EventStatus newStatus) { status = newStatus; }
};
```

**Lưu ý:** mình lưu ngày/giờ dạng `std::string` (không dùng thư viện date phức tạp) — đúng tinh thần "keep it simple" của project. Validate `startTime < endTime` (BR-EVENT-001) sẽ nằm ở `EventService` sau này, **không nằm trong model này**.

---

## Bước 6 — `Booking`

`src/models/Booking.h`:

```cpp
#pragma once

#include <string>

enum class BookingStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Booking {
private:
    int id;
    std::string bookingCode;
    int eventId;
    int hallId;
    std::string startDateTime;
    std::string endDateTime;
    BookingStatus status;

public:
    Booking(int id,
            const std::string& bookingCode,
            int eventId,
            int hallId,
            const std::string& startDateTime,
            const std::string& endDateTime,
            BookingStatus status)
        : id(id),
          bookingCode(bookingCode),
          eventId(eventId),
          hallId(hallId),
          startDateTime(startDateTime),
          endDateTime(endDateTime),
          status(status) {}

    int getId() const { return id; }
    const std::string& getBookingCode() const { return bookingCode; }
    int getEventId() const { return eventId; }
    int getHallId() const { return hallId; }
    const std::string& getStartDateTime() const { return startDateTime; }
    const std::string& getEndDateTime() const { return endDateTime; }
    BookingStatus getStatus() const { return status; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setHallId(int newHallId) { hallId = newHallId; }
    void setStartDateTime(const std::string& newStartDateTime) { startDateTime = newStartDateTime; }
    void setEndDateTime(const std::string& newEndDateTime) { endDateTime = newEndDateTime; }
    void setStatus(BookingStatus newStatus) { status = newStatus; }
};
```

Logic check conflict (`overlaps()`) — theo tài liệu OOP, đây sẽ do `BookingService` xử lý (query các Booking khác trong DB), **không** đặt trong model này. Day 4 chỉ cần data holder.

---

## Bước 7 — `Package`

`src/models/Package.h`:

```cpp
#pragma once

#include <string>

enum class PackageStatus {
    ACTIVE,
    INACTIVE
};

class Package {
private:
    int id;
    std::string packageCode;
    std::string name;
    std::string description;
    double basePrice;
    PackageStatus status;

public:
    Package(int id,
            const std::string& packageCode,
            const std::string& name,
            const std::string& description,
            double basePrice,
            PackageStatus status)
        : id(id),
          packageCode(packageCode),
          name(name),
          description(description),
          basePrice(basePrice),
          status(status) {}

    int getId() const { return id; }
    const std::string& getPackageCode() const { return packageCode; }
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    double getBasePrice() const { return basePrice; }
    PackageStatus getStatus() const { return status; }

    void setName(const std::string& newName) { name = newName; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setBasePrice(double newBasePrice) { basePrice = newBasePrice; }
    void setStatus(PackageStatus newStatus) { status = newStatus; }
};
```

---

## Bước 8 — `Service` → đặt tên class là `ServiceItem`

**Điểm cần bạn lưu ý:** tài liệu thiết kế gốc (`Thiết kế các lớp với SQLite.md`) khuyến nghị đặt tên entity này là `ServiceItem` thay vì `Service`, vì `Service` rất dễ nhầm với các class tầng nghiệp vụ như `BookingService`, `PaymentService` (khác ý nghĩa hoàn toàn — một cái là domain entity, một cái là business logic class). Mình theo khuyến nghị đó. Nếu bạn muốn giữ đúng tên `Service` như đề bài liệt kê, chỉ cần đổi tên class lại — code còn lại giữ nguyên.

`src/models/ServiceItem.h`:

```cpp
#pragma once

#include <string>

enum class ServiceStatus {
    ACTIVE,
    INACTIVE
};

class ServiceItem {
private:
    int id;
    std::string serviceCode;
    std::string name;
    std::string description;
    std::string unit;
    double unitPrice;
    ServiceStatus status;

public:
    ServiceItem(int id,
                const std::string& serviceCode,
                const std::string& name,
                const std::string& description,
                const std::string& unit,
                double unitPrice,
                ServiceStatus status)
        : id(id),
          serviceCode(serviceCode),
          name(name),
          description(description),
          unit(unit),
          unitPrice(unitPrice),
          status(status) {}

    int getId() const { return id; }
    const std::string& getServiceCode() const { return serviceCode; }
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    const std::string& getUnit() const { return unit; }
    double getUnitPrice() const { return unitPrice; }
    ServiceStatus getStatus() const { return status; }

    void setName(const std::string& newName) { name = newName; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setUnit(const std::string& newUnit) { unit = newUnit; }
    void setUnitPrice(double newUnitPrice) { unitPrice = newUnitPrice; }
    void setStatus(ServiceStatus newStatus) { status = newStatus; }
};
```

---

## Bước 9 — `Quotation`

FR không định nghĩa rõ danh sách trạng thái Quotation (khác với Contract/Event/Booking) → mình để `status` là `std::string` thay vì bịa ra enum. Đây là ví dụ cho quy tắc số 2 ở đầu bài.

`src/models/Quotation.h`:

### Thiết kế enum

Vì FR không liệt kê sẵn, mình đề xuất một state machine tối thiểu bám theo đúng flow nghiệp vụ đã có trong tài liệu (`Quotation → Create Contract`, và FR-QUO-003 cho phép Update Quotation):

text

```text
DRAFT      — vừa tạo/đang chỉnh sửa (thêm/xóa Service, đổi Discount...)
CONFIRMED  — đã chốt, sẵn sàng dùng để tạo Contract
CONVERTED  — đã được dùng để tạo Contract (đánh dấu để không tạo trùng Contract từ 1 Quotation)
CANCELLED  — báo giá bị hủy
```

Đây là lựa chọn của mình vì FR không quy định — nếu bạn có ý khác (VD: không cần `CONFIRMED`, chỉ cần `DRAFT`/`CONVERTED`/`CANCELLED`), báo mình sửa, chỉ là đổi 1 dòng enum.

### Code chuẩn hóa lại — bạn gõ đè lên `src/models/Quotation.h`

cpp

```cpp
#pragma once

#include <string>

enum class QuotationStatus {
    DRAFT,
    CONFIRMED,
    CONVERTED,
    CANCELLED
};

class Quotation {
private:
    int id;
    std::string quotationCode;
    int bookingId;
    double subtotal;
    double discount;
    double tax;
    double total;
    QuotationStatus status;

public:
    Quotation(int id,
              const std::string& quotationCode,
              int bookingId,
              double subtotal,
              double discount,
              double tax,
              double total,
              QuotationStatus status)
        : id(id),
          quotationCode(quotationCode),
          bookingId(bookingId),
          subtotal(subtotal),
          discount(discount),
          tax(tax),
          total(total),
          status(status) {}

    int getId() const { return id; }
    const std::string& getQuotationCode() const { return quotationCode; }
    int getBookingId() const { return bookingId; }
    double getSubtotal() const { return subtotal; }
    double getDiscount() const { return discount; }
    double getTax() const { return tax; }
    double getTotal() const { return total; }
    QuotationStatus getStatus() const { return status; }

    void setBookingId(int newBookingId) { bookingId = newBookingId; }
    void setSubtotal(double newSubtotal) { subtotal = newSubtotal; }
    void setDiscount(double newDiscount) { discount = newDiscount; }
    void setTax(double newTax) { tax = newTax; }
    void setTotal(double newTotal) { total = newTotal; }
    void setStatus(QuotationStatus newStatus) { status = newStatus; }
};
```

**Điểm khác so với bản cũ:** chỉ có 2 chỗ đổi —

1. Thêm `enum class QuotationStatus { ... }` phía trên class.
2. Kiểu của `status` (attribute, tham số constructor, tham số `setStatus`, kiểu trả về `getStatus`) đổi từ `const std::string&` / `std::string` → `QuotationStatus`.
**Lưu ý quan trọng:** FR-QUO-002 nói `Total` phải do hệ thống tự tính, **không cho nhập tay**. Model này vẫn có `setTotal()` vì nó chỉ là data holder — việc "không cho nhập tay" là **business rule**, sẽ do `QuotationService` enforce (chỉ `QuotationService` được gọi `setTotal()` sau khi tự tính, Console/UI không được gọi trực tiếp). Model không tự chặn được việc này, đó là lý do Service Layer tồn tại.

---

## Bước 10 — `Contract`

FR-CON-003 liệt kê rõ 4 trạng thái → `enum class`.

`src/models/Contract.h`:

```cpp
#pragma once

#include <string>

enum class ContractStatus {
    DRAFT,
    ACTIVE,
    COMPLETED,
    CANCELLED
};

class Contract {
private:
    int id;
    std::string contractCode;
    int quotationId;
    int customerId;
    int eventId;
    double totalAmount;
    ContractStatus status;
    std::string contractDate;

public:
    Contract(int id,
             const std::string& contractCode,
             int quotationId,
             int customerId,
             int eventId,
             double totalAmount,
             ContractStatus status,
             const std::string& contractDate)
        : id(id),
          contractCode(contractCode),
          quotationId(quotationId),
          customerId(customerId),
          eventId(eventId),
          totalAmount(totalAmount),
          status(status),
          contractDate(contractDate) {}

    int getId() const { return id; }
    const std::string& getContractCode() const { return contractCode; }
    int getQuotationId() const { return quotationId; }
    int getCustomerId() const { return customerId; }
    int getEventId() const { return eventId; }
    double getTotalAmount() const { return totalAmount; }
    ContractStatus getStatus() const { return status; }
    const std::string& getContractDate() const { return contractDate; }

    void setQuotationId(int newQuotationId) { quotationId = newQuotationId; }
    void setCustomerId(int newCustomerId) { customerId = newCustomerId; }
    void setEventId(int newEventId) { eventId = newEventId; }
    void setTotalAmount(double newTotalAmount) { totalAmount = newTotalAmount; }
    void setStatus(ContractStatus newStatus) { status = newStatus; }
    void setContractDate(const std::string& newContractDate) { contractDate = newContractDate; }
};
```

---

## Bước 11 — `Payment`

**Điểm cần bạn lưu ý (quan trọng hơn Bước 8):** tài liệu OOP của bạn yêu cầu sau này có một **abstract class tên là `PaymentMethod`** (`process()`, `getName()`, các subclass `CashPayment`/`BankTransferPayment`/`CardPayment`) — đó là bài Step 12 "Payment + Polymorphism", **chưa làm ở Day 4**.

Nhưng `Payment` (bản ghi giao dịch) vẫn cần lưu **"giao dịch này dùng phương thức nào"** — đây là dữ liệu, khác với `PaymentMethod` (class xử lý _hành vi thanh toán_, sẽ có ở bước sau). Nếu mình đặt tên field này cũng là kiểu `PaymentMethod`, sau này sẽ đụng tên với abstract class → phải đổi tên. Vì vậy mình đặt tên enum là `PaymentMethodType` (chỉ dùng để ghi nhận "loại phương thức" trong bản ghi Payment), tách biệt khỏi `PaymentMethod` (class xử lý hành vi ở Step 12).

`src/models/Payment.h`:

```cpp
#pragma once

#include <string>

enum class PaymentMethodType {
    CASH,
    BANK_TRANSFER,
    CARD
};

class Payment {
private:
    int id;
    std::string paymentCode;
    int contractId;
    double amount;
    std::string paymentDate;
    PaymentMethodType method;
    std::string note;

public:
    Payment(int id,
            const std::string& paymentCode,
            int contractId,
            double amount,
            const std::string& paymentDate,
            PaymentMethodType method,
            const std::string& note)
        : id(id),
          paymentCode(paymentCode),
          contractId(contractId),
          amount(amount),
          paymentDate(paymentDate),
          method(method),
          note(note) {}

    int getId() const { return id; }
    const std::string& getPaymentCode() const { return paymentCode; }
    int getContractId() const { return contractId; }
    double getAmount() const { return amount; }
    const std::string& getPaymentDate() const { return paymentDate; }
    PaymentMethodType getMethod() const { return method; }
    const std::string& getNote() const { return note; }

    void setContractId(int newContractId) { contractId = newContractId; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setPaymentDate(const std::string& newPaymentDate) { paymentDate = newPaymentDate; }
    void setMethod(PaymentMethodType newMethod) { method = newMethod; }
    void setNote(const std::string& newNote) { note = newNote; }
};
```

---

## Bước 12 — `StaffAssignment`

`src/models/StaffAssignment.h`:

```cpp
#pragma once

#include <string>

class StaffAssignment {
private:
    int id;
    int eventId;
    int staffId;         // tham chiếu users.id — xem ghi chú Day 3
    std::string assignmentRole;
    std::string status;

public:
    StaffAssignment(int id,
                     int eventId,
                     int staffId,
                     const std::string& assignmentRole,
                     const std::string& status)
        : id(id),
          eventId(eventId),
          staffId(staffId),
          assignmentRole(assignmentRole),
          status(status) {}

    int getId() const { return id; }
    int getEventId() const { return eventId; }
    int getStaffId() const { return staffId; }
    const std::string& getAssignmentRole() const { return assignmentRole; }
    const std::string& getStatus() const { return status; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setStaffId(int newStaffId) { staffId = newStaffId; }
    void setAssignmentRole(const std::string& newAssignmentRole) { assignmentRole = newAssignmentRole; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
};
```

---

## Bước 13 — `SpecialRequest`

`src/models/SpecialRequest.h`:

```cpp
#pragma once

#include <string>

class SpecialRequest {
private:
    int id;
    int eventId;
    std::string description;
    std::string status;
    std::string note;

public:
    SpecialRequest(int id,
                    int eventId,
                    const std::string& description,
                    const std::string& status,
                    const std::string& note)
        : id(id),
          eventId(eventId),
          description(description),
          status(status),
          note(note) {}

    int getId() const { return id; }
    int getEventId() const { return eventId; }
    const std::string& getDescription() const { return description; }
    const std::string& getStatus() const { return status; }
    const std::string& getNote() const { return note; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
    void setNote(const std::string& newNote) { note = newNote; }
};
```

---

## Bước 14 — `Incident`

`src/models/Incident.h`:

```cpp
#pragma once

#include <string>

class Incident {
private:
    int id;
    int eventId;
    std::string description;
    std::string severity;
    std::string status;
    std::string resolution;

public:
    Incident(int id,
             int eventId,
             const std::string& description,
             const std::string& severity,
             const std::string& status,
             const std::string& resolution)
        : id(id),
          eventId(eventId),
          description(description),
          severity(severity),
          status(status),
          resolution(resolution) {}

    int getId() const { return id; }
    int getEventId() const { return eventId; }
    const std::string& getDescription() const { return description; }
    const std::string& getSeverity() const { return severity; }
    const std::string& getStatus() const { return status; }
    const std::string& getResolution() const { return resolution; }

    void setEventId(int newEventId) { eventId = newEventId; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    void setSeverity(const std::string& newSeverity) { severity = newSeverity; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
    void setResolution(const std::string& newResolution) { resolution = newResolution; }
};
```

---

## Sau khi gõ xong 14 file — tự kiểm tra

Đừng chỉ tin là code đúng — hãy tự build để chắc chắn. Cách nhanh nhất: tạm thời sửa `main.cpp` của bạn, include vài model và tạo thử object, ví dụ:

```cpp
#include "models/Customer.h"
#include "models/Hall.h"
// ...

int main() {
    Customer c(1, "C001", "Nguyen Van A", "0901234567", "a@example.com", "Ha Noi");
    Hall h(1, 1, "H001", "Palace 1", 500, HallStatus::ACTIVE);

    std::cout << c.getName() << std::endl;
    std::cout << h.getName() << " - capacity: " << h.getCapacity() << std::endl;

    return 0;
}
```

Build lại bằng `cmake --build build` (hoặc `cd build && make`). Nếu compile sạch, không lỗi header guard/thiếu include — 14 model đã đúng. Sau đó bạn nhớ **xóa đoạn test này khỏi `main.cpp`** trước khi làm bước tiếp theo, và tự tick vào `PROJECT_CHECKLIST.md` (mục "OOP Requirements Checklist → Encapsulation" coi như xong ở mức domain model).

Xong 14 class, hỏi mình để chuyển qua **Day 5 — Exceptions** khi bạn sẵn sàng.