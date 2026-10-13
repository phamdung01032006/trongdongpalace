#pragma once

#include "models/AuditLog.hpp"
#include "models/Booking.hpp"
#include "models/Center.hpp"
#include "models/Contract.hpp"
#include "models/Customer.hpp"
#include "models/Enums.hpp"
#include "models/Event.hpp"
#include "models/EventRequest.hpp"
#include "models/Hall.hpp"
#include "models/Incident.hpp"
#include "models/OperationTask.hpp"
#include "models/Payment.hpp"
#include "models/Quotation.hpp"
#include "models/ServiceItem.hpp"
#include "models/User.hpp"

#include <vector>

using namespace std;

namespace tdp {

struct Database {
    vector<User> users;
    vector<Customer> customers;
    vector<EventRequest> requests;
    vector<Center> centers;
    vector<Hall> halls;
    vector<ServiceItem> services;
    vector<Quotation> quotations;
    vector<Booking> bookings;
    vector<Contract> contracts;
    vector<Payment> payments;
    vector<Event> events;
    vector<OperationTask> tasks;
    vector<Incident> incidents;
    vector<AuditLog> logs;
};

}  // namespace tdp
