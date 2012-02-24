
struct lophilo_data {
	int p0;
};

#define DATA_SIZE PAGE_SIZE // minimal PAGE_SIZE < sizeof(struct lophilo_data)

void lophilo_data_update(void);
int lophilo_data_init(void);
void lophilo_data_cleanup(void);
long unsigned int lophilo_get_data(void);
