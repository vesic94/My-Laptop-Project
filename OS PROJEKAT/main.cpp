#include "Thread.h"

PCB *kernelStack;

extern int userMain(int argc, char* argv[]);

class UserThread: public Thread //nacin dobar
{
private:
	int argc;
	char **argv;
	int error;
public:
		UserThread(StackSize sz=defaultStackSize, Time t=defaultTimeSlice, int argcc, char* argvv[]):Thread(sz,t)
		{
			argc=argcc;
			argv=argvv;
		}
		void run(){
			
			error=userMain(argc,argv);
		}
		int getErrors(){return error;}
}

int main()
{
	
	kernelStack=new PCB(); // kernel stack
	PCB::running=new PCB();// user stack;
	
	PCB::idle = new Idle();
	Idle::idle->start(); // Kada nicega nema u Scheduleru mora da se izvrsava ova nit
	
	initialize_system_state();
	
	//************************************** ili 
	UserThread *userThread=new UserThread(defaultStackSize,defaultTimeSlice,argc,argv);
	userThread->start();
	dispatch();
	userThread->waitToComplete();
	int errors=userThread->getErrors();
	//*************************************** ili ovo ispod
	
	// pozivamo sa onim u running-u(Tekuca nit, ne kernelStack);
	int errors = userMain();  
							//(*) ukoliko napravimo klasu koja ima run i start
							//u cijem run-u pozivamo userMain() a izvedena je iz Thread
							//Onda nam treba waitToComplete
	
	restore_system_state();
	//running->waitToComplete(); //nepotrebno ako je ovo nit koja izvrsaca usermain . . ali (*)

	
	
	
	//nit roditelj kad se zavrsi poziva automatski destruktore svih niti
	//koje je ona napravila, ovo je savrseno mesto(U destruktoru niti)
	//da nit potomak blokira nit roditelja dok se ne zavrsi sama nit potomak.
	//Kod maina to nije slucaj jer se ne poziva destruktor niti 
	return errors; //  0/1
}