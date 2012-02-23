#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h> // module_init, module_exit
#include <linux/fs.h> // file_operations

#include "timer.h"
#include "netlink.h"
#include "data.h"
#include "mmap.h"
 
#define MAP_MAJOR 126
 
static int __init lophilo_init(void)
{
	struct file_operations* map_fops;
	int rc;

	printk(KERN_INFO "INIT LOPHILO\n");

	rc = lophilo_data_init();
	if(rc)
		goto failure;

	map_fops = lophilo_map_init();
	if(!map_fops)
		goto data_cleanup;

	rc = lophilo_netlink_init();
	if(rc)
		goto map_cleanup;

	rc = lophilo_timer_init();
	if(rc)
		goto netlink_cleanup;
			

	if (register_chrdev(MAP_MAJOR,"map", map_fops) <0 ) {
		printk("unable to get major for map module\n");
		goto timer_cleanup;
	}

	return 0;

timer_cleanup:
	lophilo_timer_cleanup();
netlink_cleanup:
	lophilo_netlink_cleanup();
map_cleanup:
	lophilo_map_cleanup();
data_cleanup:
	lophilo_data_cleanup();
failure:
	printk(KERN_INFO "One subsystem init failed\n");

	return -EBUSY;

}
 
static void __exit lophilo_cleanup(void)
{
	printk(KERN_INFO "CLEANUP LOPHILO\n");
	unregister_chrdev(MAP_MAJOR, "map");
	lophilo_timer_cleanup();
	lophilo_netlink_cleanup();
	lophilo_map_cleanup();
	lophilo_data_cleanup();

	return;
}
 
 
module_init(lophilo_init);
module_exit(lophilo_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ricky Ng-Adam");	
MODULE_DESCRIPTION("Lophilo support");
