//File : thread.h 
#ifndef _thread_h_
#define _thread_h_


typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; //time, x 55ms
const Time defaultTimeSlice = 2; // default=2*55ms
typedef int ID;

class PCK;

class Thread{
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	
	static void sleep(Time timeToSleep);
	
protected:
	friend class PCB;
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice= defaultTimeSlice);
	virtual void run() {}
	
private:
	ID myPCB;
	//static void wrapper(Thread* running);
};

void dispatch();


#endif