#include "lophilo.h"

typedef void (*update_function_t)(int source, int value);

int lophilo_init(update_function_t uf);
int lophilo_register(enum lophilo_sources_types type, enum lophilo_sources source, int period);
int lophilo_loop(void);

