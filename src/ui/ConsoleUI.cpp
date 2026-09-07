#include "ConsoleUI.h"
#include "MainMenu.h"
#include "../repositories/CustomerRepository.h"
#include "../repositories/HallRepository.h"
#include "../repositories/EventRepository.h"
#include "../services/CustomerService.h"
#include "../services/HallService.h"
#include "../services/EventService.h"

ConsoleUI:: ConsoleUI(DatabaseManager& db) : db(db) {}

void ConsoleUI::start() {
    CustomerRepository customerRepository(db);
    HallRepository hallRepository(db);
    EventRepository eventRepository(db);
    CustomerService customerService(customerRepository);
    HallService hallService(hallRepository);
    EventService eventService(eventRepository, customerRepository);
    MainMenu mainMenu(customerService, hallService, eventService);
    mainMenu.show();
}
