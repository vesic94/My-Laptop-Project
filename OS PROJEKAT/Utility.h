#ifndef _CONSTANTS_H_
#define CONSTANTS_H_

#define ChangeToSystemStack 0x0060
int lck=0;

#define lock(){	lck++;	}
#define unlock(	if (lck) lck--;	)



#endif