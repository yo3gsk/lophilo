#include "client.h"

#include <stdio.h>

void update(int source, int value)
{
	printf("update received for: %d value %d\n", source, value);
}

int main(void)
{
	if(lophilo_init(update))
		return 1;
	if(lophilo_register(LOPHILO_TYPE_ADC, LOPHILO_PIN_XA0, 1000))
		return 1;
	if(lophilo_loop())
		return 1;
	return 0;
}
