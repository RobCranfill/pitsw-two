

void test_accel(void);
void test_accel_1(void);
void test_accel_2(void);
void test_accel_3(void);
void test_accel_4(void);
void test_accel_5(void);

bool detect_wand_movement(void);
void show_accel(void);
void show_accel_tabbed(void);

void init_accel(void);  // this is the public one that should be called
void lis3dh_init(void);
void lis3dh_calc_value(uint16_t raw_value, float *final_value);
void lis3dh_read_data(uint8_t reg, float *final_value);
float lis3dh_read_y_accel(void);
