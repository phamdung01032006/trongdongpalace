#include "ConsoleUI.h"
#include "AuthService.h"
#include "CenterRepository.h"
#include "CenterService.h"
#include "CustomerRepository.h"
#include "CustomerService.h"
#include "DisplayHelper.h"
#include "Exceptions.h"
#include "HallRepository.h"
#include "HallService.h"
#include "JsonStorage.h"
#include "UserRepository.h"

#include <iostream>

using namespace std;

int main() {
    try {
        JsonStorage storage;
        storage.load();
        UserRepository userRepository(storage);
        CustomerRepository customerRepository(storage);
        CenterRepository centerRepository(storage);
        HallRepository hallRepository(storage);

        AuthService authService(userRepository);
        CustomerService customerService(customerRepository);
        CenterService centerService(centerRepository);
        HallService hallService(hallRepository, centerRepository);
        authService.seedDefaultUsers();

        cout << "Application started.\n";
        cout << "Data file loaded successfully.\n";

        ConsoleUI ui(authService, customerService, centerService, hallService);
        ui.run();
    } catch (const AppException& e) {
        DisplayHelper::showError(e.what());
        cout << "Application stopped because required data could not be loaded.\n";
    }
    return 0;
}
