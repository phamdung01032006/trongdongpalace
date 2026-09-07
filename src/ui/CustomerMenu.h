#pragma once
#include "../services/CustomerService.h"
class CustomerMenu { private: CustomerService& service; void create(); void viewAll(); void search(); void update(); void remove(); public: explicit CustomerMenu(CustomerService& service); void show(); };
