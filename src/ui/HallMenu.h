#pragma once
#include "../services/HallService.h"
class HallMenu { private: HallService& service; HallStatus readStatus(); void create(); void viewAll(); void update(); public: explicit HallMenu(HallService& service); void show(); };
