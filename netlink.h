#ifdef __KERNEL__ 
  #include <net/genetlink.h>
  int lophilo_notify(enum lophilo_sources source, int value);
  int lophilo_netlink_init(void);
  void lophilo_netlink_cleanup(void);
#else
  #include <netlink/netlink.h>
  #include <netlink/attr.h>
  #include <netlink/genl/genl.h>
  #include <netlink/genl/ctrl.h>
#endif

#define LOPHILO_FAMILY_NAME "LOPHILO"
#define LOPHILO_GROUP 0x00
#define LOPHILO_VERSION_NR 0x01
//#define LOPHILO_GENL_MCAST_GROUP_NAME "LOPHILO_MCAST"

enum {
	LOPHILO_C_UNSPEC,
	LOPHILO_C_REGISTER,
	LOPHILO_C_UPDATE,
	__LOPHILO_C_MAX,
};
#define LOPHILO_C_MAX (__LOPHILO_C_MAX - 1)

enum {
	LOPHILO_R_UNSPEC,
	LOPHILO_R_TYPE,
	LOPHILO_R_PERIOD_MS,
	LOPHILO_R_SOURCE,
        __LOPHILO_R_MAX,
};
#define LOPHILO_R_MAX (__LOPHILO_R_MAX - 1)

enum lophilo_sources_types {
	LOPHILO_TYPE_ADC,
};

enum lophilo_sources {
	LOPHILO_PIN_XA0,
};


static struct nla_policy lophilo_register_policy[LOPHILO_R_MAX + 1] = {
	[LOPHILO_R_TYPE] = { .type = NLA_U32 },
	[LOPHILO_R_PERIOD_MS] = { .type = NLA_U32 },
	[LOPHILO_R_SOURCE] = { .type = NLA_U32 },
};

enum {
	LOPHILO_U_UNSPEC,
	LOPHILO_U_SOURCE,
	LOPHILO_U_VALUE,
        __LOPHILO_U_MAX,
};
#define LOPHILO_U_MAX (__LOPHILO_U_MAX - 1)

static struct nla_policy lophilo_notify_policy[LOPHILO_U_MAX + 1] = {
	[LOPHILO_U_SOURCE] = { .type = NLA_U32 },
	[LOPHILO_U_VALUE] = { .type = NLA_U32 },
};

