#pragma once

#include "CenterRepository.h"
#include "HallRepository.h"

class HallService {
private:
    HallRepository& hallRepository;
    CenterRepository& centerRepository;

    void validateCenter(const string& centerId) const;

public:
    HallService(HallRepository& hallRepository, CenterRepository& centerRepository);

    void create(const Hall& hall);
    vector<Hall> getAll() const;
    Hall findById(const string& id) const;
    void update(const Hall& hall);
    void remove(const string& id);
};
