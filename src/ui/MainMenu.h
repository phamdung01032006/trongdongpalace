#pragma once

#include "../services/CustomerService.h"
#include "../services/HallService.h"
#include "../services/EventService.h"

class MainMenu {
private:
    CustomerService& customerService;
    HallService& hallService;
    EventService& eventService;
public:
    MainMenu(CustomerService& customerService, HallService& hallService, EventService& eventService);
    void show();
};
