#include "trackerHw.h"
#include "gps.h"
#include "config.h"
#include "flightState.h"
#include "kplora.h"

enum FlightState state;
volatile char uartReceivedByte = '\0';
uint8_t vbat;
uint8_t A_flag = 0;
uint8_t channel = 0;
uint8_t txPower = TRACKER_TXPOWER_LOW;

void read_analogSensors(uint8_t *voltage) {
	*voltage = (uint32_t)((HW_readADC(3)*1000 / 4095 * 33 * 3) / 1000); //Returns battery voltage * 10
}

uint8_t getSwitchPosition() {
	uint32_t adcValue = 0;
	int i;
	for(i=0;i<10;i++) {
		adcValue = adcValue + HW_readADC(15);
	}
	adcValue = adcValue / 10;

	if(adcValue < 2000)
		return 0;
	else if(adcValue > 2000 && adcValue < 2100)
		return 1;
	else if(adcValue > 3400 && adcValue < 3450)
		return 2;
	else if(adcValue > 3500 && adcValue < 3550)
		return 3;
	else if(adcValue > 3700 && adcValue < 3740)
		return 4;
	else if(adcValue > 3740 && adcValue < 3800)
		return 5;
	else if(adcValue > 3700 && adcValue < 3850)
		return 6;
	else if(adcValue > 3850 && adcValue < 3870)
		return 7;
	else
		return 0;
}

void setupTracker(uint8_t params) {
	channel = params & 0b11;
	A_flag = params & 0b100;

	if(HW_readJMP()) {
		txPower = TRACKER_TXPOWER_HIGH;
	}
}

void blink_GPS_startup() {
	HW_writeLED(1);
	HW_DelayMs(500);
	HW_writeLED(0);
	HW_DelayMs(500);
	for(int i=0;i<GPS_sat_count;i++) {
		HW_writeLED(1);
		HW_DelayMs(50);
		HW_writeLED(0);
		HW_DelayMs(300);
	}
	HW_DelayMs(1000);
}

int main(void)
{
	state = STARTUP;
	HW_trackerHwInit();
	setupTracker(getSwitchPosition());
	RADIO_init();
	switch(channel) {
	case 0:
		RADIO_modeLORA(TRACKER_FREQUENCY_0, txPower);
		break;

	case 1:
		RADIO_modeLORA(TRACKER_FREQUENCY_1, txPower);
		break;

	case 2:
		RADIO_modeLORA(TRACKER_FREQUENCY_2, txPower);
		break;

	case 3:
		RADIO_modeLORA(TRACKER_FREQUENCY_3, txPower);
		break;
	}
    __disable_irq();
    GPS_sendCmd(PMTK_RESET);
	GPS_sendCmd(PMTK_SET_GPGGA);
	GPS_sendCmd(PMTK_SET_FAST_UPDATE);
	__enable_irq();
	state = WAIT_FOR_FIX;
	while(GPS_sat_count < 5) {
		blink_GPS_startup();
	}
	state = OPERATION;
	HW_StartTimer3();
	while(state == OPERATION) {
		read_analogSensors(&vbat);
		KPLORA_pack_data_standard(state, vbat, GPS_lat, GPS_lon, GPS_alt, GPS_fix, GPS_sat_count);
		KPLORA_listenBeforeTalk();
		KPLORA_send_data_lora();
		if(A_flag) {
			KPLORA_listenForPackets();
			KPLORA_transmitRelayBuffer();
		}
		else {
			HW_DelayMs(TRACKER_TRANSMISSION_SPACING);
		}
	}
}

void USART1_IRQHandler(void) {
	if(USART1->ISR & USART_ISR_RXNE_RXFNE) {
		uartReceivedByte = USART1->RDR;
		GPS_parse(uartReceivedByte);
	}
}
