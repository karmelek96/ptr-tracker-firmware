#include "sx126x_hal.h"
sx126x_hal_status_t sx126x_hal_write(const void* context, const uint8_t* command, const uint16_t command_length, const uint8_t* data, const uint16_t data_length) {
	uint8_t tx_buffer[command_length + data_length];
	memcpy(tx_buffer, command, command_length);
	memcpy(tx_buffer + command_length, data, data_length);
	HW_write_LORACS(0);
	HW_SPI_WriteBuffer(tx_buffer, (command_length + data_length));
	HW_write_LORACS(1);
	return 0;
}

sx126x_hal_status_t sx126x_hal_read(const void* context, const uint8_t* command, const uint16_t command_length, uint8_t* data, const uint16_t data_length ) {
	uint8_t tx_buffer[command_length];
	memcpy(tx_buffer, command, command_length);
	HW_write_LORACS(0);
	HW_SPI_WriteBuffer(tx_buffer, command_length);
	HW_SPI_ReadBuffer(data, data_length);
	HW_write_LORACS(1);
	return 0;
}

sx126x_hal_status_t sx126x_hal_reset(const void* context) {
	return 0;
}

sx126x_hal_status_t sx126x_hal_wakeup(const void* context) {
	HW_write_LORACS(0);
	HW_DelayMs(2);
	HW_write_LORACS(1);
	HW_DelayMs(2);
	return 0;
}
