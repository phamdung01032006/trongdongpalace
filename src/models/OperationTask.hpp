#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

class OperationTask {
public:
    OperationTask() = default;
    OperationTask(string id, string eventId, string title, string assignee, DateTime due,
                  TaskStatus status, string note);
    const string& id() const { return id_; }
    const string& eventId() const { return eventId_; }
    const string& title() const { return title_; }
    const string& assignee() const { return assignee_; }
    DateTime due() const { return due_; }
    TaskStatus status() const { return status_; }
    const string& note() const { return note_; }
    void setTitle(string v) { title_ = move(v); }
    void setAssignee(string v) { assignee_ = move(v); }
    void setDue(DateTime v) { due_ = v; }
    void setStatus(TaskStatus v) { status_ = v; }
    void setNote(string v) { note_ = move(v); }
    nlohmann::json toJson() const;
    static OperationTask fromJson(const nlohmann::json& j);

private:
    string id_, eventId_, title_, assignee_, note_;
    DateTime due_;
    TaskStatus status_ = TaskStatus::CanLam;
};

}  // namespace tdp
