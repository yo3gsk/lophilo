#ifndef __LOPHILO_H__
#define __LOPHILO_H__
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
	LOPHILO_TIME_USEC,
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
#endif
