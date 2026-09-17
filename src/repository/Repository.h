#pragma once

#include <string>
#include <vector>

using namespace std;

// Common CRUD contract. Day 6 implements this contract for the models used
// through Day 10; later repositories reuse the same method names.
template <typename T>
class Repository {
public:
    virtual ~Repository() = default;
    virtual void load() = 0;
    virtual void save() = 0;
    virtual void add(const T& item) = 0;
    virtual void update(const T& item) = 0;
    virtual void remove(const string& id) = 0;
    virtual T findById(const string& id) const = 0;
    virtual vector<T> getAll() const = 0;
};
