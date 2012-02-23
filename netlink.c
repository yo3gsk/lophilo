#include "netlink.h"

#include <linux/module.h>
#include <linux/kernel.h>

#define LOPHILO_OWN_PID 0
#define LOPHILO_NL_FLAGS 0
#define LOPHILO_NL_AUTO 0

/*static struct genl_multicast_group lophilo_mc_group = {
     .name = LOPHILO_GENL_MCAST_GROUP_NAME,
};*/


static struct genl_family lophilo_family = {
	.id = GENL_ID_GENERATE,         //genetlink should generate an id
	.hdrsize = 0,
	.name = LOPHILO_FAMILY_NAME,        //the name of this family, used by userspace application
	.version = LOPHILO_VERSION_NR,                   //version number  
	.maxattr = LOPHILO_R_MAX,
};

int lophilo_register(struct sk_buff *skb_2, struct genl_info *info)
{
	int* type;
	int* source;
       	if (info->attrs[LOPHILO_R_TYPE] && info->attrs[LOPHILO_R_SOURCE]) {
		type = (int*) nla_data(info->attrs[LOPHILO_R_TYPE]);
		source = (int*) nla_data(info->attrs[LOPHILO_R_TYPE]);
		printk("observer requested: %d %d\n", *type, *source);
	} else {
		printk("invalid register message");
		return -1;
	}
		
	return 0;
}

struct genl_ops lophilo_register_gnl_ops = {
	.cmd = LOPHILO_C_REGISTER,
	.flags = 0,
	.policy = lophilo_register_genl_policy,
	.doit = lophilo_register,
	.dumpit = NULL,
};

int lophilo_notify(enum lophilo_sources source, int value)
{
        struct sk_buff *skb;
	void *msg_head;
	int rc;
	int skb_total_length;

	//printk(KERN_DEBUG "notifying change from source: %d value: %d\n", source, value);
	
        /* allocate some memory, since the size is not yet known use NLMSG_GOODSIZE
	 * we're inside a timer interrupt, so use GFP_ATOMIC... may fail.*/	
        skb = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (skb == NULL) {
		printk(KERN_INFO "Unable to allocate %ld bytes\n", NLMSG_GOODSIZE);
		goto out;
	}

       	msg_head = genlmsg_put(skb, LOPHILO_OWN_PID, LOPHILO_NL_AUTO, &lophilo_family, LOPHILO_NL_FLAGS, LOPHILO_C_UNSPEC);
	if (msg_head == NULL) {
		rc = -ENOMEM;
		printk(KERN_INFO "Unable to create message head\n");
		goto out;
	}
	NLA_PUT_U32(skb, LOPHILO_U_SOURCE, source);
	NLA_PUT_U32(skb, LOPHILO_U_VALUE, value);
	
	skb_total_length = genlmsg_end(skb, msg_head);
	//printk(KERN_DEBUG "skb total length: %d", skb_total_length);

	rc = genlmsg_multicast(skb, LOPHILO_OWN_PID, LOPHILO_GROUP, GFP_ATOMIC);
	if (rc) {
		//printk(KERN_INFO "Unable to send multicast to group %d, error %d\n", lophilo_mc_group.id, rc);
		printk(KERN_INFO "Unable to send multicast to group %d, error %d\n", lophilo_family.id, rc);
		goto out;
	}
	return 0;

 out:
	return 1;

 nla_put_failure:
        printk("an error occured adding attributes to message\n");
	return -EMSGSIZE;
}

int lophilo_netlink_init(void)
{
	int rc;
        printk("INIT GENERIC NETLINK LOPHILO MODULE\n");
        
	rc = genl_register_family_with_ops(&lophilo_family, &lophilo_register_gnl_ops, LOPHILO_C_MAX);
	if (rc) {
		printk("Error registering genl netlink family\n");
		return rc;
	}
	return 0;
}

void lophilo_netlink_cleanup(void)
{
        int ret;
        printk("EXIT GENERIC NETLINK LOPHILO MODULE\n");
	ret = genl_unregister_family(&lophilo_family);
	if(ret) {
		printk("genetlink exit failure: %i\n", ret);
		return ret;
	}
	return 0;

}
