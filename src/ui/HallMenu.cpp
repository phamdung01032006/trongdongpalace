#include "HallMenu.h"
#include "../utils/InputHelper.h"
#include "../utils/DisplayHelper.h"
#include "../exceptions/AppException.h"
#include <iostream>
using namespace std;
HallMenu::HallMenu(HallService& service):service(service){}
HallStatus HallMenu::readStatus(){string value=InputHelper::readString("Status (ACTIVE/INACTIVE): ");if(value=="ACTIVE"||value=="active")return HallStatus::ACTIVE;if(value=="INACTIVE"||value=="inactive")return HallStatus::INACTIVE;throw ValidationException("Status must be ACTIVE or INACTIVE.");}
void HallMenu::create(){string code=InputHelper::readString("Hall ID: ");string name=InputHelper::readString("Hall name: ");int capacity=InputHelper::readInt("Capacity: ");HallStatus status=readStatus();service.create(code,name,capacity,status);cout<<"Hall created successfully.\n";}
void HallMenu::viewAll(){auto halls=service.findAll();if(halls.empty()){cout<<"No halls found.\n";return;}for(const Hall& hall:halls)cout<<hall.getHallCode()<<" - "<<hall.getName()<<" | Capacity: "<<hall.getCapacity()<<" | "<<(hall.getStatus()==HallStatus::ACTIVE?"ACTIVE":"INACTIVE")<<"\n";}
void HallMenu::update(){string code=InputHelper::readString("Hall ID: ");service.update(code,InputHelper::readString("New hall name: "),InputHelper::readInt("New capacity: "),readStatus());cout<<"Hall updated successfully.\n";}
void HallMenu::show(){bool running=true;while(running){DisplayHelper::showHeader("HALL MANAGEMENT");cout<<"1. Create Hall\n2. View All Halls\n3. Update Hall\n4. Back\n";try{switch(InputHelper::readInt("Enter your choice: ")){case 1:create();break;case 2:viewAll();break;case 3:update();break;case 4:running=false;continue;default:DisplayHelper::showError("Invalid choice. Please try again.");}}catch(const AppException& exception){DisplayHelper::showError(exception.what());}DisplayHelper::pause();}}
