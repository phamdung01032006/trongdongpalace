#pragma once

#include "CenterRepository.h"

class CenterService {
private:
    CenterRepository& repository;

public:
    explicit CenterService(CenterRepository& repository);

    void create(const Center& center);
    vector<Center> getAll() const;
    Center findById(const string& id) const;
    void update(const Center& center);
    void remove(const string& id);
};
