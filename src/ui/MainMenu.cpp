#include "MainMenu.h"
#include "CustomerMenu.h"
#include "HallMenu.h"
#include "EventMenu.h"
#include "../utils/InputHelper.h"
#include "../utils/DisplayHelper.h"
#include <iostream>
using namespace std;

MainMenu::MainMenu(CustomerService& customerService, HallService& hallService, EventService& eventService)
    : customerService(customerService), hallService(hallService), eventService(eventService) {}

void MainMenu::show() {
    bool running = true;

    while (running) {
        DisplayHelper::clearScreen();
        DisplayHelper::showHeader("TRONG DONG PALACE MANAGEMENT");

        cout << "1. Customer Management\n";
        cout << "2. Hall Management\n";
        cout << "3. Event Management\n";
        cout << "4. Booking Management\n";
        cout << "5. Package Management\n";
        cout << "6. Service Management\n";
        cout << "7. Quotation Management\n";
        cout << "8. Contract Management\n";
        cout << "9. Payment Management\n";
        cout << "10. Event Operations\n";
        cout << "11. Reports\n";
        cout << "12. Exit\n";
        cout << "--------------------------------------------------\n";

        int choice = InputHelper::readInt("Enter your choice: ");

        switch (choice) {
            case 1:
                CustomerMenu(customerService).show();
                break;
            case 2:
                HallMenu(hallService).show();
                break;
            case 3:
                EventMenu(eventService).show();
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                cout << "[INFO] This module is not implemented yet.\n";
                DisplayHelper::pause();
                break;

            case 12:
                cout << "Exiting application. Goodbye!\n";
                running = false;
                break;

            default:
                DisplayHelper::showError("Invalid choice. Please try again.");
                DisplayHelper::pause();
                break;
        }
    }
}
