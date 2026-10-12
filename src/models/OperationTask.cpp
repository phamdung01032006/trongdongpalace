#include "models/OperationTask.hpp"

namespace tdp {

OperationTask::OperationTask(std::string id, std::string eventId, std::string title, std::string assignee, DateTime due,
                             TaskStatus status, std::string note)
    : id_(std::move(id)),
      eventId_(std::move(eventId)),
      title_(std::move(title)),
      assignee_(std::move(assignee)),
      note_(std::move(note)),
      due_(due),
      status_(status) {}

nlohmann::json OperationTask::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"title", title_}, {"assignee", assignee_},
            {"due", dateToJson(due_)}, {"status", toString(status_)}, {"note", note_}};
}

OperationTask OperationTask::fromJson(const nlohmann::json& j) {
    return OperationTask(j.at("id").get<std::string>(), j.at("eventId").get<std::string>(), j.at("title").get<std::string>(),
                         j.value("assignee", ""), dateFromJson(j.at("due")),
                         taskStatusFromString(j.value("status", "CanLam")), j.value("note", ""));
}

}  // namespace tdp
