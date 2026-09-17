#pragma once

#include "AuthService.h"
#include "CenterService.h"
#include "CustomerService.h"
#include "HallService.h"

#include <optional>

using namespace std;

class ConsoleUI {
private:
    AuthService& authService;
    CustomerService& customerService;
    CenterService& centerService;
    HallService& hallService;
    optional<User> currentUser;

public:
    ConsoleUI(AuthService& authService, CustomerService& customerService,
              CenterService& centerService, HallService& hallService);
    void run();

private:
    void showStartMenu() const;
    void showMainMenu() const;
    void showCustomerMenu();
    void showCenterHallMenu();
    void showCenterMenu();
    void showHallMenu();
    void login();
    void handleUnavailableModule(const string& module, const string& action = "READ") const;
    void requirePermission(const string& module, const string& action) const;
    int readMenuChoice(int minimum, int maximum) const;
};
