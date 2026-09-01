Được. Mình sẽ **gộp kế hoạch 8 tuần ban đầu + yêu cầu SQLite cố định** thành một kế hoạch duy nhất. Mình cũng chỉnh lại thứ tự để **SQLite được dựng từ tuần 1 và mỗi module hoàn thành là phải lưu được xuống SQLite ngay**, không để persistence dồn về cuối.

Kế hoạch này phù hợp với mục tiêu của bạn: **đồ án OOP vừa đủ, code dễ hiểu, đầy đủ requirement, không over-engineering**. Các module và business flow được giữ theo Functional Requirements bạn đã gửi.

# KẾ HOẠCH 8 TUẦN — TRỐNG ĐỒNG PALACE OOP

## 🎯 Mục tiêu cuối ngày 56

Source code phải có:

```text
C++ Console Application
        │
        ├── Authentication
        ├── Authorization
        │
        ├── Customer
        ├── Hall
        ├── Event
        ├── Booking
        ├── Availability
        ├── Package
        ├── Service
        ├── Quotation
        ├── Contract
        ├── Payment
        ├── Operations
        ├── Event Completion
        └── Reports
                │
                ▼
        Repository Layer
                │
                ▼
          SQLite Database
```

Về OOP:

```text
Encapsulation
Abstraction
Inheritance
Polymorphism
Exception Handling
```

Trong đó `PaymentMethod` là phần chính để thể hiện inheritance/polymorphism, phù hợp với tài liệu OOP bạn gửi.

---

# 🗓️ WEEK 1 — FOUNDATION + SQLITE

### Mục tiêu

Cuối tuần phải có:

```text
✓ Project build được
✓ SQLite hoạt động
✓ Database schema
✓ Models cơ bản
✓ Exception hierarchy
✓ Repository structure
✓ Console skeleton
```

---

## DAY 1 — Phân tích Requirements

### Làm

Đọc lại toàn bộ:

- Functional Requirements
    
- OOP / Inheritance / Polymorphism / Exception Handling
    

Lập checklist:

```text
[ ] Authentication
[ ] Customer
[ ] Hall
[ ] Event
[ ] Booking
[ ] Availability
[ ] Package
[ ] Service
[ ] Quotation
[ ] Contract
[ ] Payment
[ ] Operations
[ ] Event Completion
[ ] Reports
[ ] SQLite
[ ] Exception Handling
[ ] Authorization
```

Xác định flow chính:

```text
Login
 ↓
Customer
 ↓
Event
 ↓
Hall
 ↓
Booking
 ↓
Package / Service
 ↓
Quotation
 ↓
Contract
 ↓
Payment
 ↓
Operations
 ↓
Complete Event
 ↓
Report
```

### Output

Tạo:

```text
PROJECT_CHECKLIST.md
```

---

# DAY 2 — Project Setup + SQLite

Tạo project C++.

Cấu trúc:

```text
src/
├── models/
├── services/
├── repositories/
├── exceptions/
├── database/
├── ui/
├── utils/
└── main.cpp
```

Thiết lập SQLite.

Database:

```text
trong_dong_palace.db
```

Tạo:

```text
DatabaseManager
```

Có khả năng:

```text
open()
close()
execute()
```

### Cuối ngày

Chương trình chạy được:

```text
Database connected successfully.
```

---

# DAY 3 — Database Schema

Thiết kế SQLite tables:

```text
users
customers
centers
halls
events
bookings
packages
services
package_services
booking_services
quotations
quotation_items
contracts
payments
staff_assignments
special_requests
incidents
```

Thiết lập các foreign key quan trọng:

```text
events.customer_id
bookings.event_id
bookings.hall_id
quotations.booking_id
contracts.quotation_id
payments.contract_id
```

Tạo:

```text
DatabaseInitializer
```

để:

```text
CREATE TABLE IF NOT EXISTS
```

---

# DAY 4 — Domain Models

Tạo:

```text
User
Customer
Center
Hall
Event
Booking
Package
Service
Quotation
Contract
Payment
StaffAssignment
SpecialRequest
Incident
```

Chỉ cần:

```text
private attributes
constructor
getter
setter
```

Không thêm abstraction thừa.

---

# DAY 5 — Exception Hierarchy

Tạo:

```text
AppException
├── ValidationException
├── EntityNotFoundException
├── DatabaseException
└── BusinessException
      ├── BookingConflictException
      └── PaymentException
```

Test:

```cpp
throw ValidationException("Invalid customer name");
```

Catch ở UI/main.

---

# DAY 6 — Repository Foundation

Tạo Repository:

```text
CustomerRepository
HallRepository
EventRepository
BookingRepository
PackageRepository
ServiceRepository
QuotationRepository
ContractRepository
PaymentRepository
```

Không cần interface cho mọi thứ nếu không cần.

Mỗi repository bắt đầu có:

```text
save()
findById()
findAll()
update()
delete()
```

---

# DAY 7 — Foundation Test

Test:

```text
✓ Build
✓ SQLite connection
✓ Create tables
✓ Insert test record
✓ Read test record
✓ Exception works
✓ Repository works
```

### 🚨 Milestone 1

Cuối Day 7 phải có:

> **C++ + SQLite + Models + Repository + Exception chạy được.**

---

# 🟩 WEEK 2 — CUSTOMER + HALL

## DAY 8 — Customer Repository + SQLite

Implement:

```text
CustomerRepository
```

SQL:

```text
INSERT
SELECT
UPDATE
DELETE
```

Test dữ liệu thực sự nằm trong:

```text
trong_dong_palace.db
```

---

## DAY 9 — CustomerService

Implement:

```text
createCustomer()
getAllCustomers()
searchCustomer()
updateCustomer()
deleteCustomer()
```

Validation:

```text
ID không trùng
Name không rỗng
Phone hợp lệ
```

---

## DAY 10 — Customer Console

Menu:

```text
1. Create Customer
2. View Customers
3. Search Customer
4. Update Customer
5. Delete Customer
6. Back
```

---

## DAY 11 — Customer Testing

Test:

```text
Create
Read
Search
Update
Delete
Duplicate ID
Invalid phone
Empty name
```

Restart app:

```text
Customer vẫn còn
```

### Customer DONE.

---

## DAY 12 — Hall Repository

Implement SQLite:

```text
save()
findById()
findAll()
update()
```

Hall:

```text
Hall ID
Hall Name
Center
Capacity
Status
```

---

## DAY 13 — HallService

Implement:

```text
createHall()
viewHalls()
updateHall()
checkAvailability()
```

Business rule:

```text
Hall phải Active mới được Booking.
```

---

## DAY 14 — Hall UI + Test

Test:

```text
Active Hall
Inactive Hall
Invalid capacity
Non-existing Hall
Persistence
```

### 🚨 Milestone 2

```text
Customer ✓
Hall ✓
SQLite ✓
CRUD ✓
Validation ✓
Exception ✓
```

---

# 🟨 WEEK 3 — EVENT + BOOKING + AVAILABILITY

Đây là tuần core của hệ thống.

---

## DAY 15 — Event Repository

SQLite CRUD:

```text
EventRepository
```

---

## DAY 16 — EventService

Implement:

```text
createEvent()
viewEvents()
searchEvent()
updateEvent()
```

Validation:

```text
Customer tồn tại
Guest Count > 0
Start < End
```

---

## DAY 17 — Event Console + Test

Menu:

```text
1. Create Event
2. View Events
3. Search Event
4. Update Event
5. Back
```

Test invalid input.

---

## DAY 18 — Booking Repository

Booking:

```text
Booking ID
Event ID
Hall ID
Start Time
End Time
Status
```

SQLite table:

```text
bookings
```

---

## DAY 19 — Booking Conflict

Implement:

```text
hasConflict()
```

Rule:

```text
New Start < Existing End
AND
New End > Existing Start
```

Test:

```text
18:00 - 22:00
20:00 - 23:00
→ CONFLICT
```

và:

```text
18:00 - 20:00
20:00 - 22:00
→ OK
```

---

## DAY 20 — BookingService

Flow:

```text
Event exists?
 ↓
Hall exists?
 ↓
Hall Active?
 ↓
Check Conflict
 ↓
Create Booking
 ↓
Save SQLite
```

Nếu conflict:

```text
BookingConflictException
```

---

## DAY 21 — Booking Console + Integration

Test flow:

```text
Create Customer
 ↓
Create Event
 ↓
Create Hall
 ↓
Create Booking
```

Sau đó:

```text
Create conflicting Booking
```

phải bị reject.

### 🚨 Milestone 3

Đến đây phải chạy được:

```text
Customer
   ↓
Event
   ↓
Hall
   ↓
Availability
   ↓
Booking
```

và toàn bộ dữ liệu được lưu bằng SQLite.

---

# 🟧 WEEK 4 — PACKAGE + SERVICE + QUOTATION

## DAY 22 — Package

Implement:

```text
Package
PackageRepository
PackageService
PackageMenu
```

SQLite CRUD.

---

## DAY 23 — Service

Implement:

```text
Service
ServiceRepository
ServiceService
ServiceMenu
```

SQLite CRUD.

---

## DAY 24 — Package ↔ Service

Implement:

```text
Add Service to Package
View Package Services
```

Database:

```text
package_services
```

Relationship:

```text
Package
 ├── Service
 ├── Service
 └── Service
```

---

## DAY 25 — Booking Services

Implement:

```text
Add Service
Remove Service
Change Quantity
```

Database:

```text
booking_services
```

Calculation:

```text
Quantity × Unit Price
```

---

## DAY 26 — Quotation Repository

Tạo:

```text
Quotation
QuotationRepository
QuotationItem
```

SQLite:

```text
quotations
quotation_items
```

---

## DAY 27 — QuotationService

Implement:

```text
calculateSubtotal()
calculateTax()
calculateTotal()
```

Công thức:

```text
Subtotal
= Package Amount + Service Amount

Total
= Subtotal - Discount + Tax
```

Không cho user nhập Total.

---

## DAY 28 — Quotation UI + Testing

Test:

```text
Package
Service
Quantity
Discount
Tax
Total
Recalculate
```

Restart application:

```text
Quotation vẫn tồn tại.
```

### 🚨 Milestone 4

```text
Booking
 ↓
Package
 ↓
Service
 ↓
Quotation
 ↓
Total
```

đã chạy trên SQLite.

---

# 🟥 WEEK 5 — CONTRACT + PAYMENT + OOP

Đây là tuần quan trọng nhất về OOP.

---

## DAY 29 — Contract

Implement:

```text
Contract
ContractRepository
ContractService
ContractMenu
```

Flow:

```text
Quotation
 ↓
Create Contract
 ↓
SQLite
```

---

## DAY 30 — Contract Status

Implement:

```text
Draft
Active
Completed
Cancelled
```

Validation đơn giản.

---

## DAY 31 — Payment Repository

Payment:

```text
Payment ID
Contract ID
Date
Amount
Method
Note
```

SQLite:

```text
payments
```

---

## DAY 32 — Payment Business Rules

Implement:

```text
Payment > Contract Total
→ Error

Total Paid > Contract Total
→ Error

Deposit > Contract Total
→ Error
```

Outstanding:

```text
Contract Total - Total Paid
```

---

# DAY 33 — Payment Polymorphism

Tạo:

```text
PaymentMethod
      ▲
      │
 ┌────┼──────────┐
 │    │          │
Cash  Bank       Card
```

Classes:

```text
CashPayment
BankTransferPayment
CardPayment
```

Base:

```cpp
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;

    virtual bool process(double amount) = 0;
    virtual std::string getName() const = 0;
};
```

Đây là phần để chứng minh:

```text
Abstraction
Inheritance
Polymorphism
```

---

# DAY 34 — PaymentService

Service làm việc với:

```text
PaymentMethod
```

không phụ thuộc trực tiếp vào:

```text
CashPayment
BankTransferPayment
CardPayment
```

Test cả 3 payment methods.

---

# DAY 35 — Payment Console + Integration

Flow:

```text
Quotation
 ↓
Contract
 ↓
Payment
 ↓
Outstanding
```

Test:

```text
Contract = 100M
Paid = 60M
Outstanding = 40M
```

Payment thêm 50M:

```text
→ Exception
```

### 🚨 Milestone 5

Đến đây bạn đã có phần OOP nổi bật:

```text
PaymentMethod
     ↓
Cash
Bank Transfer
Card
```

và business flow tài chính hoàn chỉnh.

---

# 🟪 WEEK 6 — AUTH + OPERATIONS + REPORT

## DAY 36 — Authentication

Implement:

```text
User
UserRepository
AuthService
Login
Logout
```

SQLite:

```text
users
```

---

## DAY 37 — Authorization

Role:

```text
Management
Sales
Accountant
Coordinator
Operations
Administrator
```

Kiểm tra quyền trước khi thực hiện action.

---

## DAY 38 — Operations

Implement:

```text
StaffAssignment
SpecialRequest
Incident
```

Các chức năng:

```text
Assign Staff
Add Special Request
Add Incident
Update Status
```

SQLite đầy đủ.

---

## DAY 39 — Event Completion

Implement:

```text
completeEvent()
```

Lưu:

```text
Actual Guest Count
Operation Note
Incident
Special Request Result
```

Status:

```text
Event → Completed
```

---

## DAY 40 — Booking Report

Implement:

```text
Booking Report
```

Filter:

```text
Date
Hall
Status
```

---

## DAY 41 — Event + Payment + Revenue Report

Implement:

```text
Event Report
Payment Report
Revenue Report
```

Revenue:

```text
Total Contract Value
Total Paid
Total Outstanding
```

---

## DAY 42 — Main Menu Integration

Ghép tất cả:

```text
1. Customer
2. Hall
3. Event
4. Booking
5. Package
6. Service
7. Quotation
8. Contract
9. Payment
10. Operations
11. Reports
12. Logout
```

### 🚨 Milestone 6

**Cuối Day 42 phải có tất cả feature chính.**

Từ thời điểm này:

> ❌ Không thêm feature lớn nữa.

---

# 🟫 WEEK 7 — FULL INTEGRATION + TESTING

Tuần này **không phát triển tính năng mới**.

---

## DAY 43 — Full End-to-End

Chạy:

```text
Login
 ↓
Customer
 ↓
Event
 ↓
Hall
 ↓
Booking
 ↓
Package
 ↓
Service
 ↓
Quotation
 ↓
Contract
 ↓
Payment
 ↓
Operations
 ↓
Complete Event
 ↓
Report
```

Mục tiêu:

> Chạy từ đầu đến cuối mà không cần sửa tay database.

---

# DAY 44 — SQLite Persistence

Test:

```text
Create data
 ↓
Exit
 ↓
Restart
 ↓
Load data
```

Kiểm tra tất cả entity chính.

---

# DAY 45 — Input Validation

Cố tình nhập:

```text
Empty
Negative number
Invalid date
Invalid time
Invalid ID
Duplicate ID
Invalid menu choice
```

Mục tiêu:

```text
Application không crash.
```

---

# DAY 46 — Exception Testing

Test:

```text
ValidationException
EntityNotFoundException
BookingConflictException
PaymentException
DatabaseException
```

Kiểm tra message:

```text
[ERROR]
The selected hall is already booked.
```

thay vì stack trace khó hiểu.

---

# DAY 47 — Authorization Testing

Test từng role:

```text
Admin
Sales
Accountant
Coordinator
Operations
Management
```

Kiểm tra:

```text
Có quyền
Không có quyền
```

---

# DAY 48 — CRUD Regression

Test toàn bộ module:

```text
Customer
Hall
Event
Booking
Package
Service
Quotation
Contract
Payment
```

---

# DAY 49 — Source Code Cleanup

Dọn:

```text
Debug cout
Unused variables
Unused imports
TODO
Duplicate code
Test code
Hard-coded temporary data
```

Kiểm tra:

```text
UI → Service → Repository → SQLite
```

Không để:

```text
UI → SQLite
```

---

# 🟦 WEEK 8 — FINALIZATION

Tuần cuối là **ổn định source**, không phải xây thêm hệ thống.

---

# DAY 50 — Architecture Review

Kiểm tra từng module:

```text
Model
 ↓
Service
 ↓
Repository
 ↓
SQLite
```

Service chứa business logic.

Repository chứa SQL.

UI xử lý input/output.

---

# DAY 51 — OOP Review

Chuẩn bị chỉ ra trong source:

### Encapsulation

```text
private attributes
public methods
```

### Abstraction

```text
PaymentMethod
Repository abstraction
```

### Inheritance

```text
PaymentMethod
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

### Polymorphism

```cpp
PaymentMethod*
```

### Exception

```text
AppException
├── ValidationException
├── BusinessException
├── ...
```

---

# DAY 52 — Business Rules Review

Kiểm tra lại:

```text
Hall Active
        ↓
Booking

Start < End
        ↓
Event

No Overlap
        ↓
Booking

Payment <= Contract Total
        ↓
Payment

Deposit <= Contract Total
        ↓
Payment
```

Các business rules phải được enforce ở service/business layer.

---

# DAY 53 — Console Polish

Chỉ chỉnh UI:

```text
==================================================
          TRONG DONG PALACE MANAGEMENT
==================================================
```

Thêm:

```text
[SUCCESS]
[ERROR]
[WARNING]
```

Menu dễ nhìn.

**Không dành thời gian làm GUI.**

---

# DAY 54 — Final Bug Fix

Chạy lại:

```text
Customer
Hall
Event
Booking
Package
Service
Quotation
Contract
Payment
Operations
Report
```

Ghi:

```text
BUG-001
BUG-002
BUG-003
```

Fix hết.

---

# DAY 55 — Demo Dataset

Tạo dữ liệu demo:

```text
Users
Customers
Centers
Halls
Packages
Services
Events
Bookings
Quotations
Contracts
Payments
Staff
```

Chuẩn bị một scenario hoàn chỉnh:

```text
Nguyen Van A
      ↓
Wedding Event
      ↓
Hall H001
      ↓
Wedding Premium
      ↓
Decoration + Sound + Lighting
      ↓
Quotation
      ↓
Contract
      ↓
Payment
      ↓
Operations
      ↓
Completed
      ↓
Revenue Report
```

---

# DAY 56 — FINAL RELEASE

Không code thêm.

Chỉ kiểm tra:

```text
[✓] Build
[✓] Run
[✓] SQLite database
[✓] All tables
[✓] CRUD
[✓] Validation
[✓] Exception
[✓] Authorization
[✓] OOP
[✓] E2E Flow
[✓] Reports
[✓] No debug code
[✓] README
[✓] Demo data
```

Đóng băng source:

```text
v1.0-final
```

---

# 📊 MILESTONE QUAN TRỌNG

|Ngày|Bắt buộc hoàn thành|
|--:|---|
|**7**|Project + SQLite + Models + Repository + Exception|
|**14**|Customer + Hall|
|**21**|Event + Booking + Availability|
|**28**|Package + Service + Quotation|
|**35**|Contract + Payment + Polymorphism|
|**42**|**Toàn bộ feature chính**|
|**49**|Full Integration + Testing|
|**56**|**FINAL SOURCE CODE**|

---

# 🧱 Cách chia task cho AI Code

Đây là phần mình khuyên bạn **rất nên tuân thủ**.

Không nói với AI:

> "Hãy code project Trống Đồng Palace hoàn chỉnh."

Thay vào đó, mỗi ngày đưa đúng task của ngày đó.

Ví dụ **Day 19**:

Hôm nay implement Booking Conflict Logic theo Functional Requirements.

Yêu cầu:

- Implement BookingRepository với SQLite.
    
- Implement logic kiểm tra conflict giữa các booking cùng Hall.
    
- Rule: New Start < Existing End AND New End > Existing Start.
    
- Booking Cancelled không được tính là conflict.
    
- Nếu conflict thì throw BookingConflictException.
    
- Không sửa các module không liên quan.
    
- Không thêm design pattern hoặc abstraction không cần thiết.
    
- Code phải đơn giản, dễ đọc và dễ giải thích trong đồ án OOP.
    
- Sau khi code xong, compile và test các trường hợp overlap/non-overlap.
    

Ví dụ **Day 33**:

Hôm nay implement Payment Polymorphism.

Yêu cầu:

- Tạo abstract class PaymentMethod.
    
- Tạo CashPayment, BankTransferPayment và CardPayment kế thừa PaymentMethod.
    
- PaymentMethod phải có virtual function process().
    
- PaymentService phải làm việc thông qua PaymentMethod abstraction.
    
- Demonstrate runtime polymorphism.
    
- Không tạo architecture phức tạp.
    
- Không thêm Factory/Strategy framework nếu không thực sự cần.
    
- Code phải phù hợp với đồ án OOP sinh viên và dễ giải thích khi bảo vệ.
    

Ví dụ **Day 43**:

Hôm nay không được thêm feature mới.

Hãy kiểm tra toàn bộ End-to-End Flow:

Login → Customer → Event → Hall → Booking → Package/Service → Quotation → Contract → Payment → Operations → Complete Event → Report.

Mục tiêu:

- Tìm lỗi integration.
    
- Sửa lỗi.
    
- Đảm bảo dữ liệu được lưu và đọc từ SQLite.
    
- Không refactor lớn.
    
- Không thêm design pattern.
    
- Không thay đổi Functional Requirements.
    
- Không thêm feature ngoài scope.
    

---

# 🔥 Quy tắc quản lý 8 tuần

Mình khuyên bạn ghi 4 trạng thái cho từng task:

```text
⬜ Not Started
🔵 In Progress
🟢 Done
🔴 Bug
```

Và đặc biệt:

### Ngày 1–42

```text
BUILD
```

### Ngày 43–49

```text
INTEGRATE + TEST
```

### Ngày 50–56

```text
STABILIZE + PREPARE DEMO
```

**Không để đến tuần 7 mới nghĩ đến SQLite.** SQLite phải đi cùng từng module ngay từ ngày module đó được xây dựng.

Với cách này, ngay cả khi bạn bị chậm khoảng **5–7 ngày**, bạn vẫn có một source code có thể chạy được từ giữa/cuối Week 6 và còn Week 7–8 để cứu deadline. Đây là phương án an toàn hơn nhiều cho một đồ án OOP 8 tuần.