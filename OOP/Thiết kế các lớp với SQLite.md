Đúng. Với mindset **C++ + OOP + SQLite + Console GUI**, mình sẽ thiết kế lại theo hướng **thực tế để code được**, chứ không thiết kế theo kiểu UML quá lý thuyết.

Điểm quan trọng là:

> **Console chỉ là Presentation Layer. SQLite chỉ là Persistence Layer. Business logic nằm ở Service Layer. Các nghiệp vụ chính được mô hình hóa bằng Domain Objects.**

Vì vậy mình sẽ **không để Console xử lý nghiệp vụ**, và cũng **không để các Entity tự truy vấn SQLite**.

---

# 2. Thiết kế lớp và đối tượng

## 2.1. Định hướng kiến trúc

Hệ thống được xây dựng bằng **C++**, áp dụng **Object-Oriented Programming**, sử dụng **SQLite** để lưu trữ dữ liệu và **Console** làm giao diện người dùng.

Kiến trúc đề xuất:

```text
┌──────────────────────────────────────────────┐
│              CONSOLE UI LAYER                │
│                                              │
│  MainMenu                                    │
│  CustomerMenu                                │
│  EventMenu                                   │
│  BookingMenu                                 │
│  QuotationMenu                               │
│  ContractMenu                                │
│  PaymentMenu                                 │
│  ReportMenu                                  │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
┌──────────────────────────────────────────────┐
│              SERVICE LAYER                   │
│                                              │
│ CustomerService                              │
│ EventService                                 │
│ HallService                                  │
│ BookingService                               │
│ QuotationService                             │
│ ContractService                              │
│ PaymentService                               │
│ OperationService                             │
│ ReportService                                │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
┌──────────────────────────────────────────────┐
│             DOMAIN / MODEL LAYER             │
│                                              │
│ Customer   Event      Hall      Booking      │
│ Package    Service    Quotation Contract     │
│ Payment    Staff      Assignment ...         │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
┌──────────────────────────────────────────────┐
│             REPOSITORY LAYER                 │
│                                              │
│ CustomerRepository                           │
│ EventRepository                              │
│ HallRepository                               │
│ BookingRepository                            │
│ ...                                          │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
┌──────────────────────────────────────────────┐
│             SQLITE DATABASE                  │
│                                              │
│ customers                                    │
│ halls                                        │
│ events                                       │
│ bookings                                     │
│ packages                                     │
│ services                                     │
│ quotations                                   │
│ contracts                                    │
│ payments                                     │
│ ...                                          │
└──────────────────────────────────────────────┘
```

Đây sẽ là **kiến trúc chính thức** mình khuyến nghị cho project.

---

# 2.2. Nguyên tắc phân chia trách nhiệm

Mình muốn chốt nguyên tắc này trước khi đi vào từng class.

### Console UI làm gì?

Chỉ:

- Hiển thị menu
    
- Nhận input
    
- Gọi Service
    
- Hiển thị kết quả
    
- Hiển thị error message
    

Không làm:

- SQL
    
- Business calculation
    
- Booking conflict detection
    
- Payment validation
    

---

### Domain Object làm gì?

Đại diện cho đối tượng nghiệp vụ:

```text
Customer
Event
Hall
Booking
Package
Service
Quotation
Contract
Payment
```

Chúng chứa:

- Data
    
- Các behavior đơn giản thuộc về chính object
    

---

### Service làm gì?

Service xử lý **business logic**.

Ví dụ:

```text
BookingService
    ├── validateBooking()
    ├── checkHallAvailability()
    ├── checkConflict()
    └── createBooking()
```

---

### Repository làm gì?

Repository chịu trách nhiệm:

```text
Object ↔ SQLite
```

Ví dụ:

```text
BookingService
      ↓
BookingRepository
      ↓
SQLite
```

Service **không viết SQL**.

---

# 2.3. Domain Object Design

Mình đề xuất khoảng **12–14 domain classes** là hợp lý cho project.

Không nên cố tạo quá nhiều class vì sẽ khiến project C++ bị phức tạp không cần thiết.

---

## 2.3.1. User

Quản lý tài khoản đăng nhập.

```text
User
--------------------------------
- id              : int
- username        : string
- passwordHash    : string
- role            : UserRole
- status          : UserStatus
```

Methods:

```text
+ authenticate()
+ hasPermission()
+ isActive()
```

Trong C++:

```cpp
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
```

---

# 2.3.2. Customer

```text
Customer
--------------------------------
- id              : int
- customerCode    : string
- name            : string
- phone           : string
- email           : string
- address         : string
```

Methods:

```text
+ updateInfo()
+ isValid()
```

### Quan hệ

```text
Customer 1 ───────── * Event
```

Một khách hàng có thể có nhiều Event.

---

# 2.3.3. Center

Nếu hệ thống chỉ quản lý **một Trống Đồng Palace**, `Center` thực ra không cần quá phức tạp.

Tuy nhiên, nếu kế hoạch của project đã có entity Center thì vẫn giữ.

```text
Center
--------------------------------
- id              : int
- name            : string
- address         : string
- phone           : string
```

Quan hệ:

```text
Center 1 ───────── * Hall
```

---

# 2.3.4. Hall

```text
Hall
--------------------------------
- id              : int
- centerId        : int
- hallCode        : string
- name            : string
- capacity        : int
- status          : HallStatus
```

Methods:

```text
+ isActive()
+ canAccommodate(guestCount)
```

Enum:

```cpp
enum class HallStatus {
    ACTIVE,
    INACTIVE
};
```

**Không nên để Hall tự kiểm tra toàn bộ availability**, vì availability phụ thuộc vào Booking trong database.

Do đó:

```text
Hall
    ↓
isActive()
```

nhưng:

```text
Hall Availability
    ↓
BookingService
```

Đây là một phân chia trách nhiệm quan trọng.

---

# 2.3.5. Event

Đây là **core domain object**.

```text
Event
--------------------------------
- id              : int
- eventCode       : string
- customerId      : int
- eventType       : EventType
- eventDate       : Date
- startTime       : Time
- endTime         : Time
- guestCount      : int
- status          : EventStatus
```

Methods:

```text
+ validateTime()
+ validateGuestCount()
+ changeStatus()
+ isCompleted()
```

Enum:

```cpp
enum class EventStatus {
    PLANNED,
    PREPARING,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};
```

### Tại sao Event chỉ giữ `customerId`?

Với C++ + SQLite, mình khuyên **Domain Entity giữ ID/FK thay vì giữ quá nhiều object lồng nhau**.

Ví dụ:

```cpp
class Event {
private:
    int id;
    int customerId;
    int hallId;
};
```

thay vì:

```cpp
class Event {
private:
    Customer customer;
    Hall hall;
    Booking booking;
    ...
};
```

Cách thứ hai dễ tạo:

- circular dependency
    
- object graph rất lớn
    
- khó mapping SQLite
    
- khó update dữ liệu
    

Đối với project này, **ID-based relationship phù hợp hơn**.

---

# 2.3.6. Booking

Booking là một domain object quan trọng.

```text
Booking
--------------------------------
- id              : int
- bookingCode     : string
- eventId         : int
- hallId          : int
- startDateTime   : DateTime
- endDateTime     : DateTime
- status          : BookingStatus
```

Enum:

```cpp
enum class BookingStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};
```

Methods:

```text
+ cancel()
+ isActive()
+ overlaps()
```

Trong đó:

```cpp
bool Booking::overlaps(
    DateTime newStart,
    DateTime newEnd
);
```

có thể xử lý logic overlap cơ bản.

Nhưng **việc tìm các Booking khác trong database** phải do:

```text
BookingService
```

xử lý.

---

# 2.3.7. Package

```text
Package
--------------------------------
- id              : int
- packageCode     : string
- name            : string
- description     : string
- basePrice       : double
- status          : PackageStatus
```

Methods:

```text
+ activate()
+ deactivate()
+ updatePrice()
```

---

# 2.3.8. Service

Tên `Service` trong C++ có thể gây nhầm với các `Service class`.

Vì vậy mình khuyến nghị trong code gọi entity này là:

```text
EventServiceItem
```

hoặc:

```text
ServiceItem
```

để tránh nhầm với:

```text
BookingService
PaymentService
```

Thiết kế:

```text
ServiceItem
--------------------------------
- id              : int
- serviceCode     : string
- name            : string
- description     : string
- unit            : string
- unitPrice       : double
- status          : ServiceStatus
```

Methods:

```text
+ calculateAmount(quantity)
+ updatePrice()
```

---

# 2.3.9. PackageServiceItem

Đây là class mình **thêm vào thiết kế** vì khi triển khai SQLite, quan hệ Package–Service là **many-to-many**.

```text
Package
   *
   |
   |
   *
PackageServiceItem
   *
   |
   |
   *
ServiceItem
```

Class:

```text
PackageServiceItem
--------------------------------
- packageId       : int
- serviceId       : int
- defaultQuantity : int
```

Database tương ứng:

```text
packages
services
package_services
```

Ví dụ:

```text
Wedding Premium
      |
      +---- Decoration
      +---- Sound
      +---- Lighting
      +---- Catering
```

Đây là thiết kế **rất phù hợp với SQLite**.

---

# 2.3.10. Quotation

```text
Quotation
--------------------------------
- id              : int
- quotationCode   : string
- bookingId       : int
- subtotal        : double
- discount        : double
- tax             : double
- total           : double
- status          : QuotationStatus
```

Methods:

```text
+ calculateSubtotal()
+ calculateTotal()
+ applyDiscount()
```

---

# 2.3.11. QuotationItem

Mình khuyên **bắt buộc nên có class này**.

```text
QuotationItem
--------------------------------
- id              : int
- quotationId     : int
- serviceId       : int
- description     : string
- quantity        : int
- unitPrice       : double
- amount          : double
```

Quan hệ:

```text
Quotation 1 ─────── * QuotationItem
                         |
                         |
                         1
                    ServiceItem
```

Database:

```text
quotations
quotation_items
```

Điều này giúp quotation có snapshot giá tại thời điểm báo giá.

Ví dụ:

```text
Service current price = 2,500,000

Quotation created:
unitPrice = 2,000,000
```

Sau này Service tăng giá lên 3 triệu thì **Quotation cũ vẫn giữ 2 triệu**.

Đây là một quyết định thiết kế rất quan trọng.

---

# 2.3.12. Contract

```text
Contract
--------------------------------
- id              : int
- contractCode    : string
- quotationId     : int
- customerId      : int
- eventId         : int
- totalAmount     : double
- status          : ContractStatus
- contractDate    : Date
```

Methods:

```text
+ activate()
+ cancel()
+ complete()
```

Contract không cần chứa trực tiếp:

```cpp
Quotation quotation;
Customer customer;
Event event;
```

mà giữ:

```cpp
int quotationId;
int customerId;
int eventId;
```

Sau đó Service/Repository lấy object khi cần.

---

# 2.3.13. Payment

Đây là chỗ mình sẽ thiết kế khác bản trước một chút để phù hợp SQLite.

```text
Payment
--------------------------------
- id              : int
- paymentCode     : string
- contractId      : int
- amount          : double
- paymentDate     : DateTime
- method          : PaymentMethod
- note            : string
```

Enum:

```cpp
enum class PaymentMethod {
    CASH,
    BANK_TRANSFER,
    CARD
};
```

**MVP chưa cần tạo `CashPayment`, `CardPayment`, `BankTransferPayment` thành 3 bảng/class riêng.**

Nếu mục tiêu chính là SQLite + Console + OOP, enum là đơn giản và phù hợp hơn.

Nếu giáo viên **bắt buộc phải demonstrate inheritance/polymorphism**, lúc đó mới đưa Strategy/PaymentMethod hierarchy vào.

---

# 2.3.14. Staff

```text
Staff
--------------------------------
- id              : int
- staffCode       : string
- name            : string
- phone           : string
- role            : StaffRole
- status          : StaffStatus
```

---

# 2.3.15. StaffAssignment

Không nên:

```text
Event
  |
  +---- vector<Staff>
```

Mà dùng association object:

```text
StaffAssignment
--------------------------------
- id              : int
- eventId         : int
- staffId         : int
- assignmentRole  : string
- status          : AssignmentStatus
```

Quan hệ:

```text
Event 1 ───── * StaffAssignment * ───── 1 Staff
```

SQLite:

```text
events
staff
staff_assignments
```

---

# 2.3.16. SpecialRequest

```text
SpecialRequest
--------------------------------
- id              : int
- eventId         : int
- description     : string
- status          : RequestStatus
- note            : string
```

---

# 2.3.17. Incident

```text
Incident
--------------------------------
- id              : int
- eventId         : int
- description     : string
- severity        : IncidentSeverity
- status          : IncidentStatus
- resolution      : string
```

---

# 2.4. Service Layer

Đây mới là nơi xử lý business workflow.

## CustomerService

```text
CustomerService
--------------------------------
+ createCustomer()
+ updateCustomer()
+ deleteCustomer()
+ findCustomer()
+ getAllCustomers()
```

---

## HallService

```text
HallService
--------------------------------
+ createHall()
+ updateHall()
+ getAllHalls()
+ checkAvailability()
```

---

## EventService

```text
EventService
--------------------------------
+ createEvent()
+ updateEvent()
+ cancelEvent()
+ completeEvent()
```

---

## BookingService

Đây là Service quan trọng nhất.

```text
BookingService
--------------------------------
+ createBooking()
+ cancelBooking()
+ findBooking()
+ checkAvailability()
+ checkConflict()
```

Flow:

```text
Console
   ↓
BookingService
   ↓
1. Validate Event
   ↓
2. Validate Hall
   ↓
3. Check Hall Active
   ↓
4. Query SQLite for conflicting bookings
   ↓
5. Create Booking
   ↓
BookingRepository
   ↓
SQLite
```

---

## QuotationService

```text
QuotationService
--------------------------------
+ createQuotation()
+ addItem()
+ removeItem()
+ calculateSubtotal()
+ calculateTotal()
```

---

## ContractService

```text
ContractService
--------------------------------
+ createContract()
+ activateContract()
+ cancelContract()
+ completeContract()
```

---

## PaymentService

```text
PaymentService
--------------------------------
+ createPayment()
+ validatePayment()
+ getTotalPaid()
+ getOutstanding()
```

Business rule:

```text
Total Paid <= Contract Total
```

---

## OperationService

```text
OperationService
--------------------------------
+ assignStaff()
+ addSpecialRequest()
+ addIncident()
+ updateEventStatus()
+ completeEvent()
```

---

## ReportService

```text
ReportService
--------------------------------
+ bookingReport()
+ eventReport()
+ paymentReport()
+ revenueReport()
```

---

# 2.5. Repository Layer

Với SQLite, Repository là phần rất quan trọng.

Mình đề xuất có một SQLite connection/database class chung:

```text
Database
--------------------------------
- sqlite3* connection

+ open()
+ close()
+ execute()
+ prepare()
+ beginTransaction()
+ commit()
+ rollback()
```

Sau đó:

```text
CustomerRepository
--------------------------------
+ save()
+ update()
+ delete()
+ findById()
+ findAll()
```

Tương tự:

```text
HallRepository
EventRepository
BookingRepository
PackageRepository
ServiceRepository
QuotationRepository
ContractRepository
PaymentRepository
StaffRepository
```

---

# 2.6. Không cho Domain truy cập SQLite

Đây là nguyên tắc mình muốn **chốt cứng** cho project.

Không làm:

```cpp
class Booking {
public:
    void saveToDatabase();
};
```

và cũng không làm:

```cpp
class Booking {
public:
    void loadFromSQLite();
};
```

Thay vào đó:

```text
Booking
   ↑
   |
BookingRepository
   ↑
   |
SQLite
```

Domain object không biết SQLite tồn tại.

Điều này làm cho code OOP sạch hơn rất nhiều.

---

# 2.7. Console UI

Vì đây là **Console GUI**, mình sẽ không gọi nó là GUI Layer theo nghĩa Qt.

Có thể gọi chính xác hơn:

> **Console Presentation Layer**

Cấu trúc:

```text
ConsoleApp
│
├── MainMenu
│
├── CustomerMenu
├── HallMenu
├── EventMenu
├── BookingMenu
├── PackageMenu
├── QuotationMenu
├── ContractMenu
├── PaymentMenu
├── OperationMenu
└── ReportMenu
```

Ví dụ:

```cpp
class BookingMenu {
private:
    BookingService& bookingService;

public:
    void show();
    void createBooking();
    void searchBooking();
    void cancelBooking();
};
```

Menu chỉ:

```text
Input
 ↓
Service
 ↓
Result
 ↓
Output
```

---

# 2.8. Dependency Injection đơn giản

Không cần làm Dependency Injection quá phức tạp.

Có thể khởi tạo ở `main.cpp`:

```text
main()
 |
 +-- Database
 |
 +-- Repositories
 |     |
 |     +-- CustomerRepository
 |     +-- HallRepository
 |     +-- BookingRepository
 |
 +-- Services
 |     |
 |     +-- CustomerService
 |     +-- HallService
 |     +-- BookingService
 |
 +-- Menus
       |
       +-- CustomerMenu
       +-- BookingMenu
```

Ví dụ:

```cpp
int main() {

    Database db("trongdong.db");

    CustomerRepository customerRepo(db);
    HallRepository hallRepo(db);
    BookingRepository bookingRepo(db);

    CustomerService customerService(customerRepo);
    HallService hallService(hallRepo);
    BookingService bookingService(
        bookingRepo,
        hallRepo
    );

    MainMenu menu(
        customerService,
        hallService,
        bookingService
    );

    menu.show();

    return 0;
}
```

Cách này rất phù hợp với một project C++ OOP quy mô sinh viên.

---

# 2.9. Class Diagram logic

Nếu gom lại, architecture sẽ như sau:

```text
                    ┌─────────────────┐
                    │   Console UI    │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │    Services     │
                    └────────┬────────┘
                             │
              ┌──────────────┴──────────────┐
              ▼                             ▼
       ┌─────────────┐              ┌──────────────┐
       │   Domain    │              │  Repository  │
       │   Objects   │              │              │
       └─────────────┘              └──────┬───────┘
                                           │
                                           ▼
                                    ┌──────────────┐
                                    │    SQLite    │
                                    └──────────────┘
```

Và domain relationship:

```text
Customer
   │
   │ 1..*
   ▼
 Event ────────────── 1 Hall
   │
   │ 1
   ▼
Booking
   │
   │ 1
   ▼
Quotation
   │
   │ 1
   ▼
Contract
   │
   │ 1..*
   ▼
Payment


Package 1..* ─── PackageServiceItem ─── *..1 ServiceItem


Event 1..* ─── StaffAssignment ─── *..1 Staff

Event 1..* ─── SpecialRequest
Event 1..* ─── Incident
```

---

# 2.10. SQLite Mapping

Một ưu điểm của thiết kế này là mapping sang SQLite khá thẳng.

|C++ Class|SQLite Table|
|---|---|
|User|`users`|
|Customer|`customers`|
|Center|`centers`|
|Hall|`halls`|
|Event|`events`|
|Booking|`bookings`|
|Package|`packages`|
|ServiceItem|`services`|
|PackageServiceItem|`package_services`|
|Quotation|`quotations`|
|QuotationItem|`quotation_items`|
|Contract|`contracts`|
|Payment|`payments`|
|Staff|`staff`|
|StaffAssignment|`staff_assignments`|
|SpecialRequest|`special_requests`|
|Incident|`incidents`|

Đây là một trong những lý do mình thay đổi thiết kế so với bản trước: **class model phải nghĩ đến khả năng mapping xuống relational database ngay từ đầu.**

---

# 2.11. Những gì mình sẽ KHÔNG làm

Với project này, mình khuyên tránh các thiết kế sau:

### ❌ Không tạo class cho từng Role

Không cần:

```text
SalesStaff
Accountant
Coordinator
OperationsStaff
Management
```

Nếu các Role chủ yếu khác nhau về permission thì:

```cpp
enum class UserRole
```

là đủ.

---

### ❌ Không tạo class cho từng Payment Method

Không cần ngay:

```text
CashPayment
CardPayment
BankTransferPayment
```

MVP chỉ cần:

```cpp
enum class PaymentMethod
```

---

### ❌ Không để Entity chạy SQL

Không:

```text
Booking → SQLite
Customer → SQLite
Payment → SQLite
```

Mà:

```text
Service → Repository → SQLite
```

---

### ❌ Không nhồi Business Logic vào Menu

Không:

```text
BookingMenu
    ├── SQL
    ├── conflict detection
    ├── payment calculation
    └── validation
```

Mà:

```text
BookingMenu
     ↓
BookingService
     ↓
BookingRepository
     ↓
SQLite
```

---

# 2.12. Bộ class cuối cùng mình đề xuất

Nếu phải **chốt ngay class list để bắt đầu coding**, mình sẽ dùng:

### Domain

```text
User
Customer
Center
Hall
Event
Booking
Package
ServiceItem
PackageServiceItem
Quotation
QuotationItem
Contract
Payment
Staff
StaffAssignment
SpecialRequest
Incident
```

### Service

```text
CustomerService
HallService
EventService
BookingService
PackageService
QuotationService
ContractService
PaymentService
OperationService
ReportService
```

### Repository

```text
Database
CustomerRepository
HallRepository
EventRepository
BookingRepository
PackageRepository
ServiceRepository
QuotationRepository
ContractRepository
PaymentRepository
StaffRepository
```

### Console

```text
MainMenu
CustomerMenu
HallMenu
EventMenu
BookingMenu
PackageMenu
QuotationMenu
ContractMenu
PaymentMenu
OperationMenu
ReportMenu
```

**Tổng cộng khoảng 45 class/module**, nhưng không có nghĩa là tất cả đều phức tạp. Domain chỉ khoảng **17 class**, còn Service/Repository/Menu chủ yếu là lớp điều phối.

Đây là mức mình thấy **cân bằng nhất giữa OOP, SQLite, Console và khả năng hoàn thành project**.

---

## Chốt kiến trúc

Nếu viết thành một câu trong tài liệu thiết kế, mình sẽ ghi:

> **The system adopts a layered object-oriented architecture consisting of a Console Presentation Layer, Service Layer, Domain Model Layer, Repository Layer, and SQLite Persistence Layer. Domain objects represent core business entities, Service classes implement business rules and workflows, Repository classes handle data access, and Console classes provide user interaction.**

Với thiết kế này, **phần tiếp theo nên là Class Diagram/UML**, và mình khuyên vẽ thành **2 diagram** thay vì nhét toàn bộ 45 class vào một hình: **(1) Domain Class Diagram** và **(2) Architecture/Class Dependency Diagram**. Cách này vừa dễ đọc vừa dễ bảo vệ.