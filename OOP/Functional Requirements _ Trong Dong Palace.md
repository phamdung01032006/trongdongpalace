Giả định:

- **Application type:** C++ Console Application
    
- **Không có GUI/Qt**
    
- Giao diện chỉ là **menu console + input/output text**
    
- Trọng tâm là **OOP + business logic + CRUD + validation + exception handling**
    
- Scope bám theo MVP trong kế hoạch: Customer, Hall, Event, Booking, Availability, Package/Service, Quotation, Contract, Payment, Operations, Event Completion và Basic Report.
    

---

# 1. Functional Requirements Specification

## 1.1. System Overview

**Trống Đồng Palace Event Management System** là ứng dụng console được xây dựng bằng C++ nhằm hỗ trợ quản lý các hoạt động tổ chức sự kiện và tiệc cưới tại Trống Đồng Palace.

Hệ thống hỗ trợ quy trình nghiệp vụ chính:

```text
Customer
   ↓
Event Request
   ↓
Check Hall
   ↓
Check Availability
   ↓
Package / Service
   ↓
Quotation
   ↓
Booking
   ↓
Contract
   ↓
Payment
   ↓
Operations
   ↓
Event Completion
   ↓
Report
```

Hệ thống được thiết kế theo hướng Object-Oriented Programming và có thể tổ chức theo các layer:

```text
Console UI
     ↓
Service Layer
     ↓
Domain / Model Layer
     ↓
Repository Layer
     ↓
Data Storage
```

---

# 2. Actors

Hệ thống bao gồm các nhóm người dùng theo kế hoạch dự án:

|Actor|Vai trò|
|---|---|
|Management|Quản lý hoạt động, theo dõi Booking, Event và Report|
|Sales Staff|Quản lý Customer, Event, Booking, Quotation và Contract|
|Accountant|Quản lý Payment|
|Coordinator|Điều phối Event và phân công nhân sự|
|Operations|Quản lý quá trình vận hành và hoàn tất Event|
|Administrator|Quản lý User và dữ liệu hệ thống|

---

# 3. Authentication

## FR-AUTH-001 – Login

**Description:**  
Hệ thống phải cho phép người dùng đăng nhập bằng tài khoản đã được tạo.

**Input:**

- Username
    
- Password
    

**Processing:**

1. Người dùng nhập Username.
    
2. Người dùng nhập Password.
    
3. Hệ thống kiểm tra thông tin đăng nhập.
    
4. Nếu thông tin hợp lệ, hệ thống xác định Role của người dùng.
    
5. Hệ thống hiển thị Main Menu.
    

**Output:**

```text
Login successful.
Welcome, Nguyen Van A.

Your role: Sales Staff
```

**Exception:**

```text
Invalid username or password.
Please try again.
```

**Priority:** Must Have

---

## FR-AUTH-002 – Role Authorization

Hệ thống phải kiểm soát quyền truy cập dựa trên Role.

Ví dụ:

|Chức năng|Management|Sales|Accountant|Coordinator|Operations|Admin|
|---|--:|--:|--:|--:|--:|--:|
|Customer|R|CRUD|R|R|R|CRUD|
|Hall|R|R|R|CRUD|R|CRUD|
|Event|R|CRUD|R|CRUD|CRUD|CRUD|
|Booking|R|CRUD|R|R|R|CRUD|
|Quotation|R|CRUD|R|R|R|CRUD|
|Contract|R|CRUD|R|R|R|CRUD|
|Payment|R|R|CRUD|R|R|CRUD|
|Operations|R|R|R|CRUD|CRUD|CRUD|
|Report|R|R|R|R|R|CRUD|

Trong đó:

- C = Create
    
- R = Read
    
- U = Update
    
- D = Delete
    

---

# 4. Customer Management

## FR-CUS-001 – Create Customer

Hệ thống phải cho phép người dùng có quyền tạo Customer.

**Input:**

- Customer ID
    
- Customer Name
    
- Phone
    
- Email
    
- Address
    

**Processing:**

1. Nhập thông tin Customer.
    
2. Validate dữ liệu.
    
3. Kiểm tra Customer ID không tồn tại.
    
4. Tạo Customer.
    
5. Lưu Customer.
    

**Output:**

```text
Customer created successfully.

Customer ID: C001
Name: Nguyen Van A
Phone: 0901234567
```

**Exception:**

- Customer ID đã tồn tại.
    
- Thiếu thông tin bắt buộc.
    
- Số điện thoại không hợp lệ.
    

**Priority:** Must Have

---

## FR-CUS-002 – View Customer

Hệ thống phải cho phép xem danh sách Customer.

Thông tin hiển thị:

```text
Customer ID
Customer Name
Phone
Email
Address
```

**Priority:** Must Have

---

## FR-CUS-003 – Search Customer

Hệ thống phải cho phép tìm Customer theo:

- Customer ID
    
- Customer Name
    
- Phone
    

**Output:**

Danh sách Customer phù hợp.

**Priority:** Must Have

---

## FR-CUS-004 – Update Customer

Người dùng có quyền có thể cập nhật thông tin Customer.

**Priority:** Must Have

---

## FR-CUS-005 – Delete Customer

Hệ thống cho phép xóa Customer nếu Customer chưa được sử dụng trong các Event/Booking liên quan.

Nếu Customer đã có dữ liệu liên quan, hệ thống phải từ chối xóa và hiển thị lý do.

**Priority:** Should Have

---

# 5. Hall Management

## FR-HALL-001 – Create Hall

Hệ thống cho phép người dùng có quyền tạo Hall.

**Input:**

- Hall ID
    
- Hall Name
    
- Center
    
- Capacity
    
- Status
    

**Priority:** Must Have

---

## FR-HALL-002 – View Hall

Hệ thống phải hiển thị danh sách Hall.

Ví dụ:

```text
+------+----------------+----------+--------+
| ID   | Hall Name      | Capacity | Status |
+------+----------------+----------+--------+
| H001 | Palace 1       | 500      | Active |
| H002 | Palace 2       | 700      | Active |
| H003 | Palace 3       | 300      | Closed |
+------+----------------+----------+--------+
```

---

## FR-HALL-003 – Update Hall

Người dùng có quyền có thể cập nhật:

- Hall Name
    
- Capacity
    
- Status
    

---

## FR-HALL-004 – Hall Status

Hall phải có trạng thái:

```text
Active
Inactive
```

Chỉ Hall có trạng thái `Active` mới được sử dụng để tạo Booking.

**Business Rule:** BR-HALL-001

**Priority:** Must Have

---

## FR-HALL-005 – Check Hall Availability

Hệ thống phải cho phép kiểm tra Hall có khả dụng trong một khoảng thời gian hay không.

**Input:**

```text
Hall ID
Start Date/Time
End Date/Time
```

**Output:**

```text
Hall H001 is AVAILABLE.
```

hoặc:

```text
Hall H001 is NOT AVAILABLE.

Existing Booking:
B001
18:00 - 22:00
```

**Priority:** Must Have

---

# 6. Event Management

## FR-EVENT-001 – Create Event

Hệ thống cho phép tạo Event.

**Input:**

- Event ID
    
- Customer
    
- Event Type
    
- Event Date
    
- Start Time
    
- End Time
    
- Guest Count
    

**Priority:** Must Have

---

## FR-EVENT-002 – Validate Event Time

Hệ thống phải đảm bảo:

```text
Start Time < End Time
```

Nếu không hợp lệ:

```text
Invalid event time.
Start time must be earlier than end time.
```

**Business Rule:** BR-EVENT-001

**Priority:** Must Have

---

## FR-EVENT-003 – View Event

Hệ thống phải cho phép xem danh sách và thông tin chi tiết Event.

Thông tin:

```text
Event ID
Customer
Event Type
Date
Start Time
End Time
Guest Count
Hall
Status
```

---

## FR-EVENT-004 – Update Event

Người dùng có quyền có thể cập nhật Event trước khi Event hoàn thành.

---

## FR-EVENT-005 – Event Status

Event có thể có các trạng thái:

```text
Planned
Preparing
In Progress
Completed
Cancelled
```

---

# 7. Booking Management

## FR-BOOK-001 – Create Booking

Hệ thống phải cho phép tạo Booking cho Event.

**Input:**

```text
Booking ID
Event ID
Hall ID
Start Time
End Time
```

**Processing:**

```text
Validate Event
      ↓
Validate Hall
      ↓
Check Hall Status
      ↓
Check Availability
      ↓
Check Conflict
      ↓
Create Booking
```

**Priority:** Must Have

---

## FR-BOOK-002 – Check Booking Conflict

Hệ thống phải phát hiện Booking bị overlap trên cùng Hall.

Điều kiện conflict:

```text
New Start < Existing End
AND
New End > Existing Start
```

Ví dụ:

```text
Existing Booking:
18:00 ---------------- 22:00

New Booking:
20:00 -------------------- 23:00

Result:
CONFLICT
```

Hệ thống không được tạo Booking khi có conflict.

**Business Rule:** BR-BOOK-001

**Priority:** Must Have

---

## FR-BOOK-003 – View Booking

Hệ thống phải hiển thị:

```text
Booking ID
Customer
Event
Hall
Start Time
End Time
Status
```

---

## FR-BOOK-004 – Search Booking

Có thể tìm Booking theo:

- Booking ID
    
- Customer
    
- Event
    
- Hall
    
- Date
    
- Status
    

---

## FR-BOOK-005 – Cancel Booking

Người dùng có quyền có thể Cancel Booking.

Khi Cancel:

```text
Booking Status = Cancelled
```

Booking bị Cancel không được xem là Booking đang chiếm Hall.

---

# 8. Package Management

## FR-PKG-001 – Create Package

Hệ thống cho phép tạo Package.

Thông tin:

```text
Package ID
Package Name
Description
Price
Status
```

---

## FR-PKG-002 – View Package

Hệ thống phải hiển thị danh sách Package.

Ví dụ:

```text
+------+----------------------+------------+
| ID   | Package              | Price      |
+------+----------------------+------------+
| P001 | Wedding Basic        | 5,000,000  |
| P002 | Wedding Premium      | 8,000,000  |
| P003 | Wedding Luxury       | 12,000,000 |
+------+----------------------+------------+
```

---

## FR-PKG-003 – Add Service to Package

Một Package có thể chứa nhiều Service.

Ví dụ:

```text
Wedding Premium
    |
    +-- Decoration
    +-- Sound
    +-- Lighting
    +-- Catering
```

---

# 9. Service Management

## FR-SVC-001 – Create Service

Hệ thống cho phép tạo Service.

Thông tin:

```text
Service ID
Service Name
Description
Unit
Unit Price
Status
```

---

## FR-SVC-002 – View Service

Hệ thống phải hiển thị danh sách Service.

---

## FR-SVC-003 – Add Service to Booking

Người dùng có quyền có thể thêm Service vào Booking.

Thành tiền:

```text
Amount = Quantity × Unit Price
```

---

# 10. Quotation Management

## FR-QUO-001 – Create Quotation

Hệ thống phải cho phép tạo Quotation dựa trên Booking.

Quotation bao gồm:

```text
Quotation ID
Booking
Package
Services
Subtotal
Discount
Tax
Total
```

---

## FR-QUO-002 – Calculate Quotation

Hệ thống phải tự động tính:

```text
Subtotal
= Package Amount + Service Amount

Total
= Subtotal - Discount + Tax
```

Hệ thống không cho phép người dùng tùy ý nhập Total.

---

## FR-QUO-003 – Update Quotation

Người dùng có quyền có thể:

- Thêm/xóa Service
    
- Thay đổi Quantity
    
- Thay đổi Package
    
- Áp dụng Discount
    

Hệ thống phải tự động tính lại Total.

---

## FR-QUO-004 – View Quotation

Hệ thống phải hiển thị chi tiết Quotation.

Ví dụ:

```text
============================================
              QUOTATION Q001
============================================
Customer       : Nguyen Van A
Event          : Wedding
Hall           : Palace 1

Package        : Wedding Premium
Package Price  : 8,000,000

Services:
1. Decoration      x1       5,000,000
2. Sound           x1       2,000,000
3. Lighting        x1       1,500,000

--------------------------------------------
Subtotal                  : 16,500,000
Discount                  : 1,000,000
Tax                       : 1,550,000
--------------------------------------------
TOTAL                     : 17,050,000
============================================
```

---

# 11. Contract Management

## FR-CON-001 – Create Contract

Hệ thống cho phép tạo Contract từ Quotation.

```text
Quotation
    ↓
Create Contract
```

Contract bao gồm:

```text
Contract ID
Customer
Event
Booking
Quotation
Contract Total
Status
```

---

## FR-CON-002 – View Contract

Hệ thống phải cho phép xem thông tin Contract.

---

## FR-CON-003 – Contract Status

Contract có thể có trạng thái:

```text
Draft
Active
Completed
Cancelled
```

---

# 12. Payment Management

## FR-PAY-001 – Record Payment

Accountant có thể ghi nhận Payment.

Thông tin:

```text
Payment ID
Contract ID
Payment Date
Amount
Payment Method
Note
```

---

## FR-PAY-002 – Validate Payment

Hệ thống không cho phép:

```text
Payment > Contract Total
```

hoặc:

```text
Total Paid > Contract Total
```

**Business Rule:** BR-PAY-001

---

## FR-PAY-003 – Validate Deposit

Deposit không được lớn hơn Contract Total.

**Business Rule:** BR-PAY-002

---

## FR-PAY-004 – Calculate Outstanding

Hệ thống tự động tính:

```text
Outstanding
=
Contract Total - Total Paid
```

Ví dụ:

```text
Contract Total : 100,000,000
Total Paid     : 60,000,000
Outstanding    : 40,000,000
```

---

## FR-PAY-005 – View Payment History

Hệ thống hiển thị:

```text
Payment ID
Date
Amount
Method
Note
```

---

# 13. Event Operations

## FR-OPS-001 – Assign Staff

Coordinator có thể phân công nhân sự cho Event.

Thông tin:

```text
Event
Staff
Role
```

Ví dụ:

```text
Event: Wedding E001

Coordinator:
    Nguyen Van A

Operations:
    Tran Van B

Support:
    Le Van C
```

---

## FR-OPS-002 – Manage Special Request

Hệ thống cho phép ghi nhận yêu cầu đặc biệt của khách hàng.

Thông tin:

```text
Request ID
Event ID
Description
Status
Note
```

---

## FR-OPS-003 – Manage Incident

Operations có thể ghi nhận sự cố trong Event.

Thông tin:

```text
Incident ID
Event
Description
Severity
Status
Resolution
```

---

## FR-OPS-004 – Update Operation Status

Operations có thể cập nhật:

```text
Planned
Preparing
In Progress
Completed
```

---

# 14. Event Completion

## FR-EOC-001 – Complete Event

Sau khi Event kết thúc, Operations/Coordinator có thể đánh dấu Event là Completed.

Hệ thống phải kiểm tra Event đang ở trạng thái phù hợp.

---

## FR-EOC-002 – Record Completion Information

Hệ thống có thể lưu:

```text
Actual Guest Count
Operation Note
Incident
Special Request Result
```

---

# 15. Reporting

## FR-REP-001 – Booking Report

Hệ thống phải cung cấp Booking Report.

Ví dụ:

```text
============================================
              BOOKING REPORT
============================================
Booking ID     Customer        Hall
--------------------------------------------
B001           Nguyen A        Palace 1
B002           Tran B          Palace 2
B003           Le C            Palace 1
============================================
```

Có thể lọc theo:

- Date
    
- Hall
    
- Status
    

---

## FR-REP-002 – Event Report

Báo cáo:

```text
Total Events
Events by Type
Events by Hall
Events by Status
```

---

## FR-REP-003 – Payment Report

Báo cáo:

```text
Contract Total
Total Paid
Outstanding
```

---

## FR-REP-004 – Revenue Report

Báo cáo doanh thu cơ bản:

```text
Total Contract Value
Total Paid
Total Outstanding
```

Có thể lọc theo thời gian.

---

# 16. Data Management

## FR-DATA-001 – Save Data

Hệ thống phải lưu dữ liệu của các đối tượng chính:

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

Theo kế hoạch, phần Persistence có thể sử dụng **SQLite hoặc JSON/CSV**.

---

## FR-DATA-002 – Load Data

Khi khởi động chương trình, hệ thống phải đọc dữ liệu đã lưu và khôi phục trạng thái cần thiết.

---

## FR-DATA-003 – Repository

Các Service không truy cập trực tiếp Persistence.

Kiến trúc:

```text
Console
   ↓
Service
   ↓
Repository
   ↓
Data Storage
```

---

# 17. Console Interface Requirements

Vì hệ thống chỉ sử dụng Console, giao diện phải được tổ chức theo menu phân cấp.

## FR-CONSOLE-001 – Main Menu

Sau khi Login thành công:

```text
==================================================
          TRONG DONG PALACE MANAGEMENT
==================================================
Logged in as: Nguyen Van A
Role        : Sales Staff
--------------------------------------------------
1. Customer Management
2. Hall Management
3. Event Management
4. Booking Management
5. Package Management
6. Service Management
7. Quotation Management
8. Contract Management
9. Payment Management
10. Event Operations
11. Reports
12. Logout
--------------------------------------------------
Enter your choice:
```

---

## FR-CONSOLE-002 – Module Menu

Mỗi module phải có menu riêng.

Ví dụ Customer:

```text
==================================================
              CUSTOMER MANAGEMENT
==================================================
1. Create Customer
2. View All Customers
3. Search Customer
4. Update Customer
5. Delete Customer
6. Back
--------------------------------------------------
Enter your choice:
```

---

## FR-CONSOLE-003 – Input Validation

Hệ thống phải kiểm tra dữ liệu đầu vào.

Ví dụ:

```text
Enter Guest Count: -10

[ERROR]
Guest count must be greater than 0.

Please enter again:
```

---

## FR-CONSOLE-004 – Error Handling

Khi xảy ra lỗi nghiệp vụ, hệ thống phải hiển thị thông báo rõ ràng.

Ví dụ:

```text
[ERROR] Booking Conflict

Hall H001 is already booked
from 18:00 to 22:00.

Booking creation failed.
```

Hệ thống không được kết thúc chương trình đột ngột vì lỗi nghiệp vụ.

---

## FR-CONSOLE-005 – Operation Confirmation

Đối với thao tác quan trọng như Delete hoặc Cancel, hệ thống nên yêu cầu xác nhận.

Ví dụ:

```text
Are you sure you want to cancel Booking B001?

1. Yes
2. No

Enter your choice:
```

---

# 18. Core Business Rules

|Rule ID|Rule|
|---|---|
|BR-HALL-001|Hall phải tồn tại và Active mới được sử dụng để Booking|
|BR-EVENT-001|Event Start Time phải nhỏ hơn End Time|
|BR-BOOK-001|Không được có Booking overlap trên cùng Hall|
|BR-PAY-001|Tổng Payment không được vượt Contract Total|
|BR-PAY-002|Deposit không được vượt Contract Total|

Các Business Rules này phải được enforce ở **Business/Service Layer**, không chỉ kiểm tra ở Console. Điều này phù hợp với kiến trúc OOP được định hướng trong kế hoạch.

---

# 19. End-to-End Functional Flow

Hệ thống phải hỗ trợ được flow chính:

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
Select Package / Service
  ↓
Create Quotation
  ↓
Create Contract
  ↓
Record Payment
  ↓
Assign Staff
  ↓
Event Operations
  ↓
Complete Event
  ↓
Generate Report
```

Đây là flow quan trọng nhất để sử dụng trong **Integration Test và Final Demo**.

---

# 20. MVP Scope

## Must Have

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
Basic Report
Console Menu
Data Persistence
Validation
Exception Handling
Audit log
```

## Should Have

```text
Search nâng cao
Special Request
Incident
Payment Method Polymorphism
Advanced Report
```

## Out of Scope

```text
GUI
Qt
Mobile Application
Online Payment Gateway
AI
Cloud Deployment
Microservices
External CRM
External Accounting System
```

Việc giới hạn scope như trên phù hợp với mục tiêu MVP trong kế hoạch: ưu tiên hoàn thành flow nghiệp vụ end-to-end trước, sau đó tập trung testing, refactoring và documentation.

---

# 21. Expected Console Structure

Cấu trúc chương trình ở mức người dùng có thể được thiết kế:

```text
Application
│
├── Login
│
└── Main Menu
    │
    ├── Customer Management
    │   ├── Create
    │   ├── View
    │   ├── Search
    │   ├── Update
    │   └── Delete
    │
    ├── Hall Management
    │   ├── Create
    │   ├── View
    │   ├── Update
    │   └── Availability
    │
    ├── Event Management
    │   ├── Create
    │   ├── View
    │   ├── Search
    │   └── Update
    │
    ├── Booking Management
    │   ├── Create
    │   ├── View
    │   ├── Search
    │   └── Cancel
    │
    ├── Package Management
    │
    ├── Service Management
    │
    ├── Quotation Management
    │
    ├── Contract Management
    │
    ├── Payment Management
    │
    ├── Event Operations
    │
    └── Reports
```

# 22. Priority Summary

|Module|Priority|
|---|---|
|Authentication|Must Have|
|Customer|Must Have|
|Hall|Must Have|
|Event|Must Have|
|Availability|Must Have|
|Booking|Must Have|
|Package|Must Have|
|Service|Must Have|
|Quotation|Must Have|
|Contract|Must Have|
|Payment|Must Have|
|Operations|Must Have|
|Event Completion|Must Have|
|Basic Report|Must Have|
|Special Request|Should Have|
|Incident|Should Have|
|Advanced Report|Should Have|
|GUI|Out of Scope|

### Giao diện console mẫu tổng thể

Mình khuyên bạn **không cần làm console quá phức tạp**. Chỉ cần menu rõ ràng, có header, separator và thông báo thành công/lỗi là đủ đẹp cho một project OOP.

**1. Login**

```text
==================================================
              TRONG DONG PALACE
          EVENT MANAGEMENT SYSTEM
==================================================

Username: admin
Password: ********

Login successful!

Welcome, Admin
Role: Administrator

Press Enter to continue...
```

**2. Main Menu**

```text
==================================================
          TRONG DONG PALACE MANAGEMENT
==================================================
User: Nguyen Van A
Role: Sales Staff
==================================================

1. Customer Management
2. Hall Management
3. Event Management
4. Booking Management
5. Package Management
6. Service Management
7. Quotation Management
8. Contract Management
9. Payment Management
10. Event Operations
11. Reports
12. Logout

--------------------------------------------------
Enter your choice: 4
```

**3. Booking Management**

```text
==================================================
              BOOKING MANAGEMENT
==================================================

1. Create Booking
2. View All Bookings
3. Search Booking
4. Check Hall Availability
5. Cancel Booking
6. Back

--------------------------------------------------
Enter your choice: 1
```

**4. Create Booking – thành công**

```text
==================================================
                CREATE BOOKING
==================================================

Enter Booking ID : B001
Enter Event ID   : E001
Enter Hall ID    : H002

Start Time       : 18:00
End Time         : 22:00

Checking Hall Availability...

Hall H002 is AVAILABLE.

--------------------------------------------------
Booking Information
--------------------------------------------------
Booking ID : B001
Event      : E001 - Wedding
Customer   : C001 - Nguyen Van A
Hall       : H002 - Palace 2
Time       : 18:00 - 22:00
--------------------------------------------------

Booking created successfully!

Press Enter to continue...
```

**5. Create Booking – conflict**

```text
==================================================
                CREATE BOOKING
==================================================

Enter Booking ID : B002
Enter Event ID   : E002
Enter Hall ID    : H002

Start Time       : 20:00
End Time         : 23:00

Checking Hall Availability...

==================================================
                  BOOKING CONFLICT
==================================================

Hall H002 is NOT AVAILABLE.

Existing Booking:
Booking ID : B001
Time       : 18:00 - 22:00

Requested Time:
20:00 - 23:00

--------------------------------------------------
Booking creation failed.
Please select another Hall or Time.
==================================================

Press Enter to continue...
```

**6. Quotation**

```text
==================================================
                 QUOTATION
==================================================

Quotation ID : Q001
Booking ID   : B001
Customer     : Nguyen Van A
Event        : Wedding
Hall         : Palace 2

--------------------------------------------------
Package / Services
--------------------------------------------------
1. Wedding Premium       8,000,000
2. Decoration            5,000,000
3. Sound                 2,000,000
4. Lighting              1,500,000

--------------------------------------------------
Subtotal                : 16,500,000
Discount                :  1,000,000
Tax                     :  1,550,000
--------------------------------------------------
TOTAL                   : 17,050,000
==================================================
```

**7. Payment**

```text
==================================================
                PAYMENT
==================================================

Contract ID     : CT001
Contract Total  : 100,000,000
Total Paid      : 60,000,000
Outstanding     : 40,000,000

--------------------------------------------------

Enter Payment Amount : 20,000,000

Payment Method:
1. Cash
2. Bank Transfer
3. Card

Choose method: 2

Payment recorded successfully.

--------------------------------------------------
New Payment:
20,000,000

Total Paid:
80,000,000

Outstanding:
20,000,000
--------------------------------------------------
```

**8. Report**

```text
==================================================
              REVENUE REPORT
==================================================

Period: September 2026

--------------------------------------------------
Total Events           : 25
Total Bookings         : 23
Completed Events       : 18
Cancelled Bookings     : 2

--------------------------------------------------
Total Contract Value   : 850,000,000
Total Paid             : 620,000,000
Outstanding            : 230,000,000
==================================================
```

Như vậy, **Console UI chỉ là lớp giao tiếp**, còn phần xử lý phải nằm phía dưới:

```text
Console Menu
     │
     ▼
BookingService
     │
     ├── validateEvent()
     ├── validateHall()
     ├── checkAvailability()
     ├── checkConflict()
     └── createBooking()
              │
              ▼
       BookingRepository
              │
              ▼
          Data Store
```

Đây là cách làm khá phù hợp với project của bạn vì vừa **đơn giản để code trong 9 tuần**, vừa có đủ đất để thể hiện **Encapsulation, Abstraction, Inheritance, Polymorphism, Exception Handling và SOLID** như mục tiêu OOP trong kế hoạch.

**Bước tiếp theo hợp lý:** từ bản FR này, mình có thể tách thành **danh sách Use Case chính (UC-xxx) + Actor + Pre-condition + Main Flow + Alternative/Exception Flow** để bạn dùng trực tiếp cho phần _System Analysis_.