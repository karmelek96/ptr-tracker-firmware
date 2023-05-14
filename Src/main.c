#include <stdint.h>
#include "trackerHw.h"
#include "gps.h"
#include "telemetryPacket.h"
#include "config.h"
#include "flightState.h"

#define RELAYBUFFER_SIZE 5

enum FlightState state;
volatile char uartReceivedByte = '\0';
DataPackageRF_t selfTelemetryPacket;
DataPackageRF_t relayBuffer[RELAYBUFFER_SIZE];
DataPackageRF_t receivedPacket;
uint8_t vbat;

void read_analogSensors(uint8_t *voltage) {
	*voltage = (uint32_t)((HW_readADC(3)*1000 / 4095 * 33 * 3) / 1000); //Returns battery voltage * 10
}

void pack_data() {
	selfTelemetryPacket.state = state;
	read_analogSensors(&vbat);
	selfTelemetryPacket.vbat_10 = vbat;
	selfTelemetryPacket.packet_id = 0x00AA; //We specify what type of frame we're sending, in this case the big 48 byte struct
	selfTelemetryPacket.lat = GPS_lat;
	selfTelemetryPacket.lon = GPS_lon;
	selfTelemetryPacket.alti_gps = GPS_alt * 1000; //To mm
	selfTelemetryPacket.sats_fix = ((GPS_fix & 3) << 6) | (GPS_sat_count & 0x3F);
}

void send_data_lora(uint8_t* data) {
	HW_writeLED(1);
	RADIO_sendPacketLoRa(data, sizeof(DataPackageRF_t), 500);
	HW_writeLED(0);
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

void fillRelayBuffer(DataPackageRF_t newData, DataPackageRF_t* buffer) {
	int i;
	for(i = 0;i<RELAYBUFFER_SIZE;i++) {
		if(buffer[i].packet_id == 0) {
			buffer[i] = newData;
			buffer[i].packet_id++;
		}
	}
}

void listenForPackets() {
	if(HW_readDIO1() == 1) { //We received something
		if(RADIO_getCRC() == 0) {
			if(RADIO_getRxPayloadSize() == sizeof(selfTelemetryPacket)) {
				RADIO_getRxPayload(&receivedPacket);
				if(receivedPacket.packet_id == 0x00AA) {
					fillRelayBuffer(receivedPacket, &relayBuffer);
				}
			}
		}
		RADIO_clearIrqStatus();
	}
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
	int i;
	while(GPS_sat_count < 5) {
		blink_GPS_startup();
	}
	state = OPERATION;
	HW_StartTimer3();
	while(state = OPERATION) {
		pack_data();
		send_data_lora(&selfTelemetryPacket);
		HW_DelayMs(200); //This is very important. Idk why but below 200 the sx1262 stops working completely
		RADIO_setRx();
		HW_DelayMs(5);
		while(HW_getTimer3() < TRACKER_TRANSMISSION_SPACING) {
			//Wait
			HW_DelayMs(10);
		}
		RADIO_setStandby();
		HW_DelayMs(5);
		RADIO_setRxContinuous();
		HW_DelayMs(5);
		HW_resetTimer3();
		while(HW_getTimer3() < 5000) { //If the interference doesnt stop after 5s, transmit anyway
			if(RADIO_get_rssi(0) < -90) {
				break;
			}
		}
		HW_resetTimer3();
	}
}

void USART1_IRQHandler(void) {
	if(USART1->ISR & USART_ISR_RXNE_RXFNE) {
		uartReceivedByte = USART1->RDR;
		GPS_parse(uartReceivedByte);
	}
}
