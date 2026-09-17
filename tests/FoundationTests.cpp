#include "BirthdayEvent.h"
#include "AuthService.h"
#include "Center.h"
#include "CenterRepository.h"
#include "CenterService.h"
#include "CorporateEvent.h"
#include "Customer.h"
#include "CustomerRepository.h"
#include "CustomerService.h"
#include "BookingRepository.h"
#include "ContractRepository.h"
#include "EventRepository.h"
#include "Exceptions.h"
#include "Hall.h"
#include "HallRepository.h"
#include "HallService.h"
#include "InputHelper.h"
#include "JsonStorage.h"
#include "Package.h"
#include "PackageRepository.h"
#include "PaymentRepository.h"
#include "QuotationRepository.h"
#include "Service.h"
#include "ServiceRepository.h"
#include "User.h"
#include "UserRepository.h"
#include "WeddingEvent.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;

namespace {
void expect(bool condition, const string& message) {
    if (!condition) throw runtime_error(message);
}

template <typename ExceptionType>
void expectException(const function<void()>& action, const string& expectedMessage) {
    try {
        action();
    } catch (const ExceptionType& e) {
        expect(e.what() == expectedMessage, "Unexpected exception message: " + string(e.what()));
        return;
    }
    throw runtime_error("Expected exception was not thrown.");
}

void testExceptions() {
    expectException<ValidationException>([] { throw ValidationException("Invalid customer name"); },
                                         "Invalid customer name");
    expectException<NotFoundException>([] { throw NotFoundException("Not found"); }, "Not found");
    expectException<DuplicateException>([] { throw DuplicateException("Duplicate"); }, "Duplicate");
    expectException<BookingConflictException>([] { throw BookingConflictException("Conflict"); }, "Conflict");
    expectException<AuthenticationException>([] { throw AuthenticationException("Invalid login"); }, "Invalid login");
    expectException<FileException>([] { throw FileException("File error"); }, "File error");
}

void testJsonStorage() {
    const filesystem::path testDirectory = filesystem::temp_directory_path() / "trong_dong_foundation_tests";
    const filesystem::path testFile = testDirectory / "data.json";
    error_code error;
    filesystem::remove_all(testDirectory, error);

    JsonStorage storage(testFile);
    const json created = storage.load();
    expect(filesystem::exists(testFile), "JsonStorage did not create a missing data file.");
    expect(created.at("customers").is_array(), "Customers schema is missing.");

    json saved = created;
    saved["customers"].push_back({{"id", "C001"}, {"name", "Nguyen Van A"}});
    storage.save(saved);
    expect(storage.load().at("customers").at(0).at("id") == "C001", "JSON persistence failed.");

    ofstream badJson(testFile);
    badJson << "{ invalid json";
    badJson.close();
    expectException<FileException>([&] { storage.load(); }, "Failed to parse data.json.");
    filesystem::remove_all(testDirectory, error);
}

void testBaseModels() {
    User user("U001", "admin", "password", "Administrator", "ADMIN");
    expect(user.getUsername() == "admin", "User getter failed.");

    Customer customer("C001", "Nguyen Van A", "0901234567");
    expect(customer.getName() == "Nguyen Van A", "Customer getter failed.");
    customer.setName("Tran Thi B");
    expect(customer.getName() == "Tran Thi B", "Customer setter failed.");
    expectException<ValidationException>([&] { customer.setPhone("123"); },
                                         "Phone number must contain 9 to 15 digits.");

    Center center("CT001", "Trong Dong Palace", "Ha Noi");
    Hall hall("H001", center.getId(), "Palace 1", 500);
    expect(hall.isActive(), "Hall should be active by default.");
    expectException<ValidationException>([&] { hall.setCapacity(0); },
                                         "Hall capacity must be greater than 0.");

    Package package("P001", "Wedding Premium", "Dinner package", 8000000, 100);
    Service service("S001", "Decoration", "package", 5000000);
    expect(package.getPrice() == 8000000, "Package getter failed.");
    expect(service.getUnitPrice() == 5000000, "Service getter failed.");
    expectException<ValidationException>([&] { service.setUnitPrice(-1); },
                                         "Service unit price cannot be negative.");
}

void testInputHelpers() {
    istringstream input("text\n-2\n12\n\nNguyen Van A\nmaybe\nY\ninvalid\n-1.5\n2.5\n");
    ostringstream output;
    streambuf* oldInput = cin.rdbuf(input.rdbuf());
    streambuf* oldOutput = cout.rdbuf(output.rdbuf());

    const int number = InputHelper::readInt("Number: ", 0);
    const string name = InputHelper::readString("Name: ");
    const bool answer = InputHelper::readYesNo("Confirm");
    const double price = InputHelper::readDouble("Price: ", 0.0);

    cin.rdbuf(oldInput);
    cout.rdbuf(oldOutput);

    expect(number == 12, "InputHelper did not retry an invalid integer.");
    expect(name == "Nguyen Van A", "InputHelper did not retry an empty string.");
    expect(answer, "InputHelper did not retry an invalid Y/N answer.");
    expect(fabs(price - 2.5) < 0.001, "InputHelper did not retry an invalid double.");
}

void testPolymorphismAndEventValidation() {
    vector<shared_ptr<Event>> events;
    events.push_back(make_shared<WeddingEvent>("E001", "C001", "H001", "2026-10-01", "18:00", "22:00", 100));
    events.push_back(make_shared<CorporateEvent>("E002", "C001", "H001", "2026-10-02", "08:00", "12:00", 100, true));
    events.push_back(make_shared<BirthdayEvent>("E003", "C001", "H001", "2026-10-03", "18:00", "21:00", 100, 25));

    expect(events[0]->getEventType() == "WEDDING", "Wedding runtime dispatch failed.");
    expect(events[1]->getEventType() == "CORPORATE", "Corporate runtime dispatch failed.");
    expect(events[2]->getEventType() == "BIRTHDAY", "Birthday runtime dispatch failed.");
    expect(fabs(events[0]->calculateBasePrice() - 40000000.0) < 0.001, "Wedding price is incorrect.");
    expect(fabs(events[1]->calculateBasePrice() - 29500000.0) < 0.001, "Corporate price is incorrect.");
    expect(fabs(events[2]->calculateBasePrice() - 22000000.0) < 0.001, "Birthday price is incorrect.");

    expectException<ValidationException>([] {
        WeddingEvent invalid("E004", "C001", "H001", "2026-10-04", "22:00", "18:00", 100);
    }, "Start time must be before end time.");
}

filesystem::path createTestDirectory(const string& name) {
    const filesystem::path directory = filesystem::temp_directory_path() / name;
    error_code error;
    filesystem::remove_all(directory, error);
    return directory;
}

void testCustomerRepositoryAndService() {
    const filesystem::path directory = createTestDirectory("trong_dong_customer_repository_tests");
    error_code error;
    JsonStorage storage(directory / "data.json");
    CustomerRepository repository(storage);
    CustomerService service(repository);

    // Day 6 persistence contract: add(C001) -> save() -> load() -> findById(C001).
    repository.add(Customer("C001", "Nguyen Van A", "0901234567"));
    repository.save();
    repository.load();
    expect(repository.findById("C001").getName() == "Nguyen Van A",
           "Customer add/save/load/findById contract failed.");

    service.update(Customer("C001", "Nguyen Van A", "0901234567", "a@example.com", "Ha Noi"));
    expect(service.findById("C001").getName() == "Nguyen Van A", "Customer create/find failed.");
    expectException<DuplicateException>([&] {
        service.create(Customer("C001", "Duplicate", "0901234568"));
    }, "Customer ID already exists: C001");

    service.update(Customer("C001", "Nguyen Van B", "0987654321", "b@example.com", "Da Nang"));
    expect(service.findById("C001").getName() == "Nguyen Van B", "Customer update failed.");
    CustomerRepository reloadedRepository(storage);
    expect(reloadedRepository.findById("C001").getPhone() == "0987654321", "Customer persistence failed.");

    service.remove("C001");
    expectException<NotFoundException>([&] { service.findById("C001"); }, "Customer not found: C001");
    filesystem::remove_all(directory, error);
}

void testDocumentRepositories() {
    const filesystem::path directory = createTestDirectory("trong_dong_document_repository_tests");
    error_code error;
    JsonStorage storage(directory / "data.json");
    EventRepository eventRepository(storage);
    BookingRepository bookingRepository(storage);
    PackageRepository packageRepository(storage);
    ServiceRepository serviceRepository(storage);
    QuotationRepository quotationRepository(storage);
    ContractRepository contractRepository(storage);
    PaymentRepository paymentRepository(storage);
    eventRepository.add({{"id", "E001"}, {"type", "WEDDING"}});
    eventRepository.save(); eventRepository.load();
    expect(eventRepository.findById("E001").at("type") == "WEDDING", "Event repository persistence failed.");

    bookingRepository.add(Booking("B001", "E001", "P001"));
    packageRepository.add(Package("P001", "Premium", "Dinner", 1000000, 100));
    serviceRepository.add(Service("S001", "Decoration", "package", 500000));
    quotationRepository.add(Quotation("Q001", "E001", 1500000));
    contractRepository.add(Contract("CT001", "Q001", "2026-09-17"));
    paymentRepository.add(Payment("PM001", "CT001", 500000, "2026-09-17"));
    bookingRepository.save(); packageRepository.save(); serviceRepository.save();
    quotationRepository.save(); contractRepository.save(); paymentRepository.save();
    bookingRepository.load(); packageRepository.load(); serviceRepository.load();
    quotationRepository.load(); contractRepository.load(); paymentRepository.load();
    const Booking storedBooking = bookingRepository.findById("B001");
    expect(storedBooking.getEventId() == "E001", "Booking repository persistence failed.");
    expect(storedBooking.getStatus() == "PENDING", "Booking repository default status was not persisted.");
    expect(packageRepository.findById("P001").getName() == "Premium", "Package repository persistence failed.");
    expect(serviceRepository.findById("S001").getName() == "Decoration", "Service repository persistence failed.");
    expect(quotationRepository.findById("Q001").getTotal() == 1500000, "Quotation repository persistence failed.");
    expect(contractRepository.findById("CT001").getQuotationId() == "Q001", "Contract repository persistence failed.");
    expect(paymentRepository.findById("PM001").getAmount() == 500000, "Payment repository persistence failed.");
    filesystem::remove_all(directory, error);
}

void testCenterHallServices() {
    const filesystem::path directory = createTestDirectory("trong_dong_center_hall_tests");
    error_code error;
    JsonStorage storage(directory / "data.json");
    CenterRepository centerRepository(storage);
    HallRepository hallRepository(storage);
    CenterService centerService(centerRepository);
    HallService hallService(hallRepository, centerRepository);

    centerService.create(Center("CT001", "Trong Dong Palace", "Ha Noi"));
    hallService.create(Hall("H001", "CT001", "Palace 1", 500));
    expect(hallService.findById("H001").isActive(), "Hall create/find failed.");
    expectException<NotFoundException>([&] {
        hallService.create(Hall("H002", "UNKNOWN", "Palace 2", 300));
    }, "Center not found: UNKNOWN");

    hallService.update(Hall("H001", "CT001", "Palace 1 Updated", 600, "INACTIVE"));
    expect(!hallService.findById("H001").isActive(), "Hall update failed.");
    HallRepository reloadedRepository(storage);
    expect(reloadedRepository.findById("H001").getCapacity() == 600, "Hall persistence failed.");

    hallService.remove("H001");
    expectException<NotFoundException>([&] { hallService.findById("H001"); }, "Hall not found: H001");
    filesystem::remove_all(directory, error);
}

void testAuthenticationAndAuthorization() {
    const filesystem::path directory = createTestDirectory("trong_dong_auth_tests");
    error_code error;
    JsonStorage storage(directory / "data.json");
    UserRepository userRepository(storage);
    AuthService authService(userRepository);
    authService.seedDefaultUsers();

    const User admin = authService.login("admin", "admin123");
    const User sales = authService.login("sales", "sales123");
    expect(admin.getRole() == "ADMIN", "Admin login failed.");
    expect(authService.hasPermission(sales, "CUSTOMER", "CREATE"), "Sales should manage customers.");
    expect(!authService.hasPermission(sales, "PAYMENT", "CREATE"), "Sales must not manage payments.");
    expectException<AuthenticationException>([&] { authService.login("sales", "wrong"); },
                                             "Invalid username or password.");
    expectException<AuthenticationException>([&] { authService.login("missing", "password"); },
                                             "Invalid username or password.");
    expectException<AuthenticationException>([&] { authService.requirePermission(sales, "PAYMENT", "CREATE"); },
                                             "Access denied.");

    UserRepository reloadedRepository(storage);
    expect(reloadedRepository.findByUsername("admin").getRole() == "ADMIN", "Seeded users were not persisted.");
    filesystem::remove_all(directory, error);
}
}

int main(int argc, char* argv[]) {
    try {
        const string selectedTest = argc > 1 ? argv[1] : "all";
        if (selectedTest == "all" || selectedTest == "exceptions") testExceptions();
        if (selectedTest == "all" || selectedTest == "json") testJsonStorage();
        if (selectedTest == "all" || selectedTest == "models") {
            testBaseModels();
            testInputHelpers();
        }
        if (selectedTest == "all" || selectedTest == "events") testPolymorphismAndEventValidation();
        if (selectedTest == "all" || selectedTest == "customer") testCustomerRepositoryAndService();
        if (selectedTest == "all" || selectedTest == "repository") testDocumentRepositories();
        if (selectedTest == "all" || selectedTest == "centerhall") testCenterHallServices();
        if (selectedTest == "all" || selectedTest == "auth") testAuthenticationAndAuthorization();
        cout << "FoundationTests: PASS\n";
        return 0;
    } catch (const exception& e) {
        cerr << "FoundationTests: FAIL - " << e.what() << '\n';
        return 1;
    }
}
