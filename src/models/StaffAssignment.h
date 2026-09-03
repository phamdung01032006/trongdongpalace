#pragma once
#include <string>
using namespace std;

class StaffAssignment {
private:
    int id;
    int eventId;
    int staffId; //tham chieu users.id
    string assignmentRole;
    string status;

public:
    StaffAssignment(int id,
                    int eventId,
                    int staffId,
                    const string& assignmentRole,
                    const string& status)
                : id(id),
                eventId(eventId),
                staffId(staffId),
                assignmentRole(assignmentRole),
                status(status) {}
    
    // Getters
    int getId() const { return id; }
    int getEventId() const { return eventId; }
    int getStaffId() const { return staffId; }
    const string& getAssignmentRole() const { return assignmentRole; }
    const string& getStatus() const { return status; }

    // Setters
    void setEventId(int newEventId) { eventId = newEventId; }
    void setStaffId(int newStaffId) { staffId = newStaffId; }
    void setAssignmentRole(const string& newAssignmentRole) { assignmentRole = newAssignmentRole; }
    void setStatus(const string& newStatus) { status = newStatus; }
};