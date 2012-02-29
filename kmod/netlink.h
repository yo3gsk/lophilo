#include "lophilo.h"

#ifdef __KERNEL__ 
  #include <net/genetlink.h>
#else
  #include <netlink/netlink.h>
  #include <netlink/attr.h>
  #include <netlink/genl/genl.h>
  #include <netlink/genl/ctrl.h>
#endif

#define LOPHILO_FAMILY_NAME "LOPHILO"
#define LOPHILO_MCGRP "LOPHILO_MCAST"
#define LOPHILO_VERSION_NR 0x01

#ifndef __KERNEL__
static struct nla_policy lophilo_notify_policy[LOPHILO_U_MAX + 1] = {
	[LOPHILO_U_SOURCE] = { .type = NLA_U32 },
	[LOPHILO_U_VALUE] = { .type = NLA_U32 },
};
#endif

#ifdef __KERNEL__
  int lophilo_notify(enum lophilo_sources source, int value);
  int lophilo_netlink_init(void);
  void lophilo_netlink_cleanup(void);
#endif
