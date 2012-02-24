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

enum lophilo_commands {
	LOPHILO_CMD_UNSPEC,
	LOPHILO_CMD_REGISTER,
	LOPHILO_CMD_UPDATE,
	__LOPHILO_CMD_MAX,
};
#define LOPHILO_CMD_MAX (__LOPHILO_CMD_MAX - 1)

enum request_attributes {
	LOPHILO_R_UNSPEC,
	LOPHILO_R_TYPE,
	LOPHILO_R_PERIOD_MS,
	LOPHILO_R_SOURCE,
        __LOPHILO_R_MAX,
};
#define LOPHILO_R_MAX (__LOPHILO_R_MAX - 1)

enum lophilo_sources_types {
	LOPHILO_TYPE_UNSPEC,
	LOPHILO_TYPE_ADC,
        __LOPHILO_TYPE_MAX,
};

enum lophilo_sources {
	LOPHILO_PIN_UNSPEC,
	LOPHILO_PIN_XA0,
        __LOPHILO_PIN_MAX,
};


enum update_attributes {
	LOPHILO_U_UNSPEC,
	LOPHILO_U_SOURCE,
	LOPHILO_U_VALUE,
        __LOPHILO_U_MAX,
};
#define LOPHILO_U_MAX (__LOPHILO_U_MAX - 1)

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
