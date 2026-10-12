#pragma once

#include "models/Enums.hpp"
#include "utils/helpers.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace tdp {

class OperationTask {
public:
    OperationTask() = default;
    OperationTask(std::string id, std::string eventId, std::string title, std::string assignee, DateTime due,
                  TaskStatus status, std::string note);
    const std::string& id() const { return id_; }
    const std::string& eventId() const { return eventId_; }
    const std::string& title() const { return title_; }
    const std::string& assignee() const { return assignee_; }
    DateTime due() const { return due_; }
    TaskStatus status() const { return status_; }
    const std::string& note() const { return note_; }
    void setTitle(std::string v) { title_ = std::move(v); }
    void setAssignee(std::string v) { assignee_ = std::move(v); }
    void setDue(DateTime v) { due_ = v; }
    void setStatus(TaskStatus v) { status_ = v; }
    void setNote(std::string v) { note_ = std::move(v); }
    nlohmann::json toJson() const;
    static OperationTask fromJson(const nlohmann::json& j);

private:
    std::string id_, eventId_, title_, assignee_, note_;
    DateTime due_;
    TaskStatus status_ = TaskStatus::CanLam;
};

}  // namespace tdp
