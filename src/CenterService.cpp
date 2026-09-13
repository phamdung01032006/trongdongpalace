#include "CenterService.h"

CenterService::CenterService(CenterRepository& repository) : repository(repository) {}

void CenterService::create(const Center& center) { repository.add(center); }
vector<Center> CenterService::getAll() const { return repository.getAll(); }
Center CenterService::findById(const string& id) const { return repository.findById(id); }
void CenterService::update(const Center& center) { repository.update(center); }
void CenterService::remove(const string& id) { repository.remove(id); }
