#include "mmap.h"
#include "data.h" // get_data()

#include <linux/fs.h> // file_operations
#include <linux/mm.h> // remap_pfn_range

 
#define MAP_MAJOR 126
 
static int map_mmap(struct file *filp, struct vm_area_struct *vma);
 
struct file_operations map_fops =
{
  .open    = nonseekable_open,
  .mmap    = map_mmap,
};

static int map_mmap(struct file *filp, struct vm_area_struct *vma)
{
	long unsigned int size = vma->vm_end - vma->vm_start;
	
	if(size != DATA_SIZE) {
		printk(KERN_INFO "Invalid allocation request, expected %ld, got %ld", DATA_SIZE, size);
                return -EAGAIN;
	}
	if (remap_pfn_range(vma, vma->vm_start, lophilo_get_data(), DATA_SIZE, vma->vm_page_prot)) {
		printk(KERN_INFO "Allocation failed!");
                return -EAGAIN;
	}
	return 0;
}
 
struct file_operations* lophilo_map_init(void)
{
	return &map_fops;
}
 
void lophilo_map_cleanup(void)
{
	return;
}
