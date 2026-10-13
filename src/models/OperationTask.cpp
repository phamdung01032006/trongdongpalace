#include "models/OperationTask.hpp"

using namespace std;

namespace tdp {

OperationTask::OperationTask(string id, string eventId, string title, string assignee, DateTime due,
                             TaskStatus status, string note)
    : id_(move(id)),
      eventId_(move(eventId)),
      title_(move(title)),
      assignee_(move(assignee)),
      note_(move(note)),
      due_(due),
      status_(status) {}

nlohmann::json OperationTask::toJson() const {
    return {{"id", id_}, {"eventId", eventId_}, {"title", title_}, {"assignee", assignee_},
            {"due", dateToJson(due_)}, {"status", toString(status_)}, {"note", note_}};
}

OperationTask OperationTask::fromJson(const nlohmann::json& j) {
    return OperationTask(j.at("id").get<string>(), j.at("eventId").get<string>(), j.at("title").get<string>(),
                         j.value("assignee", ""), dateFromJson(j.at("due")),
                         taskStatusFromString(j.value("status", "CanLam")), j.value("note", ""));
}

}  // namespace tdp
