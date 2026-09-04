#include "ConsoleUI.h"
#include "MainMenu.h"

ConsoleUI:: ConsoleUI(DatabaseManager& db) : db(db) {}

void ConsoleUI::start() {
    MainMenu mainMenu;
    mainMenu.show();
}