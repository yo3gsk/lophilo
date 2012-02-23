#include "netlink.h"

#include <linux/module.h>
#include <linux/kernel.h>

#define LOPHILO_OWN_PID 0
#define LOPHILO_NL_FLAGS 0
#define LOPHILO_NL_AUTO 0

static int lophilo_register(struct sk_buff *skb, struct genl_info *info);

static struct genl_multicast_group lophilo_mcgrp = {
	.name = LOPHILO_MCGRP,
};

static struct nla_policy lophilo_register_policy[LOPHILO_R_MAX + 1] = {
	[LOPHILO_R_TYPE] = { .type = NLA_U32 },
	[LOPHILO_R_PERIOD_MS] = { .type = NLA_U32 },
	[LOPHILO_R_SOURCE] = { .type = NLA_U32 },
};

static struct genl_ops lophilo_ops[] = {
	{
	.cmd = LOPHILO_CMD_REGISTER,
	.policy = lophilo_register_policy,
	.doit = lophilo_register,
	},
};

static struct genl_family lophilo_family = {
	.id = GENL_ID_GENERATE,         
	.hdrsize = 0,
	.name = LOPHILO_FAMILY_NAME,   
	.version = LOPHILO_VERSION_NR, 
	.maxattr = LOPHILO_R_MAX,
};

static pid_t sender_pid = 0;
struct net * sender_net;
static int sequence = 0;

static int lophilo_register(struct sk_buff *skb, struct genl_info *info)
{
	u32 type, source;
	if(!info) {
		printk("null info structure\n");
		return -EINVAL;
	}
       	if (info->attrs[LOPHILO_R_TYPE] && info->attrs[LOPHILO_R_SOURCE]) {
		type = nla_get_u32(info->attrs[LOPHILO_R_TYPE]);
		source = nla_get_u32(info->attrs[LOPHILO_R_SOURCE]);
		if(!type || !source) {
			printk("invalid type and/or source\n");
			return -EINVAL;
		}
		printk("observer requested: %d %d\n", type, source);
		sender_pid = info->snd_pid;
		sender_net = genl_info_net(info);
	} else {
		printk("invalid register message\n");
		return -EINVAL;
	}
	return 0;
}

int lophilo_notify(enum lophilo_sources source, int value)
{
        struct sk_buff *skb;
	void *msg_head;
	int rc;
	int skb_total_length;
	if(!sender_net || !sender_pid) {
		return 0;
	}
	//printk(KERN_DEBUG "notifying change from source: %d value: %d\n", source, value);
	
        /* allocate some memory, since the size is not yet known use NLMSG_GOODSIZE
	 * we're inside a timer interrupt, so use GFP_ATOMIC... may fail.*/	
        skb = genlmsg_new(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_INFO "Unable to allocate %ld bytes\n", NLMSG_GOODSIZE);
		return -ENOMEM;
	}

       	msg_head = genlmsg_put(skb, LOPHILO_OWN_PID, sequence++, &lophilo_family, LOPHILO_NL_FLAGS, LOPHILO_CMD_UPDATE);
	if (!msg_head) {
		printk(KERN_INFO "Unable to create message head\n");
		return -ENOMEM;
	}
	NLA_PUT_U32(skb, LOPHILO_U_SOURCE, source);
	NLA_PUT_U32(skb, LOPHILO_U_VALUE, value);
	
	skb_total_length = genlmsg_end(skb, msg_head);

	// FIXME: multicast not working
	//rc = genlmsg_multicast(skb, LOPHILO_OWN_PID, lophilo_mcgrp.id, GFP_ATOMIC);
	rc = genlmsg_unicast(sender_net, skb, sender_pid);
	if (rc < 0) {
		printk(KERN_INFO "Unable to send message to group %d, error %d\n", lophilo_mcgrp.id, rc);
		sender_net = NULL;
		sender_pid = 0;
		return rc;
	} 
	return 0;

 nla_put_failure:
        printk("an error occured adding attributes to message\n");
	return -EMSGSIZE;
}

int lophilo_netlink_init(void)
{
	int rc;
        printk("INIT GENERIC NETLINK LOPHILO MODULE\n");
        
	rc = genl_register_family_with_ops(&lophilo_family, lophilo_ops, ARRAY_SIZE(lophilo_ops));
	if (rc) {
		printk("Error registering genl netlink family\n");
		return rc;
	}
	rc = genl_register_mc_group(&lophilo_family, &lophilo_mcgrp);
	if(rc) {
		printk("unable to register multicast group");
		genl_unregister_family(&lophilo_family);
		return -EINVAL;
	}
	return 0;
}

void lophilo_netlink_cleanup(void)
{
        int rc;
        printk("EXIT GENERIC NETLINK LOPHILO MODULE\n");
	rc = genl_unregister_family(&lophilo_family);
	if(rc) {
		printk("genetlink exit failure: %i\n", rc);
	}
	// once family is unregistered, all multicast groups are also unregistered
	return;

}
