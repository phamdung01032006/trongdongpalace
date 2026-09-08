# Member 1 — Event + OOP Core

### Nhiệm vụ chính

Xây dựng:

```
Event
├── WeddingEvent
├── CorporateEvent
└── BirthdayEvent
```

Các file:

```
Event.h / Event.cpp
WeddingEvent.h / WeddingEvent.cpp
CorporateEvent.h / CorporateEvent.cpp
BirthdayEvent.h / BirthdayEvent.cpp
```

### Phải thể hiện được

**Class & Object**

- private attributes
- public getter/setter
- constructor
- destructor
- member function
- `this`

Slide môn học nhấn mạnh class gồm data member và member function, đồng thời `private` phục vụ che giấu thông tin.

**Inheritance**

```
class WeddingEvent : public Event
```

```
class CorporateEvent : public Event
```

```
class BirthdayEvent : public Event
```

Phải giải thích được:

- base class
- derived class
- public inheritance
- protected/private
- constructor/destructor trong inheritance

**Polymorphism**

```
class Event {
public:
    virtual string getEventType() const = 0;
    virtual double calculateBasePrice() const = 0;
    virtual ~Event() = default;
};
```

Sau đó:

```
vector<shared_ptr<Event>> events;
```

và:

```
for (auto& event : events) {
    cout << event->getEventType();
    cout << event->calculateBasePrice();
}
```

Đây chính là phần quan trọng nhất của slide đa hình: hàm `virtual` cho phép gọi implementation phụ thuộc object thực tế thông qua base pointer/reference, tức dynamic binding.

**Exception**

Ví dụ:

```
if (startTime >= endTime)
    throw ValidationException("Invalid event time");
```

**I/O**

Event phải có:

```
toJson()
fromJson()
```

và được Repository lưu/đọc.

### Deliverable

```
Event hierarchy
+ Constructor/Destructor
+ Inheritance
+ Abstract class
+ Virtual function
+ Runtime polymorphism
+ Validation exception
+ JSON serialization
```

---

# 3. Member 2 — Customer + Center + Hall + Package

### Nhiệm vụ chính

```
Customer
Center
Hall
Package
Service
```

Các CRUD:

```
Customer CRUD
Center CRUD
Hall CRUD
Package CRUD
Service CRUD
```

### Phải tập trung học

#### Class & Object

Member 2 phải chịu trách nhiệm làm class model tương đối đầy đủ:

```
class Customer {
private:
    string id;
    string name;
    string phone;
    string email;

public:
    Customer();
    Customer(...);

    string getId() const;
    void setName(...);

    ...
};
```

Qua đó thực hành:

- encapsulation
- constructor
- destructor
- `this`
- object
- object collection
- static nếu phù hợp

---

### Inheritance

Member 2 **không được chỉ làm CRUD**.

Phải tự implement một bài nhỏ về inheritance trong module của mình.

Ví dụ:

```
Service
├── DecorationService
├── PhotographyService
└── MusicService
```

Hoặc nếu muốn hệ thống đơn giản hơn:

```
Package
├── WeddingPackage
└── CorporatePackage
```

Không nhất thiết phải giữ hierarchy này trong architecture cuối cùng nếu nó làm hệ thống phức tạp, nhưng người này phải **thực hành inheritance**.

---

### Polymorphism

Cho một collection:

```
vector<shared_ptr<Service>>
```

và gọi:

```
service->calculatePrice();
```

Mục tiêu là Member 2 cũng hiểu được:

```
Base pointer
      ↓
Derived object
      ↓
virtual function
      ↓
dynamic binding
```

---

### I/O Stream

Member 2 phụ trách **Repository nền tảng**.

Ví dụ:

```
JsonRepository
CustomerRepository
CenterRepository
HallRepository
```

Phải hiểu:

```
ifstream
ofstream
fstream
open()
close()
read
write
```

Slide I/O quy định `ofstream` dùng để ghi, `ifstream` dùng để đọc và `fstream` hỗ trợ cả hai.

Trong project thực tế, vì dùng `nlohmann/json`, người này phải hiểu:

```
data.json
    ↓
ifstream
    ↓
json
    ↓
Object
```

và:

```
Object
    ↓
json
    ↓
ofstream
    ↓
data.json
```

---

### Exception

Phải xử lý:

```
File không tồn tại
JSON lỗi
ID trùng
ID không tồn tại
```

---

# 4. Member 3 — Booking + Quotation + Contract + Payment

Đây là người phụ trách **business logic khó nhất**.

### Module chính

```
Booking
Quotation
Contract
Payment
```

và đặc biệt:

```
Hall Availability
Booking Conflict
Payment Status
```

---

## Booking

Phải implement:

```
newStart < oldEnd &&
newEnd > oldStart
```

Nếu overlap:

```
throw BookingConflictException(...);
```

Nếu:

```
startTime >= endTime
```

thì:

```
throw ValidationException(...);
```

---

## Payment

Implement:

```
Outstanding = Total - Paid
```

và:

```
Paid = 0
        ↓
Unpaid

Paid < Total
        ↓
Partially Paid

Paid >= Total
        ↓
Paid
```

Không cho:

```
amount < 0
```

→ `ValidationException`.