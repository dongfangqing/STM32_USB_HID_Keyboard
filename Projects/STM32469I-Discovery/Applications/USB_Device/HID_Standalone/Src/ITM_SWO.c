#include "ITM_SWO.h"


int fputc(int ch, FILE *f)
{		
	ITM_SendChar ( ch);
	return (ch);
}

