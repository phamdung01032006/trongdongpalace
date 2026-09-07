#include "HallService.h"
#include "../exceptions/AppException.h"
HallService::HallService(HallRepository& repository) : repository(repository) {}
void HallService::validate(const Hall& hall) const { if(hall.getHallCode().empty()) throw ValidationException("Hall ID is required."); if(hall.getName().empty()) throw ValidationException("Hall name is required."); if(hall.getCapacity()<=0) throw ValidationException("Capacity must be greater than 0."); }
void HallService::create(const string& code,const string& name,int capacity,HallStatus status){Hall hall(0,1,code,name,capacity,status);validate(hall);if(repository.findByCode(code).has_value())throw BusinessException("Hall ID already exists.");repository.save(hall);}
Hall HallService::findByCode(const string& code) const{auto hall=repository.findByCode(code);if(!hall.has_value())throw EntityNotFoundException("Hall not found.");return *hall;}
vector<Hall> HallService::findAll() const{return repository.findAll();}
void HallService::update(const string& code,const string& name,int capacity,HallStatus status){Hall hall=findByCode(code);hall.setName(name);hall.setCapacity(capacity);hall.setStatus(status);validate(hall);repository.update(hall);}
