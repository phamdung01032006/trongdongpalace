#pragma once

#include "JsonDocumentRepository.h"

// Event is an abstract polymorphic hierarchy. Its typed repository belongs to
// the Event-service day; this foundation persists event documents safely.
class EventRepository final : public JsonDocumentRepository {
public:
    explicit EventRepository(JsonStorage& storage) : JsonDocumentRepository(storage, "events") {}
};
