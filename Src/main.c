#include "trackerHw.h"
#include "gps.h"
#include "config.h"
#include "flightState.h"
#include "kplora.h"

enum FlightState state;
volatile char uartReceivedByte = '\0';
uint8_t vbat;

void read_analogSensors(uint8_t *voltage) {
	*voltage = (uint32_t)((HW_readADC(3)*1000 / 4095 * 33 * 3) / 1000); //Returns battery voltage * 10
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
	RADIO_init();
	RADIO_modeLORA(TRACKER_FREQUENCY_0, TRACKER_TXPOWER_LOW);
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
		KPLORA_listenForPackets();
		KPLORA_transmitRelayBuffer();
	}
}

void USART1_IRQHandler(void) {
	if(USART1->ISR & USART_ISR_RXNE_RXFNE) {
		uartReceivedByte = USART1->RDR;
		GPS_parse(uartReceivedByte);
	}
}
