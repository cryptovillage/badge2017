void ble_init();
void ble_gap_set_adv_std_data_enabled(bool enabled);
void ble_gap_set_adv_custom_data_enabled(bool enabled);
void ble_gap_set_adv_std_data(const char *name, size_t nameLen, const uint8_t *badgeDefinedData);
void ble_gap_set_adv_custom_data(const uint8_t *data, size_t len);
