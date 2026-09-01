Được. Mình sẽ thiết kế **mục 3, 4, 5** theo đúng project plan đã upload, đặc biệt bám vào các điểm tài liệu đã xác định: hệ thống phải thể hiện **Encapsulation, Abstraction, Inheritance, Polymorphism, Exception, SOLID**; kiến trúc gồm **GUI → Service → Domain → Repository → Persistence**; và tài liệu kế hoạch đã dự kiến framework exception gồm `AppException`, `ValidationException`, `BusinessException`, `EntityNotFoundException`, đồng thời sử dụng **Payment Polymorphism** làm một minh chứng OOP rõ ràng.

# 3. Kế thừa — Inheritance

## 3.1. Mục tiêu

Trong project Trống Đồng Palace, kế thừa được sử dụng để:

- Tái sử dụng thuộc tính/hành vi chung.
    
- Biểu diễn quan hệ **"is-a"** giữa các đối tượng.
    
- Tạo nền tảng cho đa hình.
    
- Giảm duplicate code.
    
- Phù hợp với yêu cầu môn OOP.
    

Tuy nhiên, **không nên lạm dụng inheritance**. Chỉ sử dụng khi thực sự có quan hệ cha–con về mặt nghiệp vụ.

---

# 3.2. Hierarchy 1 — Application Exception

Đây là hierarchy nên có vì nó phục vụ trực tiếp cho phần xử lý ngoại lệ.

```text
                    std::exception
                          │
                          ▼
                    AppException
                          │
             ┌────────────┴────────────┐
             │                         │
             ▼                         ▼
    ValidationException        BusinessException
                                       │
                         ┌─────────────┴─────────────┐
                         │                           │
                         ▼                           ▼
              BookingConflictException      PaymentException
```

Ngoài ra:

```text
AppException
     │
     └── EntityNotFoundException
```

Thiết kế này phù hợp trực tiếp với exception framework đã được xác định trong kế hoạch: `AppException`, `ValidationException`, `BusinessException`, `EntityNotFoundException`.

Ví dụ:

```cpp
class AppException : public std::exception {
protected:
    std::string message;

public:
    explicit AppException(const std::string& message)
        : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};
```

Sau đó:

```cpp
class ValidationException : public AppException {
public:
    explicit ValidationException(const std::string& message)
        : AppException(message) {}
};
```

```cpp
class BusinessException : public AppException {
public:
    explicit BusinessException(const std::string& message)
        : AppException(message) {}
};
```

```cpp
class EntityNotFoundException : public AppException {
public:
    explicit EntityNotFoundException(const std::string& message)
        : AppException(message) {}
};
```

Riêng Booking:

```cpp
class BookingConflictException : public BusinessException {
public:
    explicit BookingConflictException(const std::string& message)
        : BusinessException(message) {}
};
```

Payment:

```cpp
class PaymentException : public BusinessException {
public:
    explicit PaymentException(const std::string& message)
        : BusinessException(message) {}
};
```

---

# 3.3. Hierarchy 2 — Payment

Đây là **inheritance hierarchy chính của business domain** mà mình khuyến nghị dùng để thể hiện OOP.

Kế hoạch dự án đã định hướng:

```text
PaymentMethod
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

và xác định đây là evidence cho **inheritance + polymorphism**.

Mình sẽ chỉnh tên một chút để thiết kế rõ ràng hơn:

```text
                   PaymentMethod
                         ▲
                         │
          ┌──────────────┼──────────────┐
          │              │              │
          │              │              │
     CashPayment   BankTransferPayment  CardPayment
```

### Base class

```cpp
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;

    virtual bool process(double amount) = 0;
    virtual std::string getName() const = 0;
};
```

Đây là **abstract class**.

Nó không đại diện cho một phương thức thanh toán cụ thể.

Nó chỉ quy định:

> Mọi payment method phải biết cách `process()` và trả về tên phương thức.

---

## 3.4. CashPayment

```cpp
class CashPayment : public PaymentMethod {
public:
    bool process(double amount) override {
        // xử lý thanh toán tiền mặt
        return true;
    }

    std::string getName() const override {
        return "Cash";
    }
};
```

---

## 3.5. BankTransferPayment

```cpp
class BankTransferPayment : public PaymentMethod {
public:
    bool process(double amount) override {
        // xử lý mô phỏng chuyển khoản
        return true;
    }

    std::string getName() const override {
        return "Bank Transfer";
    }
};
```

---

## 3.6. CardPayment

```cpp
class CardPayment : public PaymentMethod {
public:
    bool process(double amount) override {
        // xử lý mô phỏng thanh toán thẻ
        return true;
    }

    std::string getName() const override {
        return "Card";
    }
};
```

### Lưu ý

Vì đây là project **C++ + SQLite**, chúng ta không thực sự kết nối ngân hàng hay payment gateway.

`process()` chỉ mô phỏng nghiệp vụ:

```text
Cash
→ xác nhận tiền mặt

Bank Transfer
→ xác nhận chuyển khoản

Card
→ xác nhận thanh toán thẻ
```

Điều này phù hợp với scope dự án vì online payment gateway nằm ngoài phạm vi MVP.

---

# 3.7. Payment Entity và PaymentMethod

Cần phân biệt:

```text
Payment
```

và

```text
PaymentMethod
```

### Payment

Là **transaction**:

```text
Payment
----------------------
paymentId
contractId
amount
paymentDate
note
```

### PaymentMethod

Là **cách thanh toán**:

```text
PaymentMethod
     │
     ├── Cash
     ├── Bank Transfer
     └── Card
```

Quan hệ:

```text
Payment
   │
   │ uses
   ▼
PaymentMethod
```

Ví dụ:

```cpp
Payment payment(
    10000000,
    std::make_unique<BankTransferPayment>()
);
```

---

# 3.8. Tại sao không dùng inheritance cho Customer, Staff, Admin?

Không nên thiết kế kiểu:

```text
Person
 ├── Customer
 ├── Staff
 │    ├── Admin
 │    ├── Accountant
 │    └── Sales
```

trừ khi nghiệp vụ thực sự yêu cầu hành vi khác nhau.

Trong project hiện tại:

```text
User
 ├── username
 ├── password
 ├── role
 └── status
```

Role có thể quản lý bằng:

```cpp
enum class UserRole {
    ADMIN,
    SALES,
    ACCOUNTANT,
    COORDINATOR,
    OPERATIONS,
    MANAGEMENT
};
```

Điều này phù hợp hơn với permission matrix của dự án. Tài liệu kế hoạch xác định các role này là actor/role của hệ thống.

---

# 3.9. Kết luận phần kế thừa

Project sẽ có **2 hierarchy quan trọng**:

### Hierarchy A — Exception

```text
std::exception
       │
 AppException
       │
 ├── ValidationException
 ├── EntityNotFoundException
 └── BusinessException
          ├── BookingConflictException
          └── PaymentException
```

### Hierarchy B — Payment

```text
PaymentMethod
      │
 ├────┼───────────┐
 ▼    ▼           ▼
Cash  Bank       Card
```

Như vậy vừa có inheritance ở **technical layer**, vừa có inheritance ở **business/domain layer**.

---

# 4. Đa hình — Polymorphism

## 4.1. Mục tiêu

Đa hình được sử dụng để cho phép chương trình thao tác với nhiều loại object khác nhau thông qua **base class/interface**, trong khi hành vi thực tế được quyết định bởi subclass.

Trong project này, **PaymentMethod là ví dụ chính**.

---

# 4.2. Runtime Polymorphism

Base class:

```cpp
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;

    virtual bool process(double amount) = 0;
    virtual std::string getName() const = 0;
};
```

Các subclass:

```cpp
CashPayment
BankTransferPayment
CardPayment
```

Khi đó:

```cpp
std::unique_ptr<PaymentMethod> method;

method = std::make_unique<CashPayment>();
method->process(50000000);

method = std::make_unique<BankTransferPayment>();
method->process(50000000);

method = std::make_unique<CardPayment>();
method->process(50000000);
```

Cùng gọi:

```cpp
method->process(...)
```

nhưng behavior khác nhau.

Đây chính là **runtime polymorphism**.

---

# 4.3. Polymorphism trong PaymentService

`PaymentService` không cần biết cụ thể object là:

```text
CashPayment
BankTransferPayment
CardPayment
```

Nó chỉ cần biết:

```text
PaymentMethod
```

Ví dụ:

```cpp
class PaymentService {
public:
    void makePayment(
        Contract& contract,
        double amount,
        PaymentMethod& method
    ) {
        if (amount <= 0) {
            throw ValidationException(
                "Payment amount must be greater than zero."
            );
        }

        if (amount > contract.getOutstanding()) {
            throw PaymentException(
                "Payment exceeds outstanding amount."
            );
        }

        if (!method.process(amount)) {
            throw PaymentException(
                "Payment processing failed."
            );
        }

        // save payment
    }
};
```

Nhìn vào code này:

```cpp
method.process(amount);
```

`PaymentService` không cần:

```cpp
if (method == CASH)
```

hay:

```cpp
if (method == BANK_TRANSFER)
```

Đây là lợi ích rất rõ của polymorphism.

---

# 4.4. Flow thực tế

Ví dụ người dùng chọn:

```text
Payment Method:
[ Bank Transfer ▼ ]
```

Console/GUI:

```text
          PaymentMenu
               │
               ▼
        PaymentService
               │
               ▼
     PaymentMethod pointer
               │
               ▼
     BankTransferPayment
               │
               ▼
           process()
               │
               ▼
         Save Payment
               │
               ▼
            SQLite
```

Nếu người dùng chọn Cash:

```text
PaymentMenu
     ↓
PaymentService
     ↓
PaymentMethod
     ↓
CashPayment
     ↓
process()
```

**Service không thay đổi.**

Chỉ object được inject vào thay đổi.

---

# 4.5. Polymorphism và Repository

Ngoài Payment, Repository cũng có thể sử dụng polymorphism.

Ví dụ:

```cpp
class ICustomerRepository {
public:
    virtual ~ICustomerRepository() = default;

    virtual void save(const Customer&) = 0;
    virtual Customer findById(int id) = 0;
};
```

Implementation:

```text
ICustomerRepository
        ▲
        │
   ┌────┴─────────────┐
   │                  │
SQLiteCustomerRepo   MockCustomerRepo
```

Trong production:

```cpp
ICustomerRepository& repo = sqliteRepository;
```

Trong testing:

```cpp
ICustomerRepository& repo = mockRepository;
```

Điều này đặc biệt hữu ích khi viết unit test.

Tài liệu kiến trúc dự án cũng định hướng Repository interface như `ICustomerRepo`, `IHallRepo`, `IBookingRepo`, `IPaymentRepo`, tách khỏi persistence SQLite/JSON.

---

# 4.6. Polymorphism trong kiến trúc tổng thể

```text
                GUI
                 │
                 ▼
            PaymentService
                 │
                 ▼
          PaymentMethod*
                 │
       ┌─────────┼─────────┐
       ▼         ▼         ▼
     Cash      Bank       Card
       │         │         │
       └─────────┼─────────┘
                 ▼
              Payment
                 │
                 ▼
        PaymentRepository
                 │
                 ▼
              SQLite
```

Đây là diagram rất đẹp để đưa vào phần OOP của report.

---

# 4.7. Compile-time polymorphism

Ngoài runtime polymorphism, C++ còn có **compile-time polymorphism**, ví dụ function overloading.

Project có thể sử dụng một cách vừa phải:

```cpp
class ReportService {
public:
    void generateReport(int year);

    void generateReport(
        int year,
        int month
    );

    void generateReport(
        const std::string& from,
        const std::string& to
    );
};
```

Ví dụ:

```cpp
generateReport(2026);

generateReport(2026, 8);

generateReport("2026-08-01", "2026-08-31");
```

Tuy nhiên, trong report mình sẽ nhấn mạnh:

> **Runtime polymorphism thông qua `PaymentMethod` là ví dụ polymorphism chính của hệ thống.**

Nó dễ chứng minh hơn và có giá trị thiết kế rõ ràng hơn.

---

# 5. Xử lý ngoại lệ — Exception Handling

Đây là phần mình muốn thiết kế khá kỹ vì nó liên quan trực tiếp đến **GUI/Console → Service → Repository → SQLite**.

Kế hoạch dự án đã xác định exception framework:

```text
AppException
ValidationException
BusinessException
EntityNotFoundException
```

và testing cũng có riêng **Exception Tests**.

---

# 5.1. Nguyên tắc

Exception được dùng để xử lý:

1. Input không hợp lệ.
    
2. Business rule bị vi phạm.
    
3. Entity không tồn tại.
    
4. Database/persistence failure.
    
5. Lỗi runtime không mong muốn.
    

Không nên dùng:

```cpp
return -1;
```

cho tất cả mọi lỗi.

Thay vào đó Service có thể:

```cpp
throw ValidationException(...);
```

hoặc:

```cpp
throw BookingConflictException(...);
```

---

# 5.2. Exception hierarchy

Mình đề xuất hoàn chỉnh như sau:

```text
                       std::exception
                             │
                             ▼
                       AppException
                             │
           ┌─────────────────┼──────────────────┐
           │                 │                  │
           ▼                 ▼                  ▼
 ValidationException   BusinessException   EntityNotFoundException
                             │
                   ┌─────────┴──────────┐
                   │                    │
                   ▼                    ▼
        BookingConflictException   PaymentException
```

Có thể bổ sung:

```text
AppException
    │
    └── DatabaseException
```

cho tầng Repository.

---

# 5.3. AppException

```cpp
class AppException : public std::exception {
protected:
    std::string message;

public:
    explicit AppException(
        const std::string& message
    )
        : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};
```

Đây là base exception của application.

---

# 5.4. ValidationException

Dùng khi **input không hợp lệ**.

Ví dụ:

```text
Guest count = -5
End time < Start time
Customer name empty
Payment amount <= 0
```

Code:

```cpp
throw ValidationException(
    "Guest count must be greater than zero."
);
```

---

# 5.5. EntityNotFoundException

Dùng khi không tìm thấy object.

Ví dụ:

```cpp
Customer customer =
    customerRepository.findById(customerId);
```

Nếu không tồn tại:

```cpp
throw EntityNotFoundException(
    "Customer not found."
);
```

Ví dụ khác:

```text
Hall H001 không tồn tại
Event E001 không tồn tại
Contract C001 không tồn tại
```

---

# 5.6. BusinessException

Dùng khi input có thể hợp lệ về mặt syntax nhưng **vi phạm business rule**.

Ví dụ:

```text
Booking conflict
Payment > Contract Total
Cancel completed event
Book inactive Hall
```

```cpp
throw BusinessException(
    "Hall is not available."
);
```

Nhưng tốt hơn là tạo exception cụ thể:

```cpp
throw BookingConflictException(
    "Hall H001 is already booked."
);
```

---

# 5.7. BookingConflictException

Đây là exception quan trọng nhất của Booking.

Business rule của dự án:

> Không được booking overlapping cùng Hall.

Kế hoạch cũng xác định rõ `BR-BOOK-001`: không được booking overlapping cùng Hall.

Flow:

```text
User creates Booking
        ↓
BookingService
        ↓
Check Hall
        ↓
Check Availability
        ↓
Conflict?
     /     \
   NO       YES
   ↓         ↓
 Save      throw
          BookingConflictException
```

---

# 5.8. PaymentException

Business rule:

```text
Total Paid <= Contract Total
```

Kế hoạch dự án cũng quy định không được thanh toán vượt quá Contract Total và Deposit không được vượt Total.

Ví dụ:

```cpp
if (amount > contract.getOutstanding()) {
    throw PaymentException(
        "Payment amount exceeds outstanding balance."
    );
}
```

---

# 5.9. DatabaseException

Đây là exception thuộc Persistence/Repository layer.

Ví dụ SQLite không mở được:

```cpp
throw DatabaseException(
    "Unable to open SQLite database."
);
```

Hoặc SQL execution fail:

```cpp
throw DatabaseException(
    "Failed to insert booking."
);
```

---

# 5.10. Exception flow toàn hệ thống

Đây là flow quan trọng nhất:

```text
┌───────────────┐
│    GUI        │
└───────┬───────┘
        │
        ▼
┌───────────────────┐
│ BookingService    │
└─────────┬─────────┘
          │
          │ conflict
          ▼
┌─────────────────────────────┐
│ BookingConflictException    │
└─────────────┬───────────────┘
              │
              │ throw
              ▼
┌───────────────────┐
│ GUI catch         │
└─────────┬─────────┘
          │
          ▼
┌─────────────────────────────┐
│ "Hall is unavailable."      │
└─────────────────────────────┘
```

---

# 5.11. Exception không nên xử lý ở Domain

Ví dụ không nên:

```cpp
class Booking {
public:
    void create() {
        try {
            // ...
        }
        catch (...) {
            // show GUI message
        }
    }
};
```

Domain không biết GUI.

Domain cũng không nên:

```cpp
std::cout << "Booking failed";
```

Vì architecture của project là:

```text
GUI
 ↓
Service
 ↓
Domain
 ↓
Repository
 ↓
SQLite
```

GUI mới là nơi quyết định **hiển thị lỗi thế nào**.

---

# 5.12. Service throw — GUI catch

Ví dụ:

```cpp
void BookingMenu::createBooking()
{
    try {
        bookingService.createBooking(request);

        std::cout
            << "Booking created successfully.\n";
    }
    catch (const ValidationException& ex) {
        std::cout
            << "Invalid input: "
            << ex.what()
            << '\n';
    }
    catch (const BookingConflictException& ex) {
        std::cout
            << "Booking conflict: "
            << ex.what()
            << '\n';
    }
    catch (const EntityNotFoundException& ex) {
        std::cout
            << "Not found: "
            << ex.what()
            << '\n';
    }
    catch (const AppException& ex) {
        std::cout
            << "Application error: "
            << ex.what()
            << '\n';
    }
}
```

Nếu sau này dùng Qt GUI:

```cpp
try {
    bookingService.createBooking(request);
}
catch (const BookingConflictException& ex) {
    QMessageBox::warning(
        this,
        "Booking Conflict",
        ex.what()
    );
}
```

**Business logic không đổi.**

Chỉ thay Presentation Layer.

Đây chính là lợi ích của kiến trúc đã chọn.

---

# 5.13. Exception tại Repository

Ví dụ:

```cpp
void BookingRepository::save(
    const Booking& booking
) {
    try {
        // SQLite INSERT
    }
    catch (const std::exception& ex) {
        throw DatabaseException(
            std::string("Failed to save booking: ")
            + ex.what()
        );
    }
}
```

Service:

```text
BookingService
      ↓
BookingRepository
      ↓
SQLite error
      ↓
DatabaseException
      ↑
      │
BookingService
      ↑
      │
GUI
```

---

# 5.14. Không catch `...` quá sớm

Không nên:

```cpp
try {
    bookingService.createBooking();
}
catch (...) {
    std::cout << "Error";
}
```

vì sẽ mất thông tin loại exception.

Nên:

```cpp
catch (const BookingConflictException& ex)
```

trước:

```cpp
catch (const BusinessException& ex)
```

sau đó:

```cpp
catch (const AppException& ex)
```

và cuối cùng nếu thực sự cần:

```cpp
catch (const std::exception& ex)
```

---

# 5.15. Bảng Exception Specification

|Exception|Khi nào xảy ra|Layer phát sinh|UI xử lý|
|---|---|---|---|
|`ValidationException`|Input sai|Service|Hiển thị validation message|
|`EntityNotFoundException`|Không tìm thấy entity|Service/Repository|Thông báo không tồn tại|
|`BookingConflictException`|Hall bị trùng lịch|BookingService|Thông báo conflict|
|`PaymentException`|Thanh toán vượt số dư|PaymentService|Thông báo payment error|
|`DatabaseException`|SQLite lỗi|Repository|Thông báo database error|
|`AppException`|Lỗi application chung|Any application layer|Generic error|

---

# 5.16. Ví dụ hoàn chỉnh — Create Booking

Đây là ví dụ rất tốt để đưa vào report.

### User

```text
Customer: Nguyễn Văn A
Hall: Hall A
Date: 20/09/2026
Start: 18:00
End: 22:00
```

### GUI

```text
Create Booking
      ↓
BookingService
```

### Service

```cpp
void BookingService::createBooking(
    const BookingRequest& request
) {
    if (request.start >= request.end) {
        throw ValidationException(
            "Start time must be before end time."
        );
    }

    if (!hallRepository.exists(request.hallId)) {
        throw EntityNotFoundException(
            "Hall does not exist."
        );
    }

    if (!hallRepository.isActive(request.hallId)) {
        throw BusinessException(
            "Hall is inactive."
        );
    }

    if (bookingRepository.hasConflict(
            request.hallId,
            request.start,
            request.end)) {

        throw BookingConflictException(
            "The selected hall is already booked."
        );
    }

    Booking booking(request);

    bookingRepository.save(booking);
}
```

### Nếu thành công

```text
Booking created successfully.
```

### Nếu conflict

```text
┌───────────────────────────────────┐
│ Booking Conflict                  │
├───────────────────────────────────┤
│ Hall A is already booked          │
│ during the selected time.         │
│                                   │
│              [ OK ]               │
└───────────────────────────────────┘
```

Đây chính là business rule `BR-BOOK-001` được hiện thực hóa thành code + exception.

---

# 5.17. Ví dụ hoàn chỉnh — Payment

```text
User
 ↓
Payment GUI
 ↓
PaymentService
 ↓
Validate amount
 ↓
Check outstanding
 ↓
PaymentMethod
 ↓
Cash / Bank / Card
 ↓
PaymentRepository
 ↓
SQLite
```

Nếu:

```text
Contract Total = 100M
Already Paid   = 80M
New Payment    = 30M
```

thì:

```text
30M > Outstanding 20M
```

→

```cpp
throw PaymentException(
    "Payment exceeds outstanding amount."
);
```

GUI hiển thị:

```text
Payment Error

Outstanding amount: 20,000,000
Payment entered:    30,000,000

Payment cannot be processed.
```

---

# 5.18. Tổng hợp 3 phần OOP

Cuối cùng, phần OOP của project có thể trình bày rất gọn như sau:

```text
                 OOP DESIGN
                     │
       ┌─────────────┼─────────────┐
       │             │             │
       ▼             ▼             ▼
  INHERITANCE    POLYMORPHISM   EXCEPTION
       │             │             │
       ▼             ▼             ▼
   PaymentMethod  PaymentMethod  AppException
   Exception      virtual        │
   hierarchy      functions      ├─ Validation
                                  ├─ Business
                                  ├─ NotFound
                                  └─ Database
```

## Cụ thể

### Inheritance

```text
PaymentMethod
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

và:

```text
AppException
├── ValidationException
├── BusinessException
│   ├── BookingConflictException
│   └── PaymentException
├── EntityNotFoundException
└── DatabaseException
```

### Polymorphism

```cpp
PaymentMethod* method;

method = new CashPayment();
method->process();

method = new BankTransferPayment();
method->process();

method = new CardPayment();
method->process();
```

### Exception

```text
GUI
 ↓
Service
 ↓
throw Exception
 ↓
GUI catch
 ↓
User-friendly error message
```

---

## Đề xuất chốt cho project

Mình sẽ **giữ Payment Method làm ví dụ chính cho Inheritance + Polymorphism**, thay vì cố nhồi inheritance vào `Customer/Staff/User`. Cách này vừa đúng OOP, vừa có business meaning, vừa dễ demo cho giảng viên. Kế hoạch gốc của project cũng đã định hướng Payment Polymorphism theo đúng hướng này.

**Bước tiếp theo hợp lý nhất là mình thiết kế luôn mục 6 — SOLID + Design Patterns**, đặc biệt chọn 2–3 pattern thực sự phù hợp với **C++ + SQLite + GUI**, chẳng hạn Repository, Strategy và Factory, rồi nối chúng trực tiếp vào Class Diagram hiện tại.