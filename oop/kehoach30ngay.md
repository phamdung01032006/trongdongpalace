# KẾ HOẠCH 30 NGÀY — C++ OOP TRỐNG ĐỒNG PALACE
## Bản rút gọn từ kế hoạch 8 tuần, chuyển sang JSON + nlohmann/json

> Mục tiêu: trong 30 ngày có một hệ thống C++17 Console **chạy được end-to-end**, thể hiện rõ OOP, JSON persistence, business rules, exception handling và có thể demo/giải thích cho giảng viên.
>
> Kiến trúc cố định:
>
> `Console UI → Service → Repository → data.json`
>
> **Không dùng SQL/SQLite.** Repository chỉ xử lý JSON; Service chứa business logic; UI không truy cập JSON trực tiếp.

---

# 1. NGUYÊN TẮC LÀM VIỆC TRONG 30 NGÀY

## Definition of Done

Một task chỉ được đánh dấu 🟢 DONE khi:

```text
1. Compile thành công
2. Happy case chạy đúng
3. Error/validation case chạy đúng
4. Dữ liệu JSON được save/load nếu task có persistence
5. Không crash khi exception xảy ra
6. Không phá vỡ feature đã hoàn thành trước đó
```

Trạng thái:

```text
🟢 DONE     = compile + test pass
🟡 PARTIAL  = chạy nhưng còn test fail
🔴 NOT DONE = chưa compile hoặc chức năng chính chưa chạy
```

**Không dùng “code xong” làm tiêu chuẩn hoàn thành.**

---

# 2. NGUYÊN TẮC TESTING + CONSOLE UI TRONG 30 NGÀY

## 2.1. Testing được làm song song, không dồn vào Day 29–30

Mỗi feature phải đi theo vòng lặp:

```text
Implement → Compile → Unit / Functional Test → Fix
→ Integration Test → Regression Test → 🟢 DONE
```

Mỗi ngày phải lưu test evidence ngắn gọn, ví dụ:

```text
tests/
├── day01_json_test.txt
├── day02_exception_test.txt
├── day07_customer_test.txt
├── day13_booking_conflict_test.txt
├── day19_quotation_test.txt
├── day22_payment_test.txt
├── day28_integration_test.txt
└── day30_final_e2e.txt
```

### Mức test bắt buộc

```text
1. Compile Test
2. Happy Case
3. Validation / Error Case
4. Persistence Test: Save → Exit → Restart → Load
5. Integration Test
6. Regression Test
7. Final E2E Test
```

Mỗi feature DONE phải ghi:

```text
Feature:
Date:
Files tested:
Input:
Expected:
Actual:
Result: PASS / FAIL
```

---

## 2.2. ConsoleUI được code song song với Service

Kiến trúc:

```text
Input
 ↓
ConsoleUI
 ↓
Service
 ↓
Repository
 ↓
data.json
```

ConsoleUI **không được**:

```text
❌ đọc/ghi data.json trực tiếp
❌ tự kiểm tra booking overlap
❌ tự tính quotation total
❌ tự quyết định payment hợp lệ
❌ chứa business rule
```

ConsoleUI chỉ:

```text
Hiển thị menu
Nhận input
Gọi Service
Hiển thị kết quả
Catch exception
Yêu cầu xác nhận Delete/Cancel
```

Có thể tổ chức đơn giản:

```cpp
class ConsoleUI {
public:
    void run();
    void showLoginMenu();
    void showMainMenu();
    void showCustomerMenu();
    void showHallMenu();
    void showEventMenu();
    void showBookingMenu();
    void showPackageMenu();
    void showServiceMenu();
    void showQuotationMenu();
    void showContractMenu();
    void showPaymentMenu();
    void showOperationsMenu();
    void showReportMenu();
};
```

Nếu class quá lớn, có thể tách menu helper đơn giản; không cần UI framework.

### Error handling của UI

```cpp
try {
    // gọi Service
}
catch (const AppException& e) {
    cout << "[ERROR] " << e.what() << endl;
}
```

Sau exception, chương trình phải tiếp tục chạy.

### Quy tắc menu

```text
0. Back
9. Exit
```

Delete/Cancel:

```text
Are you sure? (Y/N)
```

Role không có quyền:

```text
[ERROR] Access denied.
```

# 2. MỤC TIÊU SAU 30 NGÀY

Phải chạy được flow:

```text
Login
  ↓
Customer
  ↓
Event Request
  ↓
Check Hall Availability
  ↓
Booking
  ↓
Package / Service
  ↓
Quotation
  ↓
Contract
  ↓
Deposit / Payment
  ↓
Event Preparation
  ↓
Staff / Table / Requirement / Incident
  ↓
Complete Event
  ↓
Search / Report
```

Và chứng minh được:

```text
Class/Object
Encapsulation
Composition/Aggregation
Inheritance
Abstract Class
Virtual Function
Runtime Polymorphism
Exception Handling
JSON File I/O
Repository
CRUD
Validation
Business Logic
```

Chương 2 của học phần nhấn mạnh class, object, thành phần dữ liệu/phương thức và che giấu dữ liệu qua phạm vi truy xuất; vì vậy các model phải thực sự có encapsulation, không chỉ dùng struct chứa dữ liệu.  
Chương 3 yêu cầu hiểu lớp cơ sở/lớp dẫn xuất và cách kế thừa; chương 4 tập trung vào virtual function, liên kết động và đa hình.  
Chương 5 tập trung vào I/O stream và thao tác mở/đọc/ghi file; chương 6 yêu cầu cơ chế `try/catch/throw` và xây dựng lớp ngoại lệ.

---

# 🟩 PHASE 1 — FOUNDATION + OOP CORE
## DAY 1 → DAY 5

---

# DAY 1 — Project Setup + JSON Foundation
### Console UI

Tạo skeleton `ConsoleUI::run()`:

```text
================================
     TRONG DONG PALACE
================================
1. Login
0. Exit
```

Chưa cần đầy đủ module, nhưng UI phải compile và chạy được.

### Test UI

```text
[PASS] Application starts
[PASS] Main screen displayed
[PASS] 0 → Exit
[PASS] Invalid menu option → clear error
```


### Task

Tạo project C++17:

```text
project/
├── include/
├── src/
├── data/
│   └── data.json
├── tests/
│   └── dayXX_test.txt
├── main.cpp
└── CMakeLists.txt
```

Cài `nlohmann/json`.

Tạo:

```text
JsonStorage
InputHelper
DisplayHelper
ConsoleUI
```

Khởi tạo `data.json` với schema thống nhất.

### Test

**Test 1 — Compile**

```text
Build successful
```

**Test 2 — File**

Chạy chương trình:

```text
Application started.
Data file loaded successfully.
```

Nếu `data.json` chưa tồn tại → tự tạo file rỗng hợp lệ.

**Test 3 — JSON lỗi**

Cố tình sửa JSON thành:

```text
{ invalid json
```

Expected:

```text
[ERROR] Failed to parse data.json.
```

Application không crash.

### 🟢 DONE

```text
C++17 ✓
CMake ✓
nlohmann/json ✓
data.json ✓
Load/save foundation ✓
Invalid JSON handled ✓
```

---

# DAY 2 — Exception Hierarchy
### Console UI

Thêm cơ chế dùng chung:

```text
showError()
pause()
readMenuChoice()
```

Test từng custom exception và bảo đảm sau lỗi menu vẫn chạy tiếp, không crash.


### Task

Tạo:

```text
AppException
ValidationException
NotFoundException
DuplicateException
BookingConflictException
AuthenticationException
FileException
```

Thiết kế exception theo hierarchy để UI có thể catch `AppException`.

### Test

Trigger:

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

Test thêm:

```text
NotFoundException
DuplicateException
BookingConflictException
AuthenticationException
FileException
```

### 🟢 DONE

```text
throw ✓
catch ✓
message ✓
application không crash ✓
```

---

# DAY 3 — Base Models + Encapsulation
### Console UI

Hoàn thiện `InputHelper`:

```text
readString()
readInt()
readDouble()
readYesNo()
```

Test nhập chữ vào số, chuỗi rỗng, số âm và lựa chọn Y/N không hợp lệ.


### Task

Tạo các model nền:

```text
User
Customer
Center
Hall
Package
Service
```

Mỗi class có:

```text
private attributes
constructor
getter
setter
validation cơ bản
```

Không public toàn bộ dữ liệu.

### Test

```cpp
Customer c("C001", "Nguyen Van A", "0901234567");
cout << c.getName();
```

Expected:

```text
Nguyen Van A
```

Test setter hợp lệ và không hợp lệ.

### 🟢 DONE

Mỗi model compile và chứng minh được:

```text
private data
    ↓
public method
    ↓
controlled access
```

---

# DAY 4 — Event Inheritance

### Task

Tạo abstract class:

```text
Event
├── WeddingEvent
├── CorporateEvent
└── BirthdayEvent
```

`Event` có:

```cpp
virtual string getEventType() const = 0;
virtual double calculateBasePrice() const = 0;
virtual ~Event() = default;
```

Mỗi class con phải có behavior khác nhau.

### Test

Tạo:

```cpp
WeddingEvent
CorporateEvent
BirthdayEvent
```

Kiểm tra:

```text
getEventType()
calculateBasePrice()
```

Expected: kết quả khác nhau theo loại event.

### 🟢 DONE

```text
Abstract class ✓
Inheritance ✓
Override ✓
Different behavior ✓
```

---

# DAY 5 — Runtime Polymorphism + Integration

### Task

Phải có:

```cpp
vector<shared_ptr<Event>> events;

for (auto& e : events)
    cout << e->calculateBasePrice();
```

Không dùng logic kiểu:

```cpp
if (type == "WEDDING") ...
```

để thay thế polymorphism.

### Test

Cho vector chứa cả:

```text
WeddingEvent
CorporateEvent
BirthdayEvent
```

Gọi cùng:

```cpp
e->calculateBasePrice()
```

Expected:

```text
Mỗi object gọi đúng implementation của class con.
```

### 🟢 MILESTONE 1

```text
Project ✓
JSON ✓
Exception ✓
Encapsulation ✓
Inheritance ✓
Abstract Class ✓
Virtual Function ✓
Runtime Polymorphism ✓
```

---

# 🟦 PHASE 2 — REPOSITORY + CUSTOMER + CENTER/HALL
## DAY 6 → DAY 10

---

# DAY 6 — Repository Foundation
### Console UI / Architecture Test

Tạo luồng mẫu:

```text
ConsoleUI → CustomerService → CustomerRepository → data.json
```

Test và xác nhận UI không truy cập JSON/Repository trực tiếp để thực hiện business action.


### Task

Thiết kế repository interface/structure:

```text
load()
save()
add()
update()
remove()
findById()
getAll()
```

Tạo:

```text
UserRepository
CustomerRepository
CenterRepository
HallRepository
EventRepository
BookingRepository
PackageRepository
ServiceRepository
QuotationRepository
ContractRepository
PaymentRepository
```

Không cần full implementation tất cả ngay; phải có cấu trúc thống nhất.

### Test

Dùng `CustomerRepository` làm mẫu:

```text
add(C001)
↓
data.json
↓
load()
↓
findById(C001)
```

Expected:

```text
C001 - Nguyen Van A
```

### 🟢 DONE

Repository → JSON → Repository hoạt động.

---

# DAY 7 — Customer CRUD
### Console UI

Tạo Customer Menu:

```text
1. Create Customer
2. View All
3. Find Customer
4. Update Customer
5. Delete Customer
0. Back
```

Test CRUD, duplicate ID, NotFound, input sai và Delete confirmation.


### Task

Hoàn thiện:

```text
CustomerRepository
CustomerService
CustomerMenu
```

Chức năng:

```text
Create
View
Search
Update
Delete
```

### Test

```text
Create C001
View C001
Update C001
Search C001
Delete C001
View C001
```

Expected:

```text
Create → success
Update → new data
Delete → NotFound
```

### Error cases

```text
ID trùng
ID không tồn tại
Tên rỗng
Số điện thoại sai
```

### Persistence

```text
Create
↓
Exit
↓
Restart
↓
View
```

C001 vẫn phải tồn tại.

---

# DAY 8 — Center + Hall CRUD
### Console UI

Thêm Center Menu và Hall Menu. Test navigation, CRUD, ID không tồn tại và Delete confirmation.


### Task

Hoàn thiện:

```text
CenterService
HallService
CenterRepository
HallRepository
CenterMenu
HallMenu
```

### Validation

```text
Capacity <= 0
Hall ID trùng
Center ID không tồn tại
Hall không thuộc Center
```

### Business rule

Chỉ Hall có:

```text
status = ACTIVE
```

mới được dùng cho Booking.

### 🟢 DONE

CRUD + validation + JSON persistence pass.

---

# DAY 9 — User + Authentication
### Console UI

Login screen:

```text
Username:
Password:
1. Login
0. Exit
```

Test đúng tài khoản, sai password, user không tồn tại, input rỗng và Exit.


### Task

Tạo:

```text
UserRepository
AuthService
LoginMenu
```

Role:

```text
ADMIN
SALES
ACCOUNTANT
COORDINATOR
MANAGEMENT
```

### Test

Valid:

```text
admin / correct-password
```

Expected:

```text
Login successful.
```

Invalid password:

```text
AuthenticationException
```

User không tồn tại:

```text
AuthenticationException
```

### 🟢 DONE

Login trả về user + role.

---

# DAY 10 — Authorization + Foundation Regression
### Console UI

Tạo Main Menu theo role:

```text
1. Customer
2. Center / Hall
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

Test từng role, action bị cấm, Logout → Login và regression các module đã DONE.


### Task

Tạo permission matrix đơn giản.

Ví dụ:

```text
SALES
✓ Customer
✓ Event
✓ Booking
✗ Payment management
```

Không chỉ ẩn menu; action không có quyền phải bị từ chối.

### Test

```text
Sales → Payment CRUD
```

Expected:

```text
[ERROR] Access denied.
```

Chạy regression:

```text
Login
Customer
Center
Hall
```

### 🟢 MILESTONE 2

```text
JSON Repository ✓
Customer ✓
Center ✓
Hall ✓
Authentication ✓
Authorization ✓
```

---

# 🟨 PHASE 3 — EVENT REQUEST + BOOKING
## DAY 11 → DAY 15

---

# DAY 11 — Event Repository + Event Service
### Console UI

Thêm Event Menu:

```text
1. Create Event Request
2. View Events
3. Find Event
4. Update Event
5. Cancel Event
0. Back
```

Test Event hợp lệ, invalid time, guest count, Customer/Hall không tồn tại.


### Task

Persistence cho:

```text
Event
WeddingEvent
CorporateEvent
BirthdayEvent
```

Lưu JSON chỉ chứa dữ liệu + ID cần thiết, **không lưu pointer C++**.

### Validation

```text
Start time >= End time
Guest count <= 0
Hall không tồn tại
Customer không tồn tại
```

### Test

```text
18:00 → 22:00
```

Pass.

```text
22:00 → 18:00
```

Expected:

```text
ValidationException
Start time must be before end time.
```

---

# DAY 12 — Event Request + Availability
### Console UI

Flow:

```text
Create Event Request
→ Select Customer
→ Select Event Type
→ Select Hall
→ Enter Start / End
→ Check Availability
```

Test AVAILABLE, CONFLICT, Hall inactive, invalid time và Hall không tồn tại.


### Task

Hoàn thiện:

```text
Create Event Request
View Event Request
Check Hall Availability
```

Flow:

```text
Customer
 ↓
Event Request
 ↓
Select Hall
 ↓
Check Availability
```

### Test

Existing:

```text
H001
18:00 → 22:00
```

New:

```text
H001
20:00 → 23:00
```

Expected:

```text
CONFLICT
```

---

# DAY 13 — Booking Conflict Logic
### Console UI

Hiển thị kết quả từ Service:

```text
[AVAILABLE] Hall H001 is available.
[CONFLICT] Hall H001 is already booked in this time range.
```

Lưu evidence vào `tests/day13_booking_conflict_test.txt`. UI không tự tính overlap.


### Task

Implement đúng:

```cpp
newStart < oldEnd && newEnd > oldStart
```

Chỉ xét booking/event còn hiệu lực; booking Cancelled không chiếm lịch.

### Test bắt buộc

| Existing | New | Expected |
|---|---|---|
|18–22|20–23|CONFLICT|
|18–22|16–19|CONFLICT|
|18–22|22–23|NO CONFLICT|
|18–22|14–18|NO CONFLICT|
|18–22 Cancelled|20–23|NO CONFLICT|

### 🟢 DONE

**5/5 cases PASS.**

Đây là business rule quan trọng nhất của hệ thống, phải có evidence test.

---

# DAY 14 — BookingService
### Console UI

Tạo Booking Menu:

```text
1. Check Availability
2. Create Booking
3. View Bookings
4. Find Booking
5. Cancel Booking
0. Back
```

Test valid booking, Event/Hall/Customer không tồn tại, inactive Hall, invalid time, overlap, duplicate ID và Cancel confirmation.


### Task

BookingService chịu trách nhiệm:

```text
Validate Event
Validate Hall
Validate Hall Active
Validate time
Check overlap
Create Booking
Save JSON
```

UI không tự kiểm tra overlap.

### Error cases

```text
Event không tồn tại
Hall không tồn tại
Hall inactive
Time invalid
Booking conflict
Customer không tồn tại
```

Mỗi case phải throw exception phù hợp.

---

# DAY 15 — Booking UI + E2E
### Console UI

Hoàn thiện navigation:

```text
Login → Main Menu → Event → Booking
→ Availability → Create Booking → View Booking
```

Lưu E2E evidence vào `tests/day15_booking_e2e.txt`.


### Test

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
Create Booking
↓
View Booking
```

Persistence:

```text
Exit
↓
Restart
↓
View Booking
```

### 🟢 MILESTONE 3

Booking flow hoàn chỉnh.

---

# 🟧 PHASE 4 — PACKAGE + SERVICE + QUOTATION
## DAY 16 → DAY 20

---

# DAY 16 — Package + Service CRUD
### Console UI

Package Menu và Service Menu đều có:

```text
Create / View / Find / Update / Delete / Back
```

Test CRUD, duplicate ID, NotFound, giá âm, quantity không hợp lệ và Delete confirmation.


### Task

Hoàn thiện:

```text
PackageRepository
PackageService
ServiceRepository
ServiceService
PackageMenu
ServiceMenu
```

CRUD:

```text
Create
Read
Update
Delete
```

### Validation

```text
Price < 0
Quantity <= 0
Duplicate ID
NotFound
```

---

# DAY 17 — Package ↔ Service

### Task

Thiết kế quan hệ:

```text
Package
 ├── Service
 ├── Service
 └── Service
```

Dùng ID:

```text
packageId
serviceId
```

Không lưu pointer.

### Test

```text
P001
 ↓
Add S001
 ↓
Add S002
 ↓
View P001
```

Expected:

```text
P001
 ├── S001
 └── S002
```

Kiểm tra JSON.

---

# DAY 18 — Booking Services

### Task

Cho Booking sử dụng service:

```text
Booking B001
 ↓
Sound x2
 ↓
Decoration x1
```

Công thức:

```text
Service Amount = Quantity × Unit Price
```

### Test

```text
Sound = 2,000,000
Quantity = 2
```

Expected:

```text
4,000,000
```

Invalid:

```text
Quantity = 0
Quantity = -1
```

Expected:

```text
ValidationException
```

---

# DAY 19 — Quotation
### Console UI

Quotation Menu:

```text
1. Create Quotation
2. View Quotation
3. Calculate Total
4. Find Quotation
5. Update Quotation
0. Back
```

UI không tự tính Total; chỉ hiển thị kết quả từ QuotationService.


### Task

Tạo:

```text
Quotation
QuotationItem
QuotationRepository
QuotationService
QuotationMenu
```

Công thức:

```text
Subtotal = Package Amount + Service Amount
Total = Subtotal - Discount + Tax
```

### Test

```text
Package = 8,000,000
Decoration = 5,000,000
Sound = 2,000,000
Lighting = 1,500,000
```

Expected:

```text
Subtotal = 16,500,000
Discount = 1,000,000
Tax = 1,550,000
Total = 17,050,000
```

System tự tính Total; user không nhập Total trực tiếp.

---

# DAY 20 — Quotation Integration
### UI Integration Test

Chạy:

```text
Booking → Package/Service → Quotation → Calculate → Save
→ Restart → View Quotation
```

Sau đó chạy regression từ Login đến Booking.


### Full flow

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
Save JSON
```

Restart application → Quotation vẫn tồn tại.

### 🟢 MILESTONE 4

```text
Package ✓
Service ✓
Package-Service ✓
Booking-Service ✓
Quotation ✓
Calculation ✓
Persistence ✓
```

---

# 🟥 PHASE 5 — CONTRACT + PAYMENT + OPERATIONS
## DAY 21 → DAY 25

---

# DAY 21 — Contract
### Console UI

Contract Menu:

```text
1. Create Contract
2. View Contracts
3. Find Contract
4. Update Status
5. Cancel Contract
0. Back
```

Test Quotation không tồn tại, duplicate ID, invalid status và Cancel confirmation.


### Task

Tạo:

```text
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
View Contract
```

Status:

```text
DRAFT
ACTIVE
COMPLETED
CANCELLED
```

### Test

```text
Q001 → CT001
```

Expected:

```text
Contract created successfully.
```

---

# DAY 22 — Payment
### Console UI

Payment Menu:

```text
1. Record Payment
2. View Payments
3. View Contract Balance
4. Find Payment
0. Back
```

Test amount = 0, amount âm, Payment > Outstanding, Deposit > Total và Contract không tồn tại.


### Task

Tạo:

```text
PaymentRepository
PaymentService
PaymentMenu
```

Công thức:

```text
Outstanding = Total - Paid
```

Status:

```text
Paid = 0
→ Unpaid

Paid < Total
→ Partially Paid

Paid >= Total
→ Paid
```

### Test

Contract:

```text
100M
```

Cases:

```text
20M → Outstanding 80M
80M → Outstanding 20M
20M → Outstanding 0
```

Invalid:

```text
Amount < 0
Payment > Outstanding
Deposit > Contract Total
```

Expected:

```text
ValidationException / PaymentException
```

---

# DAY 23 — Payment Polymorphism
### Console UI

Khi record payment:

```text
1. Cash
2. Bank Transfer
3. Card
```

Test cả 3 method và một lựa chọn không hợp lệ. Behavior phải thể hiện runtime polymorphism.


### Task

Nếu nhóm sử dụng payment method, thiết kế:

```text
PaymentMethod
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

Base class có virtual function:

```cpp
virtual void process(double amount) = 0;
```

### Test

```cpp
PaymentMethod* p = new CashPayment();
p->process(20000000);
```

Expected:

```text
Cash payment processed.
```

Lặp lại với:

```text
BankTransferPayment
CardPayment
```

### 🟢 DONE

Có runtime polymorphism thực tế, không phải inheritance để “đủ checklist”.

---

# DAY 24 — Event Operations
### Console UI

Operations Menu:

```text
1. Assign Staff
2. Add Special Requirement
3. Create Table
4. Assign Guest/Table
5. Record Incident
6. View Event Operations
0. Back
```

Test Event/Staff/Table không tồn tại, duplicate assignment, input rỗng và incident không hợp lệ.


### Task

Hoàn thiện:

```text
Staff
Staff Assignment
Event Requirement
Table
Table Assignment
Incident
```

Chức năng tối thiểu:

```text
Assign Staff
Add Special Requirement
Create Table / Table Plan
Assign Guest/Table
Record Incident
```

Status:

```text
PLANNED
PREPARING
IN_PROGRESS
COMPLETED
```

### Test

```text
Event
 ↓
Assign Staff
 ↓
Add Requirement
 ↓
Create Table
 ↓
Assign Table
 ↓
Add Incident
```

Kiểm tra JSON sau mỗi action.

---

# DAY 25 — Event Completion + E2E Business Flow
### Console UI

Operations phải có:

```text
1. Start Preparation
2. Update Event Status
3. Complete Event
4. View Operations
```

Không cho Complete nếu Event đã CANCELLED. Lưu E2E evidence vào `tests/day25_core_e2e.txt`.


### Task

Hoàn thiện:

```text
Complete Event
Actual Guest Count
Operation Note
```

Không cho Complete nếu Event đã:

```text
CANCELLED
```

### Full E2E

```text
Customer
 ↓
Event Request
 ↓
Hall Availability
 ↓
Booking
 ↓
Package/Service
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
```

### 🟢 MILESTONE 5

**Core business flow chạy được từ đầu đến cuối.**

---

# 🟪 PHASE 6 — SEARCH + REPORT + TESTING + FINAL
## DAY 26 → DAY 30

---

# DAY 26 — Search + Filter
### Console UI

Search/Report Menu:

```text
1. Search Customer
2. Search Event
3. Search Booking
4. Search Hall
5. Search Contract
6. Search Payment
7. Filter
0. Back
```

Test ID, Name, Date, Hall, Event Type, Status và No Result.


### Task

Tạo:

```text
ReportService
SearchService hoặc chức năng search trong Service phù hợp
```

Search/filter:

```text
Customer
Event
Booking
Hall
Contract
Payment
```

Filter theo:

```text
ID
Name
Date
Hall
Event Type
Status
```

### Test

Tạo ít nhất 5–10 records.

Test filter:

```text
Hall = H001
Status = ACTIVE
Event Type = WEDDING
```

Expected: chỉ trả về record phù hợp.

---

# DAY 27 — Reports
### Console UI

Report Menu:

```text
1. Booking Report
2. Event Report
3. Payment Report
4. Revenue Report
0. Back
```

Test dataset có expected result trước, đồng thời test trường hợp không có dữ liệu.


### Báo cáo tối thiểu

```text
Booking Report
Event Report
Payment Report
Revenue Report
```

### Event Report

```text
Total Events
Events by Type
Events by Hall
Events by Status
```

### Payment/Revenue

```text
Total Contract Value
Total Paid
Total Outstanding
```

### Test

Dùng dataset demo cố định và tính expected result trước.

Ví dụ:

```text
Contract A = 100M, Paid = 80M
Contract B = 50M, Paid = 50M
```

Expected:

```text
Contract Value = 150M
Paid = 130M
Outstanding = 20M
```

---

# DAY 28 — Full Integration + Persistence Test
### Console UI Test

Kiểm tra toàn bộ navigation:

```text
Login → Main Menu → Module → Back → Main Menu
→ Logout → Login
```

Test exception recovery: sau một lỗi nghiệp vụ, UI phải quay lại menu bình thường.


### Test 1 — Full restart

Tạo dữ liệu:

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
Operations
```

Sau đó:

```text
Exit
↓
Restart
↓
Load JSON
↓
Read all
```

Không entity chính nào được mất dữ liệu.

### Test 2 — Broken JSON

Cố tình làm JSON lỗi.

Expected:

```text
FileException
```

Application xử lý an toàn.

### Test 3 — Missing ID

Tìm:

```text
C999
```

Expected:

```text
NotFoundException
```

---

# DAY 29 — Final OOP + Business Rule + Regression Test
### Console UI Regression

Chạy toàn bộ menu bằng từng role và kiểm tra:

```text
Navigation
Permission
Input validation
Exception recovery
Business rule error display
```

Đây là vòng regression cuối trước Final Acceptance.


## OOP checklist

### Encapsulation

```text
private attributes
getter/setter
validation
```

### Inheritance

```text
Event
├── WeddingEvent
├── CorporateEvent
└── BirthdayEvent
```

### Polymorphism

```text
vector<shared_ptr<Event>>
```

và/hoặc:

```text
PaymentMethod*
├── CashPayment
├── BankTransferPayment
└── CardPayment
```

### Exception

Test:

```text
ValidationException
NotFoundException
DuplicateException
BookingConflictException
AuthenticationException
FileException
```

### Business rules

| Rule | Test | Expected |
|---|---|---|
| Start < End | 22–18 | Reject |
| No overlap | 20–23 vs 18–22 | Reject |
| Boundary | 22–23 vs 18–22 | Allow |
| Cancelled | Cancelled vs 20–23 | Allow |
| Hall Active | Inactive Hall → Booking | Reject |
| Payment | Payment > Outstanding | Reject |
| Deposit | Deposit > Total | Reject |
| Quantity | Quantity <= 0 | Reject |

### Regression

```text
Login ✓
Customer ✓
Hall ✓
Event ✓
Booking ✓
Package ✓
Service ✓
Quotation ✓
Contract ✓
Payment ✓
Operations ✓
Reports ✓
```

---

# DAY 30 — FINAL ACCEPTANCE TEST + DEMO
## FINAL CONSOLE UI WALKTHROUGH

Demo theo đúng flow:

```text
1. Login
2. Main Menu + Role
3. Customer
4. Center/Hall
5. Event Request
6. Availability
7. Booking
8. Package/Service
9. Quotation
10. Contract
11. Payment
12. Operations
13. Complete Event
14. Search
15. Report
16. Logout
```

Không phát triển feature mới. Chỉ Test → Fix critical bug → Retest → Prepare demo.


## 🚫 Không phát triển feature mới

Chỉ:

```text
Test
Fix critical bug
Retest
Prepare demo
```

## FINAL ACCEPTANCE CHECKLIST

```text
========================================
       TRONG DONG PALACE
       FINAL ACCEPTANCE TEST
========================================

[ ] C++17 build
[ ] Application starts
[ ] ConsoleUI starts
[ ] Main Menu navigation
[ ] Role-based menu
[ ] data.json loads
[ ] Invalid JSON handled

[ ] Login
[ ] Authorization

[ ] Customer CRUD
[ ] Center CRUD
[ ] Hall CRUD

[ ] Event Request
[ ] WeddingEvent
[ ] CorporateEvent
[ ] BirthdayEvent

[ ] Runtime Polymorphism
[ ] Hall Availability
[ ] Booking
[ ] Overlap validation

[ ] Package
[ ] Service
[ ] Package-Service
[ ] Booking-Service
[ ] Quotation

[ ] Contract
[ ] Payment
[ ] Payment rules

[ ] Staff Assignment
[ ] Table Plan
[ ] Special Requirement
[ ] Incident
[ ] Event Completion

[ ] Search
[ ] Filter
[ ] Reports

[ ] Validation
[ ] Exception Handling
[ ] JSON Persistence
[ ] Restart Test
[ ] Full E2E
[ ] UI Exception Recovery
[ ] UI Navigation Regression
[ ] Test Evidence files
[ ] OOP Demo
```

---

# 3. BẢNG 30 NGÀY TÓM TẮT

| Ngày | Task | 🟢 DONE khi |
|---:|---|---|
| 1 | Project + JSON | Build + load/save + invalid JSON |
| 2 | Exception | Throw/catch pass |
| 3 | Base Models | Encapsulation + compile |
| 4 | Event Inheritance | Abstract + 3 subclasses |
| 5 | Polymorphism | Runtime dispatch pass |
| 6 | Repository | Repository ↔ JSON |
| 7 | Customer | CRUD + persistence |
| 8 | Center/Hall | CRUD + validation |
| 9 | Authentication | Login pass |
| 10 | Authorization | Role permission pass |
| 11 | Event Service | Validation + persistence |
| 12 | Availability | Availability check |
| 13 | Conflict Logic | 5/5 cases pass |
| 14 | Booking Service | All booking rules pass |
| 15 | Booking UI | E2E booking pass |
| 16 | Package/Service | CRUD pass |
| 17 | Package-Service | Relationship + JSON |
| 18 | Booking-Service | Quantity calculation |
| 19 | Quotation | Calculation pass |
| 20 | Quotation Integration | Full quotation flow |
| 21 | Contract | Create + status |
| 22 | Payment | Payment rules pass |
| 23 | Payment Polymorphism | 3 methods dispatch |
| 24 | Operations | Staff/Table/Requirement/Incident |
| 25 | Completion + E2E | Core business flow pass |
| 26 | Search/Filter | Filters pass |
| 27 | Reports | 4 reports pass |
| 28 | Persistence/Integration | Restart + broken JSON |
| 29 | OOP + Regression | All checklist pass |
| 30 | Final Acceptance | Full demo pass |

---

# 4. EVIDENCE TEST — BẮT BUỘC

Không chỉ ghi:

```text
DAY 13 = DONE
```

Phải có evidence.

## DAY-13-TEST.txt

```text
[PASS] 18:00-22:00 vs 20:00-23:00 → CONFLICT
[PASS] 18:00-22:00 vs 16:00-19:00 → CONFLICT
[PASS] 18:00-22:00 vs 22:00-23:00 → NO CONFLICT
[PASS] 18:00-22:00 vs 14:00-18:00 → NO CONFLICT
[PASS] Cancelled booking vs 20:00-23:00 → NO CONFLICT
```

## DAY-19-TEST.txt

```text
Package       = 8,000,000
Services      = 8,500,000
Subtotal      = 16,500,000
Discount      = 1,000,000
Tax           = 1,550,000
Expected      = 17,050,000
Actual        = 17,050,000

[PASS] Quotation calculation
```

## DAY-23-TEST.txt

```text
[PASS] CashPayment::process()
[PASS] BankTransferPayment::process()
[PASS] CardPayment::process()
[PASS] PaymentMethod* runtime polymorphism
```

## DAY-28-TEST.txt

```text
[PASS] Save JSON
[PASS] Exit application
[PASS] Restart application
[PASS] Load JSON
[PASS] Customer restored
[PASS] Booking restored
[PASS] Contract restored
[PASS] Payment restored
[PASS] Invalid JSON handled
```

## DAY-30-E2E.txt

```text
[PASS] Login
[PASS] Customer
[PASS] Event Request
[PASS] Hall Availability
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

---

# 4.1. MA TRẬN CONSOLE UI + TESTING THEO NGÀY

| Ngày | Console UI | Testing bắt buộc |
|---:|---|---|
| 1 | Skeleton + Exit | Build, JSON, invalid JSON |
| 2 | Error handler | Exception + recovery |
| 3 | InputHelper | Invalid input |
| 4–5 | Event/polymorphism demo | Inheritance + runtime dispatch |
| 6 | UI → Service → Repository | Architecture boundary |
| 7 | Customer Menu | CRUD + validation + persistence |
| 8 | Center/Hall Menu | CRUD + validation |
| 9 | Login UI | Authentication |
| 10 | Main Menu + role | Authorization + regression |
| 11 | Event Menu | Event validation |
| 12 | Availability UI | Available/conflict |
| 13 | Conflict result UI | 5/5 overlap cases |
| 14 | Booking Menu | Booking rules |
| 15 | Booking flow | E2E + restart |
| 16–18 | Package/Service UI | CRUD + relationship + calculation |
| 19–20 | Quotation UI | Calculation + integration |
| 21 | Contract UI | Contract validation |
| 22–23 | Payment UI | Payment rules + polymorphism |
| 24–25 | Operations UI | Operations + E2E |
| 26 | Search UI | Search/filter/no-result |
| 27 | Report UI | 4 report outputs |
| 28 | Full navigation | Persistence + exception recovery |
| 29 | Full UI regression | OOP + business rules |
| 30 | Final demo UI | Acceptance + E2E |

**Quy tắc:** Feature chưa có test PASS thì chưa được đánh dấu 🟢 DONE.

# 5. CÁCH CHIA 30 NGÀY — KHÔNG ĐƯỢC NHẦM

Kế hoạch 30 ngày **không phải** lấy 56 ngày chia đôi.

Phần bị cắt trước tiên là:

```text
❌ CRUD UI quá chi tiết cho từng entity phụ
❌ Refactor nhiều vòng
❌ Feature nâng cao
❌ Báo cáo phức tạp
❌ Tối ưu không cần thiết
```

Nhưng **không được cắt**:

```text
✓ Event inheritance
✓ Runtime polymorphism
✓ Exception
✓ JSON persistence
✓ Repository
✓ Customer/Hall/Event/Booking
✓ Hall overlap
✓ Quotation
✓ Contract
✓ Payment
✓ Authentication/Authorization
✓ Operations tối thiểu
✓ Search/Report
✓ E2E test
```

Đặc biệt, vì đây là bài **OOP**, không nên hy sinh phần OOP để chạy theo số lượng chức năng. Một hệ thống có 15 menu nhưng không chứng minh được inheritance/polymorphism sẽ yếu hơn một hệ thống ít chức năng hơn nhưng OOP được thiết kế và demo rõ ràng.

---

# 6. NGUYÊN TẮC SINH TỒN TRONG 30 NGÀY

Từ **Day 20 trở đi: NO NEW FEATURE nếu core flow chưa ổn.**

Ưu tiên:

```text
CORE FLOW
   ↓
BUSINESS RULE
   ↓
JSON PERSISTENCE
   ↓
EXCEPTION
   ↓
OOP DEMO
   ↓
SEARCH/REPORT
   ↓
UI POLISH
```

Nếu bị trễ:

```text
Cắt UI đẹp
Cắt report nâng cao
Cắt feature phụ
```

**Không cắt:**

```text
Booking conflict
JSON save/load
Exception
Inheritance
Polymorphism
Payment validation
E2E
```

Mục tiêu cuối cùng:

```text
Requirement
    ↓
Class Design
    ↓
OOP
    ↓
Service
    ↓
Repository
    ↓
data.json
    ↓
Console UI
    ↓
Test
    ↓
FINAL DEMO
```
