#include "timer.h"
#include "data.h"

#include <linux/hrtimer.h>

static struct timer_list timer_1s;

int lophilo_timer_init(void) {
	setup_timer( &timer_1s, lophilo_timer_callback, 0 );
	printk( "Starting timer to fire in 200ms (%ld)\n", jiffies );
	if(mod_timer( &timer_1s, jiffies + msecs_to_jiffies(200))) {
		printk("attempt at modifying active timer to same value");
		return 1;
	}
	return 0;
}

void lophilo_timer_callback( unsigned long data )
{
	lophilo_data_update();
	if(mod_timer(&timer_1s, jiffies + msecs_to_jiffies(1000))) {
		printk(KERN_INFO "Unable to reset timer");
	}
} 

void lophilo_timer_cleanup(void)
{
	int ret;
	printk("Timer module uninstalling\n");
	ret = del_timer( &timer_1s );
	if (ret) 
		printk("The timer is still in use...\n");
}
