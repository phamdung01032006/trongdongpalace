#pragma once
#include <string>
#include <vector>
#include "../models/Event.h"
#include "../repositories/EventRepository.h"
#include "../repositories/CustomerRepository.h"
using namespace std;
class EventService { private: EventRepository& repository; CustomerRepository& customerRepository; void validate(const Event& event) const; public: EventService(EventRepository& repository, CustomerRepository& customerRepository); void create(const string& code,const string& customerCode,const string& type,const string& date,const string& startTime,const string& endTime,int guestCount); Event findByCode(const string& code) const; vector<Event> findAll() const; void update(const string& code,const string& customerCode,const string& type,const string& date,const string& startTime,const string& endTime,int guestCount,EventStatus status); };
