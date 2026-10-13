#pragma once

#include "models/Booking.hpp"
#include "models/Customer.hpp"
#include "models/Enums.hpp"
#include "models/Event.hpp"
#include "models/EventRequest.hpp"
#include "models/Hall.hpp"
#include "models/Payment.hpp"
#include "models/ServiceItem.hpp"
#include "models/User.hpp"

#include <vector>

using namespace std;

namespace tdp {

struct Database {
    vector<User> users;
    vector<Customer> customers;
    vector<EventRequest> requests;
    vector<Hall> halls;
    vector<ServiceItem> services;
    vector<Booking> bookings;
    vector<Payment> payments;
    vector<Event> events;
};

}  // namespace tdp
