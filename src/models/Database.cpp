#ifndef TDP_MODELS_DATABASE_CPP
#define TDP_MODELS_DATABASE_CPP

#include "models/Booking.cpp"
#include "models/Customer.cpp"
#include "models/Enums.cpp"
#include "models/Event.cpp"
#include "models/EventRequest.cpp"
#include "models/Hall.cpp"
#include "models/Payment.cpp"
#include "models/ServiceItem.cpp"
#include "models/User.cpp"

#include <vector>

using namespace std;

struct Database {
    vector<User> users;
    vector<Customer> customers;
    vector<EventRequest> requests;
    vector<Hall> halls;
    vector<ServiceItem> services;
    vector<Booking> bookings;
    vector<Payment> payments;
    vector<shared_ptr<Event>> events;
};

#endif
