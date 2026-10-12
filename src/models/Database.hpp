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

namespace tdp {

struct Database {
    std::vector<User> users;
    std::vector<Customer> customers;
    std::vector<EventRequest> requests;
    std::vector<Center> centers;
    std::vector<Hall> halls;
    std::vector<ServiceItem> services;
    std::vector<Quotation> quotations;
    std::vector<Booking> bookings;
    std::vector<Contract> contracts;
    std::vector<Payment> payments;
    std::vector<Event> events;
    std::vector<OperationTask> tasks;
    std::vector<Incident> incidents;
    std::vector<AuditLog> logs;
};

}  // namespace tdp
