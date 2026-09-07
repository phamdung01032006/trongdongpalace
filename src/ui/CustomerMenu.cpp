#include "CustomerMenu.h"
#include "../utils/InputHelper.h"
#include "../utils/DisplayHelper.h"
#include "../exceptions/AppException.h"
#include <iostream>
using namespace std;
CustomerMenu::CustomerMenu(CustomerService& service):service(service){}
void CustomerMenu::create(){string code=InputHelper::readString("Customer ID: ");string name=InputHelper::readString("Name: ");string phone=InputHelper::readString("Phone: ");string email=InputHelper::readString("Email: ");string address=InputHelper::readString("Address: ");service.create(code,name,phone,email,address);cout<<"Customer created successfully.\n";}
void CustomerMenu::viewAll(){auto customers=service.findAll();if(customers.empty()){cout<<"No customers found.\n";return;}for(const Customer& customer:customers)cout<<customer.getCustomerCode()<<" - "<<customer.getName()<<" | "<<customer.getPhone()<<"\n";}
void CustomerMenu::search(){Customer customer=service.findByCode(InputHelper::readString("Customer ID: "));cout<<customer.getCustomerCode()<<" - "<<customer.getName()<<"\nPhone: "<<customer.getPhone()<<"\nEmail: "<<customer.getEmail()<<"\nAddress: "<<customer.getAddress()<<"\n";}
void CustomerMenu::update(){string code=InputHelper::readString("Customer ID: ");string name=InputHelper::readString("New name: ");string phone=InputHelper::readString("New phone: ");string email=InputHelper::readString("New email: ");string address=InputHelper::readString("New address: ");service.update(code,name,phone,email,address);cout<<"Customer updated successfully.\n";}
void CustomerMenu::remove(){string code=InputHelper::readString("Customer ID: ");string confirm=InputHelper::readString("Type Y to confirm delete: ");if(confirm=="Y"||confirm=="y"){service.remove(code);cout<<"Customer deleted successfully.\n";}else cout<<"Delete cancelled.\n";}
void CustomerMenu::show(){bool running=true;while(running){DisplayHelper::showHeader("CUSTOMER MANAGEMENT");cout<<"1. Create Customer\n2. View All Customers\n3. Search Customer\n4. Update Customer\n5. Delete Customer\n6. Back\n";try{switch(InputHelper::readInt("Enter your choice: ")){case 1:create();break;case 2:viewAll();break;case 3:search();break;case 4:update();break;case 5:remove();break;case 6:running=false;continue;default:DisplayHelper::showError("Invalid choice. Please try again.");} }catch(const AppException& exception){DisplayHelper::showError(exception.what());}DisplayHelper::pause();}}
