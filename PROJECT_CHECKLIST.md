# PROJECT CHECKLIST — Trống Đồng Palace Event Management System

> Nguồn: `Functional Requirements _ Trong Dong Palace.md`, `Kế thừa, đa hình và xử lý ngoại lệ.md`,
> `Thiết kế các lớp với SQLite.md`, Instruction cho AI Code.
> Application type: **C++ Console Application** (không GUI, không web/mobile/cloud).

---

## 0. Checklist tổng quát

- [ ] Authentication
- [ ] Customer
- [ ] Hall
- [ ] Event
- [ ] Booking
- [ ] Availability
- [ ] Package
- [ ] Service
- [ ] Quotation
- [ ] Contract
- [ ] Payment
- [ ] Operations
- [ ] Event Completion
- [ ] Reports
- [ ] SQLite
- [ ] Exception Handling
- [ ] Authorization

---

## 1. Flow chính

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

Đây là flow ưu tiên hoàn thiện end-to-end trước cho Integration Test / Final Demo.

---

## 2. Checklist chi tiết theo từng mục (bám FR)

### 2.1. Authentication
- [ ] FR-AUTH-001 — Login (username/password → xác định Role → hiện Main Menu)

### 2.2. Authorization
- [ ] FR-AUTH-002 — Role Authorization (theo bảng phân quyền CRUD/R theo Role: Management, Sales, Accountant, Coordinator, Operations, Admin)
- [ ] `hasPermission(role, permission)` hoặc cơ chế tương đương đơn giản

### 2.3. Customer
- [ ] FR-CUS-001 — Create Customer (validate, check trùng ID)
- [ ] FR-CUS-002 — View Customer
- [ ] FR-CUS-003 — Search Customer (theo ID/Name/Phone)
- [ ] FR-CUS-004 — Update Customer
- [ ] FR-CUS-005 — Delete Customer (từ chối nếu đã gắn Event/Booking)

### 2.4. Hall
- [ ] FR-HALL-001 — Create Hall
- [ ] FR-HALL-002 — View Hall
- [ ] FR-HALL-003 — Update Hall
- [ ] FR-HALL-004 — Hall Status Active/Inactive — **BR-HALL-001**

### 2.5. Event
- [ ] FR-EVENT-001 — Create Event
- [ ] FR-EVENT-002 — Validate Event Time (Start < End) — **BR-EVENT-001**
- [ ] FR-EVENT-003 — View Event (list + detail)
- [ ] FR-EVENT-004 — Update Event (trước khi Completed)
- [ ] FR-EVENT-005 — Event Status: Planned / Preparing / In Progress / Completed / Cancelled

### 2.6. Booking
- [ ] FR-BOOK-001 — Create Booking (Validate Event → Validate Hall → Check Status → Check Availability → Check Conflict)
- [ ] FR-BOOK-002 — Check Booking Conflict (overlap) — **BR-BOOK-001**
- [ ] FR-BOOK-003 — View Booking
- [ ] FR-BOOK-004 — Search Booking (ID/Customer/Event/Hall/Date/Status)
- [ ] FR-BOOK-005 — Cancel Booking (Cancelled không tính là đang chiếm Hall)

### 2.7. Availability
- [ ] FR-HALL-005 — Check Hall Availability (theo khoảng thời gian, dùng chung logic với FR-BOOK-002)

### 2.8. Package
- [ ] FR-PKG-001 — Create Package
- [ ] FR-PKG-002 — View Package
- [ ] FR-PKG-003 — Add Service to Package (many-to-many)

### 2.9. Service
- [ ] FR-SVC-001 — Create Service
- [ ] FR-SVC-002 — View Service
- [ ] FR-SVC-003 — Add Service to Booking (Amount = Quantity × Unit Price)

### 2.10. Quotation
- [ ] FR-QUO-001 — Create Quotation (từ Booking)
- [ ] FR-QUO-002 — Calculate Quotation (Subtotal = Package + Service; Total = Subtotal - Discount + Tax; tự tính, không cho nhập tay Total)
- [ ] FR-QUO-003 — Update Quotation (tự tính lại Total khi thay đổi Service/Package/Discount)
- [ ] FR-QUO-004 — View Quotation (chi tiết)

### 2.11. Contract
- [ ] FR-CON-001 — Create Contract (từ Quotation)
- [ ] FR-CON-002 — View Contract
- [ ] FR-CON-003 — Contract Status: Draft / Active / Completed / Cancelled

### 2.12. Payment
- [ ] FR-PAY-001 — Record Payment
- [ ] FR-PAY-002 — Validate Payment (Total Paid ≤ Contract Total) — **BR-PAY-001**
- [ ] FR-PAY-003 — Validate Deposit (Deposit ≤ Contract Total) — **BR-PAY-002**
- [ ] FR-PAY-004 — Calculate Outstanding (Contract Total − Total Paid)
- [ ] FR-PAY-005 — View Payment History
- [ ] Payment Method Polymorphism: `PaymentMethod` (abstract) → `CashPayment`, `BankTransferPayment`, `CardPayment`

### 2.13. Operations
- [ ] FR-OPS-001 — Assign Staff (Event ↔ Staff ↔ Role, qua `StaffAssignment`)
- [ ] FR-OPS-002 — Manage Special Request
- [ ] FR-OPS-003 — Manage Incident
- [ ] FR-OPS-004 — Update Operation Status (Planned/Preparing/In Progress/Completed)

### 2.14. Event Completion
- [ ] FR-EOC-001 — Complete Event (kiểm tra trạng thái hợp lệ trước khi Complete)
- [ ] FR-EOC-002 — Record Completion Information (Actual Guest Count, Note, Incident, Special Request Result)

### 2.15. Reports
- [ ] FR-REP-001 — Booking Report
- [ ] FR-REP-002 — Event Report (Total / by Type / by Hall / by Status)
- [ ] FR-REP-003 — Payment Report (Contract Total / Total Paid / Outstanding)
- [ ] FR-REP-004 — Revenue Report (có thể lọc theo thời gian)

### 2.16. SQLite
- [x] `DatabaseManager` — open() / close() / execute() (+ PRAGMA foreign_keys = ON)
- [x] `DatabaseInitializer` — CREATE TABLE IF NOT EXISTS cho 17 bảng, đã build/test
- [ ] FR-DATA-001 — Save Data cho toàn bộ entity chính (chờ Repository)
- [ ] FR-DATA-002 — Load Data khi khởi động app (chờ Repository)
- [ ] FR-DATA-003 — Repository layer (Service không viết SQL trực tiếp)

### 2.17. Exception Handling
- [ ] `AppException` (base, kế thừa `std::exception`)
- [ ] `ValidationException`
- [ ] `EntityNotFoundException`
- [ ] `DatabaseException`
- [ ] `BusinessException`
  - [ ] `BookingConflictException`
  - [ ] `PaymentException`
- [ ] Console catch đúng thứ tự (specific → general), không catch `...` quá sớm

---

## 3. Cross-cutting (áp dụng xuyên suốt mọi module, không tính là module riêng)

- [ ] Console UI: Main Menu + Module Menu cho từng mục ở mục 0
- [ ] Input Validation: không crash vì input sai (số âm, chuỗi rỗng, sai định dạng ngày/giờ, sai lựa chọn menu)
- [ ] Operation Confirmation cho Delete/Cancel
- [ ] Audit Log mức đơn giản (nếu còn thời gian, không bắt buộc phức tạp)

---

## 4. Domain Relationship (để dựng Class Diagram / ERD)

```text
Customer 1 ──── * Event
Event    1 ──── 1 Booking ──── 1 Hall
Booking  1 ──── 1 Quotation
Quotation 1 ──── 1 Contract
Contract 1 ──── * Payment
Event    1 ──── * StaffAssignment ──── * Staff
Event    1 ──── * SpecialRequest
Event    1 ──── * Incident

Package 1..* ── PackageServiceItem ── *..1 ServiceItem
Quotation 1..* ── QuotationItem ── *..1 ServiceItem
Center   1 ──── * Hall
```

---

## 5. OOP Requirements Checklist

- [ ] **Encapsulation**: toàn bộ domain class có private attribute + public getter/setter cần thiết
- [ ] **Abstraction**: `PaymentMethod` là abstract class (pure virtual `process()`, `getName()`)
- [ ] **Inheritance**:
  - [ ] `PaymentMethod` → `CashPayment`, `BankTransferPayment`, `CardPayment`
  - [ ] `AppException` → `ValidationException`, `EntityNotFoundException`, `DatabaseException`, `BusinessException` → `BookingConflictException`, `PaymentException`
- [ ] **Polymorphism**: gọi `method->process(amount)` qua `unique_ptr<PaymentMethod>`, Service không hard-code `if (method == CASH)`
- [ ] **Exception Handling**: throw ở Service, catch ở Console; Domain không biết Console/SQLite

---

## 6. Core Business Rules Checklist

- [ ] **BR-HALL-001** — Hall phải tồn tại và Active mới được dùng để Booking
- [ ] **BR-EVENT-001** — Event Start Time < End Time
- [ ] **BR-BOOK-001** — Không được Booking overlap trên cùng Hall
- [ ] **BR-PAY-001** — Tổng Payment không được vượt Contract Total
- [ ] **BR-PAY-002** — Deposit không được vượt Contract Total

> Toàn bộ rule trên phải được enforce ở **Service Layer**, không chỉ validate ở Console.

---

## 7. Development Roadmap

- [x] Day 1 — Phân tích Requirements → `PROJECT_CHECKLIST.md`
- [x] Day 2 — Project Setup + SQLite (`DatabaseManager`, kết nối `trong_dong_palace.db`)
- [x] Day 3 — Database Schema (`DatabaseInitializer`, 17 bảng + FK chính)
- [ ] Day 4+ — Exceptions
- [ ] Repositories (interface + SQLite implementation)
- [ ] Customer
- [ ] Hall
- [ ] Event
- [ ] Booking + Availability
- [ ] Package + Service
- [ ] Quotation
- [ ] Contract
- [ ] Payment + Polymorphism
- [ ] Operations
- [ ] Event Completion
- [ ] Reports
- [ ] Authentication + Authorization
- [ ] Full Persistence (mọi entity qua Repository)
- [ ] Testing
- [ ] Refactoring
- [ ] Final integration & Demo

---

## 8. Definition of Done (áp dụng cho mỗi feature)

- [ ] Code compile được
- [ ] Requirement được implement đúng
- [ ] Input hợp lệ chạy đúng
- [ ] Input không hợp lệ được xử lý (không crash)
- [ ] Business rule liên quan được enforce
- [ ] Exception được throw/catch đúng chỗ
- [ ] Dữ liệu được lưu (nếu có persistence)
- [ ] Dữ liệu load lại đúng sau khi restart
- [ ] Không có duplicate code rõ ràng
- [ ] Kiến trúc vẫn đơn giản (không over-engineering)
