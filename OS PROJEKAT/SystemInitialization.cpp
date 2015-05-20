#include SystemInicialization.h 

void initialize_system_state(){
	lock();
	//inicijalizuj tajmer i sve sto treba old=tajmer; tajmer=new;
	unlock();
}

void restore_system_state(){
	lock();
	//vrati sve sto treba tajmer=old;
	unlock();
}