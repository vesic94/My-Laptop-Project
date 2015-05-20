#include Thread.h


Thread::Thread(StackSize stackSize, Time timeslice)
{
	
}

Thread::~Thread()
{
	waitToComplete();
	
}