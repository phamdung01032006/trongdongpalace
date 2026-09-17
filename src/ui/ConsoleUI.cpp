#include "ConsoleUI.h"
#include "DisplayHelper.h"
#include "Exceptions.h"
#include "InputHelper.h"

#include <iostream>

using namespace std;

ConsoleUI::ConsoleUI(AuthService& authService, CustomerService& customerService,
                     CenterService& centerService, HallService& hallService)
    : authService(authService), customerService(customerService),
      centerService(centerService), hallService(hallService) {}

void ConsoleUI::showStartMenu() const {
    DisplayHelper::showHeader("TRONG DONG PALACE");
    cout << "1. Login\n0. Exit\n";
}

void ConsoleUI::showMainMenu() const {
    DisplayHelper::showHeader("TRONG DONG PALACE MANAGEMENT");
    cout << "Logged in as: " << currentUser->getFullName() << "\n";
    cout << "Role        : " << currentUser->getRole() << "\n--------------------------------\n";
    cout << "1. Customer Management\n2. Center / Hall Management\n3. Event Management\n"
         << "4. Booking Management\n5. Package Management\n6. Service Management\n"
         << "7. Quotation Management\n8. Contract Management\n9. Payment Management\n"
         << "10. Event Operations\n11. Reports\n12. Logout\n";
}

int ConsoleUI::readMenuChoice(int minimum, int maximum) const {
    return InputHelper::readInt("Enter your choice: ", minimum, maximum);
}

void ConsoleUI::requirePermission(const string& module, const string& action) const {
    if (!currentUser) throw AuthenticationException("Please login first.");
    authService.requirePermission(*currentUser, module, action);
}

void ConsoleUI::login() {
    DisplayHelper::showHeader("LOGIN");
    const string username = InputHelper::readString("Username: ");
    const string password = InputHelper::readString("Password: ");
    currentUser = authService.login(username, password);
    DisplayHelper::showSuccess("Login successful. Welcome, " + currentUser->getFullName() + ".");
}

void ConsoleUI::handleUnavailableModule(const string& module, const string& action) const {
    requirePermission(module, action);
    DisplayHelper::showError(module + " module will be implemented in a later development day.");
}

void ConsoleUI::showCustomerMenu() {
    bool inMenu = true;
    while (inMenu) {
        DisplayHelper::showHeader("CUSTOMER MANAGEMENT");
        cout << "1. Create Customer\n2. View All Customers\n3. Find Customer\n"
             << "4. Update Customer\n5. Delete Customer\n0. Back\n";
        try {
            switch (readMenuChoice(0, 5)) {
                case 0: inMenu = false; break;
                case 1: {
                    requirePermission("CUSTOMER", "CREATE");
                    Customer customer(InputHelper::readString("Customer ID: "), InputHelper::readString("Name: "),
                                      InputHelper::readString("Phone: "), InputHelper::readString("Email (optional): ", true),
                                      InputHelper::readString("Address (optional): ", true));
                    customerService.create(customer);
                    DisplayHelper::showSuccess("Customer created successfully.");
                    break;
                }
                case 2: {
                    requirePermission("CUSTOMER", "READ");
                    cout << "ID\tName\tPhone\tEmail\n";
                    for (const Customer& customer : customerService.getAll()) {
                        cout << customer.getId() << '\t' << customer.getName() << '\t' << customer.getPhone()
                             << '\t' << customer.getEmail() << '\n';
                    }
                    break;
                }
                case 3: {
                    requirePermission("CUSTOMER", "READ");
                    const Customer customer = customerService.findById(InputHelper::readString("Customer ID: "));
                    cout << customer.getId() << " - " << customer.getName() << " | " << customer.getPhone()
                         << " | " << customer.getEmail() << " | " << customer.getAddress() << '\n';
                    break;
                }
                case 4: {
                    requirePermission("CUSTOMER", "UPDATE");
                    const string id = InputHelper::readString("Customer ID: ");
                    customerService.findById(id);
                    customerService.update(Customer(id, InputHelper::readString("New name: "),
                                                    InputHelper::readString("New phone: "),
                                                    InputHelper::readString("New email (optional): ", true),
                                                    InputHelper::readString("New address (optional): ", true)));
                    DisplayHelper::showSuccess("Customer updated successfully.");
                    break;
                }
                case 5: {
                    requirePermission("CUSTOMER", "DELETE");
                    const string id = InputHelper::readString("Customer ID: ");
                    if (InputHelper::readYesNo("Delete customer " + id + "?")) {
                        customerService.remove(id);
                        DisplayHelper::showSuccess("Customer deleted successfully.");
                    }
                    break;
                }
            }
        } catch (const AppException& e) { DisplayHelper::showError(e.what()); }
    }
}

void ConsoleUI::showCenterMenu() {
    bool inMenu = true;
    while (inMenu) {
        DisplayHelper::showHeader("CENTER MANAGEMENT");
        cout << "1. Create Center\n2. View All Centers\n3. Find Center\n"
             << "4. Update Center\n5. Delete Center\n0. Back\n";
        try {
            switch (readMenuChoice(0, 5)) {
                case 0: inMenu = false; break;
                case 1:
                    requirePermission("CENTER", "CREATE");
                    centerService.create(Center(InputHelper::readString("Center ID: "), InputHelper::readString("Name: "),
                                                InputHelper::readString("Address (optional): ", true)));
                    DisplayHelper::showSuccess("Center created successfully.");
                    break;
                case 2:
                    requirePermission("CENTER", "READ");
                    for (const Center& center : centerService.getAll())
                        cout << center.getId() << " - " << center.getName() << " | " << center.getAddress() << '\n';
                    break;
                case 3: {
                    requirePermission("CENTER", "READ");
                    const Center center = centerService.findById(InputHelper::readString("Center ID: "));
                    cout << center.getId() << " - " << center.getName() << " | " << center.getAddress() << '\n';
                    break;
                }
                case 4: {
                    requirePermission("CENTER", "UPDATE");
                    const string id = InputHelper::readString("Center ID: ");
                    centerService.findById(id);
                    centerService.update(Center(id, InputHelper::readString("New name: "),
                                                InputHelper::readString("New address (optional): ", true)));
                    DisplayHelper::showSuccess("Center updated successfully.");
                    break;
                }
                case 5: {
                    requirePermission("CENTER", "DELETE");
                    const string id = InputHelper::readString("Center ID: ");
                    if (InputHelper::readYesNo("Delete center " + id + "?")) {
                        centerService.remove(id);
                        DisplayHelper::showSuccess("Center deleted successfully.");
                    }
                    break;
                }
            }
        } catch (const AppException& e) { DisplayHelper::showError(e.what()); }
    }
}

void ConsoleUI::showHallMenu() {
    bool inMenu = true;
    while (inMenu) {
        DisplayHelper::showHeader("HALL MANAGEMENT");
        cout << "1. Create Hall\n2. View All Halls\n3. Find Hall\n"
             << "4. Update Hall\n5. Delete Hall\n0. Back\n";
        try {
            switch (readMenuChoice(0, 5)) {
                case 0: inMenu = false; break;
                case 1: {
                    requirePermission("HALL", "CREATE");
                    hallService.create(Hall(InputHelper::readString("Hall ID: "), InputHelper::readString("Center ID: "),
                                            InputHelper::readString("Name: "), InputHelper::readInt("Capacity: ", 1),
                                            InputHelper::readString("Status (ACTIVE/INACTIVE): ")));
                    DisplayHelper::showSuccess("Hall created successfully.");
                    break;
                }
                case 2:
                    requirePermission("HALL", "READ");
                    cout << "ID\tCenter\tName\tCapacity\tStatus\n";
                    for (const Hall& hall : hallService.getAll())
                        cout << hall.getId() << '\t' << hall.getCenterId() << '\t' << hall.getName() << '\t'
                             << hall.getCapacity() << '\t' << hall.getStatus() << '\n';
                    break;
                case 3: {
                    requirePermission("HALL", "READ");
                    const Hall hall = hallService.findById(InputHelper::readString("Hall ID: "));
                    cout << hall.getId() << " - " << hall.getName() << " | center=" << hall.getCenterId()
                         << " | capacity=" << hall.getCapacity() << " | " << hall.getStatus() << '\n';
                    break;
                }
                case 4: {
                    requirePermission("HALL", "UPDATE");
                    const string id = InputHelper::readString("Hall ID: ");
                    const Hall oldHall = hallService.findById(id);
                    hallService.update(Hall(id, oldHall.getCenterId(), InputHelper::readString("New name: "),
                                            InputHelper::readInt("New capacity: ", 1),
                                            InputHelper::readString("New status (ACTIVE/INACTIVE): ")));
                    DisplayHelper::showSuccess("Hall updated successfully.");
                    break;
                }
                case 5: {
                    requirePermission("HALL", "DELETE");
                    const string id = InputHelper::readString("Hall ID: ");
                    if (InputHelper::readYesNo("Delete hall " + id + "?")) {
                        hallService.remove(id);
                        DisplayHelper::showSuccess("Hall deleted successfully.");
                    }
                    break;
                }
            }
        } catch (const AppException& e) { DisplayHelper::showError(e.what()); }
    }
}

void ConsoleUI::showCenterHallMenu() {
    bool inMenu = true;
    while (inMenu) {
        DisplayHelper::showHeader("CENTER / HALL MANAGEMENT");
        cout << "1. Center Management\n2. Hall Management\n0. Back\n";
        switch (readMenuChoice(0, 2)) {
            case 0: inMenu = false; break;
            case 1: showCenterMenu(); break;
            case 2: showHallMenu(); break;
        }
    }
}

void ConsoleUI::run() {
    bool running = true;
    while (running) {
        try {
            if (!currentUser) {
                showStartMenu();
                if (readMenuChoice(0, 1) == 0) { cout << "Goodbye.\n"; running = false; }
                else login();
                continue;
            }
            showMainMenu();
            switch (readMenuChoice(1, 12)) {
                case 1: showCustomerMenu(); break;
                case 2: showCenterHallMenu(); break;
                case 3: handleUnavailableModule("EVENT"); break;
                case 4: handleUnavailableModule("BOOKING"); break;
                case 5: handleUnavailableModule("PACKAGE"); break;
                case 6: handleUnavailableModule("SERVICE"); break;
                case 7: handleUnavailableModule("QUOTATION"); break;
                case 8: handleUnavailableModule("CONTRACT"); break;
                case 9: handleUnavailableModule("PAYMENT", "CREATE"); break;
                case 10: handleUnavailableModule("OPERATIONS"); break;
                case 11: handleUnavailableModule("REPORT"); break;
                case 12: currentUser.reset(); DisplayHelper::showSuccess("Logged out."); break;
            }
        } catch (const AppException& e) { DisplayHelper::showError(e.what()); }
    }
}
