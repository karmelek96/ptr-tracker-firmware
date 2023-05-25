#include "lora.h"

int RADIO_setupLoRaTX(uint32_t frequency, int32_t offset, uint8_t modParam1,
	uint8_t modParam2, uint8_t modParam3, uint8_t modParam4, uint8_t device) {

	sx126x_status_t status = sx126x_set_standby(0, SX126X_STANDBY_CFG_RC);


	if(status == SX126X_STATUS_OK)
		status = sx126x_set_reg_mode(0, SX126X_REG_MODE_DCDC);

	sx126x_pa_cfg_params_t sx126x_pa_cfg_params_d;
	sx126x_pa_cfg_params_d.pa_duty_cycle = 0x04;
	sx126x_pa_cfg_params_d.hp_max 	  	 = 0x07;
	sx126x_pa_cfg_params_d.device_sel 	 = 0x00;
	sx126x_pa_cfg_params_d.pa_lut 	  	 = 0x01;
	if(status == SX126X_STATUS_OK)
			status = sx126x_set_pa_cfg(0, &sx126x_pa_cfg_params_d);

	if(status == SX126X_STATUS_OK)
			//status = sx126x_set_dio3_as_tcxo_ctrl(0, SX126X_TCXO_CTRL_3_3V, 100);
	HW_DelayMs(5);

	if(status == SX126X_STATUS_OK)
			//status = sx126x_cal(0, SX126X_CAL_ALL);	//is required after setting TCXO

	if(status == SX126X_STATUS_OK)
			//status = sx126x_cal_img(0, frequency);

	if(status == SX126X_STATUS_OK)
			status = sx126x_set_dio2_as_rf_sw_ctrl(0, true);

	if(status == SX126X_STATUS_OK)
			status = sx126x_set_pkt_type(0, SX126X_PKT_TYPE_LORA);

	if(status == SX126X_STATUS_OK)
			status = sx126x_set_rf_freq(0, frequency);

	sx126x_mod_params_lora_t sx126x_mod_params_lora_d;
	sx126x_mod_params_lora_d.bw   = modParam2;
	sx126x_mod_params_lora_d.cr   = modParam3;
	sx126x_mod_params_lora_d.ldro = modParam4;
	sx126x_mod_params_lora_d.sf   = modParam1;

	if(status == SX126X_STATUS_OK)
			status = sx126x_set_lora_mod_params(0, &sx126x_mod_params_lora_d);
	if(status == SX126X_STATUS_OK)
			status = sx126x_set_buffer_base_address(0, 0, 100);

	sx126x_set_dio_irq_params(0, SX126X_IRQ_RX_DONE, SX126X_IRQ_RX_DONE, 0, 0); //This makes DIO1 go high when rx is done

	sx126x_pkt_params_lora_t sx126x_pkt_params_lora_d;
	sx126x_pkt_params_lora_d.crc_is_on 				= true;
	sx126x_pkt_params_lora_d.header_type 			= SX126X_LORA_PKT_EXPLICIT;
	sx126x_pkt_params_lora_d.invert_iq_is_on 		= false;
	sx126x_pkt_params_lora_d.pld_len_in_bytes 		= 255;
	sx126x_pkt_params_lora_d.preamble_len_in_symb 	= 8;
	if(status == SX126X_STATUS_OK)
			status = sx126x_set_lora_pkt_params(0, &sx126x_pkt_params_lora_d);

	if(status == SX126X_STATUS_OK)
			status = sx126x_set_dio_irq_params(0, SX126X_IRQ_ALL,
								(SX126X_IRQ_TX_DONE + SX126X_IRQ_TIMEOUT+ SX126X_IRQ_RX_DONE),
								 SX126X_IRQ_NONE, SX126X_IRQ_NONE);

	if(status == SX126X_STATUS_OK)
		return 0;

	return 1;
}


int RADIO_modeLORA(uint32_t frequency, int8_t txpower) {
	sx126x_status_t status = sx126x_clear_irq_status(0, SX126X_IRQ_ALL);

	if(status == SX126X_STATUS_OK){
		status = (sx126x_status_t)RADIO_setupLoRaTX(frequency, 0, SX126X_LORA_SF8, SX126X_LORA_BW_125, SX126X_LORA_CR_4_5,
							0x02, 0x02);
	}

	if(status == SX126X_STATUS_OK){
		status = sx126x_set_tx_params(0, txpower, SX126X_RAMP_10_US);
	}

	if(status == SX126X_STATUS_OK)
		return 0;

	return 1;
}

int RADIO_init()
{
	//SX126X_initIO();
	sx126x_set_trimming_capacitor_values(0, 0, 0);
	sx126x_set_dio2_as_rf_sw_ctrl(0, true);
	HW_DelayMs(20);

	RADIO_modeLORA(433000000UL, 0);

	return 0;
}




uint16_t RADIO_readIrqStatus(){
	uint16_t res = 0;
	sx126x_get_irq_status(0, (sx126x_irq_mask_t*) &res );

	return res;
}

int RADIO_clearIrqStatus() {
	return sx126x_clear_irq_status(0, 0b1111111111);
}

int RADIO_sendPacketLoRa(uint8_t *txbuffer, uint16_t size, uint32_t txtimeout) {
	if ((size == 0) || (size > 256)) {
		return 1;
	}

	sx126x_set_standby(0, SX126X_STANDBY_CFG_RC);

	sx126x_write_buffer(0, 0, txbuffer,	size);

	sx126x_pkt_params_lora_t sx126x_pkt_params_lora_d;
	sx126x_pkt_params_lora_d.crc_is_on 				= true;
	sx126x_pkt_params_lora_d.header_type 			= SX126X_LORA_PKT_EXPLICIT;
	sx126x_pkt_params_lora_d.invert_iq_is_on 		= false;
	sx126x_pkt_params_lora_d.pld_len_in_bytes 		= size;
	sx126x_pkt_params_lora_d.preamble_len_in_symb 	= 8;

	sx126x_set_lora_pkt_params(0, &sx126x_pkt_params_lora_d);

	sx126x_set_tx(0, txtimeout);	//this starts the TX

	if(txtimeout){
		volatile uint16_t timeout = 10000;
		while ((!(RADIO_readIrqStatus() & SX126X_IRQ_TX_DONE)) && timeout){	//Wait for TX done
			HW_DelayMs(1);
			timeout--;
		}

		if (RADIO_readIrqStatus() & SX126X_IRQ_TIMEOUT) {        //check for timeout
			return 1;
		} else {
			return 0;
		}
	}

	return 0;
}

int RADIO_setRxSingle() { //This mode uses more power and causes significant heating of the chip. Used only for RSSI
	sx126x_set_standby(0, SX126X_STANDBY_CFG_RC);
	HW_DelayMs(5);
	sx126x_set_rx(0, 0);
	return 0;
}

int RADIO_setRxSingleDutyCycle() {
	sx126x_set_standby(0, SX126X_STANDBY_CFG_RC);
	HW_DelayMs(5);
	//sx126x_set_rx_duty_cycle(0, 20, 100);
	sx126x_set_rx(0, 1000);
	return 0;
}

int RADIO_setStandby() {
	sx126x_set_standby(0, SX126X_STANDBY_CFG_RC);
	return 0;
}

int16_t RADIO_get_rssi(const void* context) {
    sx126x_status_t status = SX126X_STATUS_ERROR;
    int16_t rssi_in_dbm = 0;

    while (1) {
        status = sx126x_get_rssi_inst(context, &rssi_in_dbm);
        if (status == SX126X_STATUS_OK) {
            return rssi_in_dbm;
        }
    }
    HW_DelayMs(5);
}

int RADIO_getCRC() {
	return (RADIO_readIrqStatus() & SX126X_IRQ_CRC_ERROR);
}

int RADIO_getRxDone() {
	return (RADIO_readIrqStatus() & SX126X_IRQ_RX_DONE);
}

uint8_t RADIO_getRxPayloadSize() {
	sx126x_rx_buffer_status_t sx126x_rx_buffer_status_d;
	sx126x_get_rx_buffer_status(0, &sx126x_rx_buffer_status_d);
	return sx126x_rx_buffer_status_d.pld_len_in_bytes;
}

void RADIO_getRxPayload(uint8_t *buffer) {
	sx126x_rx_buffer_status_t sx126x_rx_buffer_status_d;
	sx126x_get_rx_buffer_status(0, &sx126x_rx_buffer_status_d);
	sx126x_read_buffer(0, sx126x_rx_buffer_status_d.buffer_start_pointer, buffer, sx126x_rx_buffer_status_d.pld_len_in_bytes);
}

int RADIO_setBufferBaseAddress(uint8_t tx, uint8_t rx) {
	return sx126x_set_buffer_base_address(0, tx, rx);
}
