#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x80567cab, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xaa5b0f7, "kmalloc_caches" },
	{ 0x1574c0b9, "del_timer" },
	{ 0x20ad675f, "genl_unregister_family" },
	{ 0xc9a49e2c, "__register_chrdev" },
	{ 0x9e1bdc28, "init_timer_key" },
	{ 0xf07d4184, "nonseekable_open" },
	{ 0x7d11c268, "jiffies" },
	{ 0x2bc95bd4, "memset" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
	{ 0xf6b14cdc, "nla_put" },
	{ 0xce095088, "mod_timer" },
	{ 0x862b902a, "netlink_unicast" },
	{ 0x1b92e0fa, "genl_register_family_with_ops" },
	{ 0x5062db4c, "__alloc_skb" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x6e712077, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0x559e4626, "remap_pfn_range" },
	{ 0x793431a5, "genl_register_mc_group" },
	{ 0x1ed9e8f3, "skb_put" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9D02A13455978451E2605C8");
