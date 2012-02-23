#include "data.h"

#include "netlink.h"

static struct lophilo_data* loldata;

unsigned long lophilo_get_data(void) 
{
	return virt_to_phys((void *)loldata) >> PAGE_SHIFT;
}

void lophilo_data_update(void) 
{
	if(loldata) {
		loldata->p0++;
		lophilo_notify(LOPHILO_PIN_XA0, loldata->p0);
	}
}

int lophilo_data_init(void) 
{
	loldata = (struct lophilo_data*) kmalloc(DATA_SIZE, GFP_KERNEL);
	if(!loldata) {
		printk("unable to allocate with kmalloc");
		return -EBUSY;
	}
	loldata->p0 = 0;
	return 0;
}

void lophilo_data_cleanup(void)
{
	if(loldata)
		kfree(loldata);
}
