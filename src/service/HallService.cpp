#include "HallService.h"

HallService::HallService(HallRepository& hallRepository, CenterRepository& centerRepository)
    : hallRepository(hallRepository), centerRepository(centerRepository) {}

void HallService::validateCenter(const string& centerId) const {
    centerRepository.findById(centerId);
}

void HallService::create(const Hall& hall) {
    validateCenter(hall.getCenterId());
    hallRepository.add(hall);
}

vector<Hall> HallService::getAll() const { return hallRepository.getAll(); }
Hall HallService::findById(const string& id) const { return hallRepository.findById(id); }

void HallService::update(const Hall& hall) {
    validateCenter(hall.getCenterId());
    hallRepository.update(hall);
}

void HallService::remove(const string& id) { hallRepository.remove(id); }
