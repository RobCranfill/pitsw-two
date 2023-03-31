void lis3dh_init(void);
bool get_wand_movement(void);
void init_accel(i2c_inst_t *i2c);
void lis3dh_calc_value(uint16_t raw_value, float *final_value, bool isAccel);
void lis3dh_read_data(uint8_t reg, float *final_value, bool IsAccel);
void show_accel();

