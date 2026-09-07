#pragma once
#include <string>
#include <vector>
#include "../models/Hall.h"
#include "../repositories/HallRepository.h"
using namespace std;
class HallService { private: HallRepository& repository; void validate(const Hall& hall) const; public: explicit HallService(HallRepository& repository); void create(const string& code, const string& name, int capacity, HallStatus status); Hall findByCode(const string& code) const; vector<Hall> findAll() const; void update(const string& code, const string& name, int capacity, HallStatus status); };
