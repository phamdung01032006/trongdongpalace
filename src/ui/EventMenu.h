#pragma once
#include "../services/EventService.h"
class EventMenu { private: EventService& service; EventStatus readStatus(); void create(); void viewAll(); void search(); void update(); public: explicit EventMenu(EventService& service); void show(); };
