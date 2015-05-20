#ifndef _QUEUE_H_
#define _QUEUE_H_

class PCB;

class SleepingPriorityQueue{
	void unlink_first();
public:
	void put(PCB *pcb,Time t);
	void checkSleeping();// decrementing time, putting into scheduler those with time 0;
}



#endif