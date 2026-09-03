Đúng. Nếu mục tiêu là **mỗi ngày biết chắc “task hôm nay đã hoàn thành chưa”**, thì kế hoạch nên có thêm **Definition of Done + Test Cases + Expected Result** cho **từng ngày**, chứ không chỉ ghi “implement”.

Mình đã đối chiếu lại với Functional Requirements: hệ thống là **C++ Console**, mỗi module có menu riêng, có input validation/error handling, dữ liệu phải được persistence/load, và kiến trúc là `Console → Service → Repository → Data Storage`.

Dưới đây là bản mình khuyên bạn **dùng làm checklist thực tế trong 56 ngày**.

---

# KẾ HOẠCH 8 TUẦN + TEST CHECKLIST

## Quy tắc test chung cho mọi ngày

Mỗi ngày sau khi code xong phải trả lời được 4 câu:

```text
1. Code có compile không?
2. Chức năng chính có chạy không?
3. Case lỗi có được xử lý không?
4. Nếu có database thì dữ liệu có thực sự lưu xuống SQLite không?
```

Và mỗi task có 3 mức:

```text
🟢 DONE
- Code chạy
- Happy case pass
- Error case pass
- SQLite persistence pass nếu task có DB

🟡 PARTIAL
- Code chạy nhưng còn test fail

🔴 NOT DONE
- Không compile / chức năng chính chưa chạy
```

**Không được chuyển sang task tiếp theo nếu task hiện tại chưa đạt 🟢 DONE.**

---

# 🟩 WEEK 1 — FOUNDATION + SQLITE + UI FOUNDATION

---

# DAY 1 — Requirements + Console Flow

### Task

Hoàn thành:

```text
PROJECT_CHECKLIST.md
UI_FLOW.md
```

Xác định các module:

```text
Authentication
Customer
Hall
Event
Booking
Availability
Package
Service
Quotation
Contract
Payment
Operations
Event Completion
Reports
Authorization
```

Functional Requirements cũng xác định đây là scope MVP chính của hệ thống.

### Test

Không cần code.

Kiểm tra checklist có đủ:

```text
[ ] Authentication
[ ] Authorization
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
[ ] Console UI
[ ] Exception
```

### 🟢 DONE khi

Bạn có thể mở `PROJECT_CHECKLIST.md` và không còn module nào trong requirement bị bỏ sót.

---

# DAY 2 — C++ Project + SQLite + UI Framework

### Task

Tạo:

```text
DatabaseManager
ConsoleUI
MainMenu
InputHelper
DisplayHelper
```

Database:

```text
trong_dong_palace.db
```

### Test 1 — Compile

```bash
g++ ...
```

Expected:

```text
Build successful
```

### Test 2 — SQLite

Chạy:

```text
Application started.
Database connected successfully.
```

### Test 3 — Main Menu

Expected:

```text
==================================================
        TRONG DONG PALACE MANAGEMENT
==================================================

1. Customer
2. Hall
3. Event
...
4. Exit
```

### Test 4 — Invalid menu

Nhập:

```text
99
```

Expected:

```text
[ERROR] Invalid choice.
Please try again.
```

### 🟢 DONE khi

```text
Compile ✓
SQLite connect ✓
Main menu ✓
Invalid input ✓
```

---

# DAY 3 — SQLite Schema

### Task

Tạo 17 tables:

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

Functional Requirements yêu cầu persistence cho các entity chính và Repository phải nằm giữa Service và data storage.

### Test 1

Start application.

Expected:

```text
Database initialized successfully.
```

### Test 2

Kiểm tra SQLite:

```sql
SELECT name FROM sqlite_master WHERE type='table';
```

Phải thấy 17 tables.

### Test 3

Restart application.

Expected:

```text
Không tạo lỗi
Không mất database
```

### 🟢 DONE khi

```text
17 tables ✓
Foreign keys ✓
Application restart ✓
```

---

# DAY 4 — Models

### Task

Tạo các model.

Ví dụ:

```text
Customer
Hall
Event
Booking
...
```

### Test

Tạo object:

```cpp
Customer c(...);
```

Sau đó:

```cpp
std::cout << c.getName();
```

Expected:

```text
Nguyen Van A
```

Test setter:

```cpp
c.setPhone("0901234567");
```

Expected:

```text
0901234567
```

### 🟢 DONE khi

Mỗi model:

```text
Constructor ✓
Getter ✓
Setter ✓
Compile ✓
```

Không cần database ngày này.

---

# DAY 5 — Exception

### Task

Tạo:

```text
AppException
ValidationException
EntityNotFoundException
DatabaseException
BusinessException
BookingConflictException
PaymentException
```

Exception hierarchy này phù hợp với thiết kế OOP đã xác định trong tài liệu.

### Test

```cpp
throw ValidationException("Invalid customer name");
```

Catch:

```cpp
catch (const AppException& e)
```

Expected:

```text
[ERROR] Invalid customer name
```

### Test thêm

```text
BookingConflictException
PaymentException
EntityNotFoundException
```

### 🟢 DONE khi

```text
Throw ✓
Catch base class ✓
Correct message ✓
Application không crash ✓
```

---

# DAY 6 — Repository Foundation

### Task

Tạo:

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

### Test

Chưa cần full CRUD.

Chỉ cần một repository thử nghiệm:

```text
CustomerRepository
    ↓
save()
    ↓
SQLite
```

Insert:

```text
C001
Nguyen Van A
```

Sau đó:

```text
findById("C001")
```

Expected:

```text
Customer found:
C001 - Nguyen Van A
```

### 🟢 DONE khi

Ít nhất một Repository chứng minh được:

```text
Repository → SQLite → Repository
```

và tất cả repository skeleton compile được.

---

# DAY 7 — Foundation Integration

### Test bắt buộc

Chạy:

```text
Application
 ↓
Main Menu
 ↓
Repository
 ↓
SQLite
 ↓
Exception
```

### Test case

|Test|Expected|
|---|---|
|Start app|Success|
|Connect DB|Success|
|Create table|Success|
|Insert record|Success|
|Read record|Success|
|Invalid input|Friendly error|
|Throw exception|Caught|

### 🟢 MILESTONE 1

```text
C++ ✓
SQLite ✓
17 Tables ✓
Models ✓
Repository ✓
Exception ✓
Console skeleton ✓
```

---

# 🟩 WEEK 2 — CUSTOMER + HALL

# DAY 8 — CustomerRepository

### Test CRUD

```text
Create C001
View C001
Update C001
Delete C001
```

Expected:

```text
Create → found
Update → new value
Delete → not found
```

### SQLite Test

Exit app → mở lại → kiểm tra Customer.

### 🟢 DONE

CRUD + SQLite persistence pass.

---

# DAY 9 — CustomerService

### Test valid

```text
ID: C001
Name: Nguyen Van A
Phone: 0901234567
```

Expected:

```text
Customer created successfully.
```

### Test invalid

```text
Name = ""
```

Expected:

```text
[ERROR] Customer name is required.
```

Duplicate:

```text
C001
```

Expected:

```text
[ERROR] Customer ID already exists.
```

FR-CUS yêu cầu validate thông tin, kiểm tra ID trùng và xử lý các trường hợp thiếu dữ liệu/số điện thoại không hợp lệ.

### 🟢 DONE

Happy + error + DB pass.

---

# DAY 10 — CustomerMenu

### Test menu

```text
1. Create
2. View
3. Search
4. Update
5. Delete
6. Back
```

Test từng lựa chọn.

### Test navigation

```text
MainMenu
 ↓
CustomerMenu
 ↓
Back
 ↓
MainMenu
```

Expected:

Không crash, quay đúng menu.

### 🟢 DONE

Customer UI gọi Service, **không gọi SQLite trực tiếp**.

---

# DAY 11 — Customer Full Test

Chạy scenario:

```text
Create C001
 ↓
View
 ↓
Search
 ↓
Update
 ↓
View
 ↓
Delete
 ↓
View
```

### Persistence test

```text
Create C001
Exit
Restart
View
```

Expected:

```text
C001 vẫn tồn tại.
```

### 🟢 MILESTONE CUSTOMER

---

# DAY 12 — HallRepository

Test:

```text
Create H001
View H001
Update H001
```

Kiểm tra SQLite.

---

# DAY 13 — HallService

### Test valid

```text
Capacity = 500
Status = Active
```

Expected:

```text
Hall created successfully.
```

### Test invalid

```text
Capacity = -100
```

Expected:

```text
[ERROR] Capacity must be greater than 0.
```

### Business rule

Inactive:

```text
Hall H001 = Inactive
```

Booking sau này phải bị reject.

Rule này được xác định trong requirements: Hall phải tồn tại và Active mới được dùng cho Booking.

### 🟢 DONE

---

# DAY 14 — Hall UI + Test

Test:

```text
Create
View
Update
Availability
Invalid capacity
Inactive Hall
```

### 🟢 MILESTONE 2

```text
Customer ✓
Customer UI ✓
Hall ✓
Hall UI ✓
SQLite ✓
Validation ✓
Exception ✓
```

---

# 🟨 WEEK 3 — EVENT + BOOKING + AVAILABILITY

# DAY 15 — EventRepository

Test:

```text
Create Event
Read Event
Update Event
```

SQLite verification.

---

# DAY 16 — EventService

### Test valid

```text
Start = 18:00
End   = 22:00
Guest = 300
```

Expected:

```text
Event created successfully.
```

### Test invalid

```text
Start = 22:00
End   = 18:00
```

Expected:

```text
[ERROR]
Start time must be before End time.
```

Requirements xác định `Event Start Time < End Time`.

---

# DAY 17 — Event UI

Test:

```text
Create
View
Search
Update
Back
```

### 🟢 DONE khi

Toàn bộ thao tác thông qua:

```text
EventMenu
 ↓
EventService
 ↓
EventRepository
 ↓
SQLite
```

---

# DAY 18 — BookingRepository

Test:

```text
Create B001
Find B001
Cancel B001
```

SQLite check.

---

# DAY 19 — Conflict Logic

Đây là ngày phải test kỹ nhất.

### Test 1 — Overlap

Existing:

```text
18:00 → 22:00
```

New:

```text
20:00 → 23:00
```

Expected:

```text
CONFLICT
```

### Test 2

```text
16:00 → 19:00
```

Expected:

```text
CONFLICT
```

### Test 3

```text
22:00 → 23:00
```

Expected:

```text
NO CONFLICT
```

### Test 4

```text
14:00 → 18:00
```

Expected:

```text
NO CONFLICT
```

### Test 5 — Cancelled

Existing:

```text
18:00 → 22:00
Status = Cancelled
```

New:

```text
20:00 → 23:00
```

Expected:

```text
NO CONFLICT
```

### 🟢 DONE

Tất cả 5 cases pass.

---

# DAY 20 — BookingService

Test flow:

```text
Event tồn tại ✓
Hall tồn tại ✓
Hall Active ✓
Conflict check ✓
Save ✓
```

### Test lỗi

```text
Invalid Event
Invalid Hall
Inactive Hall
Conflict
Invalid time
```

Mỗi lỗi phải tạo exception phù hợp.

---

# DAY 21 — Booking UI

### Full console test

```text
Create Booking
View Booking
Search Booking
Cancel Booking
Check Availability
```

### E2E

```text
Customer
 ↓
Event
 ↓
Hall
 ↓
Booking
```

### 🟢 MILESTONE 3

---

# 🟧 WEEK 4 — PACKAGE + SERVICE + QUOTATION

# DAY 22 — Package

### Test

```text
Create Package
View Package
Update Package
Delete Package
```

SQLite persistence.

---

# DAY 23 — Service

### Test

```text
Create Service
View Service
Update Service
Delete Service
```

Test price:

```text
Unit Price = -1000
```

Expected:

```text
[ERROR] Unit price must be valid.
```

---

# DAY 24 — Package ↔ Service

Test:

```text
Package P001
 ↓
Add Decoration
 ↓
Add Sound
 ↓
Add Lighting
```

View:

```text
Wedding Premium
 ├── Decoration
 ├── Sound
 └── Lighting
```

Kiểm tra `package_services`.

---

# DAY 25 — Booking Services

Test:

```text
Booking B001
 ↓
Add Sound x2
```

Expected:

```text
Amount = 2 × UnitPrice
```

Test:

```text
Quantity = 0
Quantity = -1
```

Expected:

```text
[ERROR]
Quantity must be greater than 0.
```

Requirements xác định Service trong Booking được tính theo `Quantity × Unit Price`.

---

# DAY 26 — Quotation Repository

Test:

```text
Create Q001
Add Item
View Q001
```

SQLite:

```text
quotations
quotation_items
```

phải có record.

---

# DAY 27 — Quotation Calculation

Dùng dữ liệu:

```text
Package = 8,000,000
Decoration = 5,000,000
Sound = 2,000,000
Lighting = 1,500,000
```

Expected:

```text
Subtotal = 16,500,000
```

Discount:

```text
1,000,000
```

Tax:

```text
1,550,000
```

Expected:

```text
Total = 17,050,000
```

Đây đúng công thức requirements:

```text
Subtotal = Package Amount + Service Amount
Total = Subtotal - Discount + Tax
```

### Test quan trọng

User nhập:

```text
Total = 1,000,000
```

Không được chấp nhận.

Total phải được **system tự tính**.

### 🟢 DONE

---

# DAY 28 — Quotation UI + Integration

Test:

```text
Booking
 ↓
Package
 ↓
Service
 ↓
Quotation
 ↓
Calculate
 ↓
Display
```

Restart app:

```text
Quotation vẫn tồn tại.
```

### 🟢 MILESTONE 4

---

# 🟥 WEEK 5 — CONTRACT + PAYMENT + OOP

# DAY 29 — Contract

Test:

```text
Quotation Q001
 ↓
Create Contract
 ↓
View Contract
```

SQLite check.

---

# DAY 30 — Contract UI + Status

Test:

```text
Draft
 ↓
Active
 ↓
Completed
```

Test Cancel:

```text
Active
 ↓
Cancelled
```

Kiểm tra UI hiển thị đúng status.

---

# DAY 31 — PaymentRepository

Test:

```text
Create Payment
View Payment
Find by Contract
```

SQLite check.

---

# DAY 32 — Payment Rules

Contract:

```text
100,000,000
```

### Case 1

```text
Payment = 20M
```

Expected:

```text
Paid = 20M
Outstanding = 80M
```

### Case 2

```text
Already Paid = 80M
New Payment = 20M
```

Expected:

```text
Paid = 100M
Outstanding = 0
```

### Case 3

```text
Already Paid = 80M
New Payment = 30M
```

Expected:

```text
PaymentException
```

### Case 4

```text
Deposit > Contract Total
```

Expected:

```text
PaymentException
```

Payment requirements yêu cầu tổng payment không vượt Contract Total và Deposit không vượt Contract Total.

---

# DAY 33 — Payment Polymorphism

Test runtime polymorphism:

```text
PaymentMethod*
       ↓
CashPayment
```

Call:

```cpp
process()
```

Expected:

```text
Cash payment processed.
```

Sau đó:

```text
PaymentMethod*
       ↓
BankTransferPayment
```

Expected:

```text
Bank transfer processed.
```

Sau đó:

```text
PaymentMethod*
       ↓
CardPayment
```

Expected:

```text
Card payment processed.
```

### Test quan trọng

`PaymentService` không được viết kiểu:

```cpp
if (method == CASH)
```

cho toàn bộ behavior.

Nó phải gọi:

```cpp
method.process(amount);
```

Đây là runtime polymorphism được tài liệu OOP định hướng cho `PaymentMethod`.

---

# DAY 34 — PaymentService + Payment UI

### Console test

```text
Contract ID: CT001
Amount: 20,000,000

1. Cash
2. Bank Transfer
3. Card
```

Chọn từng loại.

Expected:

```text
Payment recorded successfully.
```

Kiểm tra SQLite sau mỗi lần thanh toán.

---

# DAY 35 — Contract + Payment Integration

### Full test

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
100M
 ↓
60M
 ↓
20M
 ↓
20M
```

Expected:

```text
Total Paid = 100M
Outstanding = 0
```

### 🟢 MILESTONE 5

```text
Contract ✓
Contract UI ✓
Payment ✓
Payment UI ✓
Payment Rules ✓
Inheritance ✓
Polymorphism ✓
```

---

# 🟪 WEEK 6 — AUTH + OPERATIONS + REPORTS

# DAY 36 — Authentication

### Test valid

```text
Username: admin
Password: correct
```

Expected:

```text
Login successful.
Welcome, Admin.
```

### Test invalid

```text
Username: admin
Password: wrong
```

Expected:

```text
Invalid username or password.
```

FR-AUTH-001 yêu cầu login bằng username/password và xác định role sau khi đăng nhập.

---

# DAY 37 — Authorization

Test từng role:

```text
Admin
Sales
Accountant
Coordinator
Operations
Management
```

Ví dụ Sales:

```text
Customer → allowed
Event → allowed
Booking → allowed
Payment CRUD → denied
```

Expected:

```text
[ERROR] Access denied.
```

Permission matrix của requirements phân quyền theo các actor này.

### 🟢 DONE

Không chỉ ẩn menu; nếu user cố gọi action không có quyền thì Service/authorization cũng phải từ chối.

---

# DAY 38 — Operations

Test:

```text
Assign Staff
Add Special Request
Add Incident
Update Status
```

Operations status:

```text
Planned
Preparing
In Progress
Completed
```

Requirements xác định Operations có các trạng thái này.

---

# DAY 39 — Event Completion

### Test valid

```text
Event status = In Progress
```

Chọn:

```text
Complete Event
```

Expected:

```text
Event completed successfully.
```

Lưu:

```text
Actual Guest Count
Operation Note
Incident
Special Request Result
```

Các thông tin này nằm trong FR-EOC-002.

### Test invalid

Event:

```text
Cancelled
```

→ Không được Complete.

---

# DAY 40 — Booking Report

Test data:

```text
B001 → H001
B002 → H002
B003 → H001
```

Report:

```text
Booking Report
```

Filter:

```text
Date
Hall
Status
```

Expected:

Kết quả filter đúng dữ liệu.

---

# DAY 41 — Event + Payment + Revenue Report

### Event Report

Kiểm tra:

```text
Total Events
Events by Type
Events by Hall
Events by Status
```

### Payment Report

```text
Contract Total
Total Paid
Outstanding
```

### Revenue Report

```text
Total Contract Value
Total Paid
Total Outstanding
```

Đây là đúng nhóm Basic Reports được yêu cầu.

---

# DAY 42 — FULL CONSOLE UI INTEGRATION

Đây là **checkpoint cực kỳ quan trọng**.

Chạy:

```text
Login
 ↓
Main Menu
 ↓
Customer Menu
 ↓
Hall Menu
 ↓
Event Menu
 ↓
Booking Menu
 ↓
Package Menu
 ↓
Service Menu
 ↓
Quotation Menu
 ↓
Contract Menu
 ↓
Payment Menu
 ↓
Operations Menu
 ↓
Report Menu
```

### Test

Mỗi menu:

```text
Open ✓
Action ✓
Back ✓
Invalid choice ✓
Exception ✓
```

### Test kiến trúc

Code review:

```text
Console UI
    ↓
Service
    ↓
Repository
    ↓
SQLite
```

Không được:

```text
Console UI
    ↓
SQLite
```

### 🟢 MILESTONE 6

> **Toàn bộ feature chính + Console UI + SQLite đã hoàn thành.**

Từ đây:

# 🚫 NO NEW FEATURES

---

# 🟫 WEEK 7 — INTEGRATION TESTING

Tuần này chuyển từ:

```text
"Code chức năng"
```

sang:

```text
"Chứng minh hệ thống chạy đúng"
```

---

# DAY 43 — Full E2E Test

Dùng một scenario duy nhất:

```text
Login
 ↓
Customer
 ↓
Event
 ↓
Hall
 ↓
Availability
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
Staff Assignment
 ↓
Operations
 ↓
Complete Event
 ↓
Revenue Report
```

### 🟢 DONE khi

Chạy từ đầu đến cuối **không sửa database thủ công**.

---

# DAY 44 — SQLite Persistence Test

Test từng entity:

```text
Create
 ↓
Exit
 ↓
Restart
 ↓
Read
```

Danh sách:

```text
User
Customer
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

Requirements yêu cầu hệ thống phải save/load dữ liệu khi khởi động lại.

### 🟢 DONE

Không có entity chính nào mất dữ liệu sau restart.

---

# DAY 45 — Invalid Input Test

Tạo bảng test:

|Input|Expected|
|---|---|
|Empty name|Error|
|Negative number|Error|
|Invalid ID|Error|
|Duplicate ID|Error|
|Invalid date|Error|
|Invalid time|Error|
|Invalid menu choice|Error|
|Quantity = 0|Error|

Requirements yêu cầu Console có input validation.

### 🟢 DONE

Không crash.

---

# DAY 46 — Exception Test

Cố tình trigger:

```text
ValidationException
EntityNotFoundException
BookingConflictException
PaymentException
DatabaseException
```

Expected:

```text
[ERROR] ...
```

Không được:

```text
terminate called...
Segmentation fault
Stack trace
```

FR-CONSOLE yêu cầu lỗi nghiệp vụ phải được hiển thị rõ ràng và application không được kết thúc đột ngột.

---

# DAY 47 — Authorization Test

Tạo matrix:

|Role|Customer|Booking|Payment|Operations|Report|
|---|--:|--:|--:|--:|--:|
|Admin|✓|✓|✓|✓|✓|
|Sales|✓|✓|R|R|R|
|Accountant|R|R|✓|R|R|
|Coordinator|R|R|R|✓|R|
|Operations|R|R|R|✓|R|
|Management|R|R|R|R|✓|

So sánh với permission matrix trong requirements.

---

# DAY 48 — CRUD Regression

Mỗi module chạy:

```text
CREATE
 ↓
READ
 ↓
UPDATE
 ↓
READ
 ↓
DELETE
 ↓
READ
```

Module:

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

### 🟢 DONE

Không có module nào CRUD bị hỏng do code module khác.

---

# DAY 49 — Code Cleanup + Architecture Test

Search source:

```text
TODO
DEBUG
cout << "test"
temporary
hard-coded
```

Sau đó review:

```text
UI
 ↓
Service
 ↓
Repository
 ↓
SQLite
```

### 🟢 MILESTONE 7

Không còn lỗi integration nghiêm trọng.

---

# 🟦 WEEK 8 — FINALIZATION

# DAY 50 — Architecture Review

Với **từng module**, tick:

```text
[ ] Model
[ ] Repository
[ ] Service
[ ] Console UI
[ ] SQLite
[ ] Validation
[ ] Exception
[ ] Test
```

Ví dụ Customer:

```text
[x] Customer
[x] CustomerRepository
[x] CustomerService
[x] CustomerMenu
[x] SQLite
[x] Validation
[x] Exception
[x] Test
```

---

# DAY 51 — OOP Test

Không chỉ đọc code — phải **chạy demo OOP**.

### Encapsulation

Chứng minh:

```text
private data
 ↓
getter/setter
```

### Inheritance

Chứng minh:

```text
PaymentMethod
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

### Polymorphism

Chạy:

```text
PaymentMethod*
 ↓
CashPayment
 ↓
process()
```

sau đó:

```text
PaymentMethod*
 ↓
BankTransferPayment
 ↓
process()
```

sau đó:

```text
PaymentMethod*
 ↓
CardPayment
 ↓
process()
```

### Exception

Trigger:

```text
Booking Conflict
Payment exceeds outstanding
Invalid input
```

### 🟢 DONE

Bạn phải có thể **mở source và chỉ cho giảng viên từng phần OOP**.

---

# DAY 52 — Business Rules Test

Tạo một bảng:

|Rule|Test|Result|
|---|---|---|
|Hall Active|Inactive Hall → Booking|PASS|
|Start < End|22:00 → 18:00|PASS|
|No overlap|20–23 vs 18–22|PASS|
|Payment ≤ Total|30M > 20M outstanding|PASS|
|Deposit ≤ Total|Deposit > Total|PASS|

Các core business rules này được nêu trực tiếp trong requirements.

### 🟢 DONE

**Không có business rule nào chỉ kiểm tra ở UI.**

---

# DAY 53 — Console UI Polish

Test:

```text
Header
Menu
Spacing
Error
Success
Back
Logout
```

Ví dụ:

```text
==================================================
              CUSTOMER MANAGEMENT
==================================================
1. Create Customer
2. View Customers
3. Search Customer
4. Update Customer
5. Delete Customer
6. Back
--------------------------------------------------
Enter your choice:
```

Đúng định hướng Console Interface trong requirements: mỗi module có menu riêng và thao tác rõ ràng.

### 🟢 DONE

Không thêm feature.

---

# DAY 54 — Final Bug Fix

Tạo bug list:

```text
BUG-001
BUG-002
BUG-003
```

Mỗi bug:

```text
Problem
 ↓
Reproduce
 ↓
Fix
 ↓
Retest
```

Không được ghi:

```text
"Probably fixed"
```

Phải test lại.

---

# DAY 55 — Demo Dataset

Tạo:

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
Staff Assignments
```

### Demo test

```text
Login
 ↓
Create Customer
 ↓
Create Event
 ↓
Select Hall
 ↓
Check Availability
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
Revenue Report
```

### 🟢 DONE

Có thể chạy demo **không cần sửa database bằng tay**.

---

# DAY 56 — FINAL ACCEPTANCE TEST

Đây là ngày **không code thêm**.

Chạy checklist:

```text
========================================
      FINAL ACCEPTANCE TEST
========================================

[ ] Application starts
[ ] SQLite connects
[ ] Login works
[ ] Authorization works

[ ] Customer works
[ ] Hall works
[ ] Event works
[ ] Availability works
[ ] Booking works

[ ] Package works
[ ] Service works
[ ] Quotation works
[ ] Contract works
[ ] Payment works

[ ] Operations works
[ ] Event Completion works
[ ] Reports work

[ ] Validation works
[ ] Exceptions work
[ ] Persistence works
[ ] OOP demo works
[ ] E2E works
```

### Final E2E

Phải chạy được:

```text
Login
  ↓
Customer
  ↓
Event
  ↓
Hall
  ↓
Availability
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
Event Completion
  ↓
Reports
```

Đây cũng chính là flow nghiệp vụ chính của hệ thống trong Functional Requirements.

---

# 📊 BẢNG "NGÀY NÀO PHẢI ĐẠT GÌ?"

Đây là bảng mình khuyên bạn **in ra hoặc đặt cạnh máy khi code**:

|Ngày|Task|Điều kiện 🟢 DONE|
|--:|---|---|
|1|Requirements|Checklist + UI flow|
|2|Project + SQLite + UI|Build + DB + Main Menu|
|3|Schema|17 tables|
|4|Models|Models compile + getter/setter|
|5|Exception|Throw/catch pass|
|6|Repository|Repository → SQLite pass|
|7|Foundation|Full foundation integration|
|8|Customer Repo|CRUD + DB|
|9|Customer Service|Validation + CRUD|
|10|Customer UI|Menu → Service|
|11|Customer Test|E2E + persistence|
|12|Hall Repo|CRUD + DB|
|13|Hall Service|Validation + rules|
|14|Hall UI|E2E|
|15|Event Repo|CRUD|
|16|Event Service|Validation|
|17|Event UI|E2E|
|18|Booking Repo|CRUD|
|19|Conflict|5 conflict cases PASS|
|20|Booking Service|All business rules|
|21|Booking UI|E2E|
|22|Package|CRUD + UI|
|23|Service|CRUD + UI|
|24|Package-Service|Relationship works|
|25|Booking-Service|Quantity calculation|
|26|Quotation Repo|DB|
|27|Quotation|Calculation PASS|
|28|Quotation UI|E2E|
|29|Contract|Create/View|
|30|Contract UI|Status works|
|31|Payment Repo|CRUD|
|32|Payment Rules|Overpayment rejected|
|33|Polymorphism|3 subclasses run|
|34|Payment UI|Payment E2E|
|35|Contract-Payment|Full flow|
|36|Login|Valid/invalid login|
|37|Authorization|Role matrix PASS|
|38|Operations|CRUD/actions|
|39|Completion|Complete Event PASS|
|40|Booking Report|Filters PASS|
|41|Reports|4 reports PASS|
|42|**Full UI**|**All menus work**|
|43|E2E|Full business flow|
|44|Persistence|Restart test PASS|
|45|Input Test|Invalid inputs handled|
|46|Exception Test|All exceptions handled|
|47|Auth Test|All roles PASS|
|48|Regression|CRUD PASS|
|49|Cleanup|No critical bugs|
|50|Architecture|Layering correct|
|51|OOP|Can demonstrate OOP|
|52|Business Rules|All rules PASS|
|53|UI Polish|Console consistent|
|54|Bug Fix|All known bugs fixed|
|55|Demo|Demo scenario PASS|
|56|**FINAL**|**Acceptance Test PASS**|

---

# 🔥 Quan trọng nhất: mỗi ngày phải có "Evidence"

Mình khuyên bạn **không đánh dấu DONE chỉ vì code xong**.

Ví dụ Day 19 phải lưu lại:

```text
DAY-19-TEST.txt

[PASS] 18:00-22:00 vs 20:00-23:00 → Conflict
[PASS] 18:00-22:00 vs 16:00-19:00 → Conflict
[PASS] 18:00-22:00 vs 22:00-23:00 → No Conflict
[PASS] 18:00-22:00 vs 14:00-18:00 → No Conflict
[PASS] Cancelled booking → No Conflict
```

Day 27:

```text
DAY-27-TEST.txt

Package       = 8,000,000
Services      = 8,500,000
Subtotal      = 16,500,000
Discount      = 1,000,000
Tax           = 1,550,000
Expected Total = 17,050,000
Actual Total   = 17,050,000

[PASS]
```

Day 33:

```text
DAY-33-TEST.txt

[PASS] CashPayment::process()
[PASS] BankTransferPayment::process()
[PASS] CardPayment::process()

[PASS] PaymentMethod* runtime polymorphism
```

Day 43:

```text
DAY-43-E2E.txt

[PASS] Login
[PASS] Customer
[PASS] Event
[PASS] Hall
[PASS] Booking
[PASS] Package
[PASS] Service
[PASS] Quotation
[PASS] Contract
[PASS] Payment
[PASS] Operations
[PASS] Event Completion
[PASS] Report
```

## 🎯 Như vậy tiêu chuẩn của bạn sẽ là:

```text
CODE
  ↓
COMPILE
  ↓
RUN
  ↓
HAPPY CASE
  ↓
ERROR CASE
  ↓
DATABASE CHECK
  ↓
INTEGRATION CHECK
  ↓
🟢 DONE
```

Chứ **không phải**:

```text
Code xong
   ↓
"Chắc là Done"
```

Cách này đặc biệt phù hợp với project của bạn vì Functional Requirements đã yêu cầu không chỉ feature, mà còn **CRUD, validation, exception handling, Console UI, persistence/load và business rules**.

Và quan trọng nhất: **từ Day 8 trở đi, mỗi module đều phải có cả Console UI + SQLite test trước khi đánh dấu hoàn thành**. Điều đó giúp bạn luôn có một hệ thống chạy được trong suốt 8 tuần, thay vì đến cuối mới ghép UI.