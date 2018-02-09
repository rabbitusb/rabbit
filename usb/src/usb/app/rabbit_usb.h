
// APP API
void rabbit_usb_wait_enumerate(void);
void rabbit_usb_cdc_dev_tx(uint32_t cdc_index, uint8_t* buf, uint32_t len);
void rabbit_usb_cdc_dev_rx(uint32_t cdc_index, uint8_t* buf, uint32_t len);

