//This file contains functions specific to the KP-LORA protocol
#include "kplora.h"

DataPackageRF_t KPLORA_selfTelemetryPacket;
DataPackageRF_t KPLORA_relayBuffer[KPLORA_RELAYBUFFER_SIZE];
DataPackageRF_t KPLORA_receivedPacket;
KPLORA_ProtocolTier KPLORA_protocolTier;
int KPLORA_LBTCounter;
uint16_t KPLORA_packetCounter = 0;

void KPLORA_pack_data_standard(int _state, uint8_t _vbat, uint32_t _lat, uint32_t _lon, uint32_t _alt, uint8_t _fix, uint8_t _sats) {
	KPLORA_selfTelemetryPacket.state = _state;
	KPLORA_selfTelemetryPacket.vbat_10 = _vbat;
	KPLORA_selfTelemetryPacket.packet_id = KPLORA_PACKET_ID_FULL; //We specify what type of frame we're sending, in this case the big 48 byte struct
	KPLORA_selfTelemetryPacket.id = TRACKER_ID;
	KPLORA_selfTelemetryPacket.packet_no = KPLORA_packetCounter++;
	KPLORA_selfTelemetryPacket.lat = _lat;
	KPLORA_selfTelemetryPacket.lon = _lon;
	KPLORA_selfTelemetryPacket.alti_gps = _alt * 1000; //To mm
	KPLORA_selfTelemetryPacket.sats_fix = ((_fix & 3) << 6) | (_sats & 0x3F);
}

void KPLORA_send_data_lora() {
	HW_writeLED(1);
	RADIO_sendPacketLoRa(&KPLORA_selfTelemetryPacket, sizeof(DataPackageRF_t), 500);
	HW_writeLED(0);
	RADIO_clearIrqStatus();
	HW_DelayMs(5);
}

void KPLORA_fillRelayBuffer(DataPackageRF_t newData, DataPackageRF_t* buffer) {
	int i;
	for(i = 0;i<KPLORA_RELAYBUFFER_SIZE;i++) {
		if(buffer[i].packet_id == 0) {
			buffer[i] = newData;
			return;
		}
	}
}

void KPLORA_listenForPackets() {
	RADIO_clearIrqStatus();
	HW_DelayMs(5);
	RADIO_setRxSingle();
	HW_DelayMs(5);
	HW_resetTimer3();
	while(HW_getTimer3() < TRACKER_TRANSMISSION_SPACING) {
		if((RADIO_readIrqStatus() & 0x2) == 0x2) {
			if(RADIO_getCRC() == 0) {
				if(RADIO_getRxPayloadSize() == sizeof(KPLORA_selfTelemetryPacket)) {
					RADIO_getRxPayload(&KPLORA_receivedPacket);
					if(KPLORA_receivedPacket.packet_id == KPLORA_PACKET_ID_FULL) {
						KPLORA_fillRelayBuffer(KPLORA_receivedPacket, &KPLORA_relayBuffer);
					}
				}
			}
			RADIO_setBufferBaseAddress(0, 100);
			RADIO_clearIrqStatus();
			HW_DelayMs(5);
		}
	}
}

int KPLORA_listenBeforeTalk() {
	HW_resetTimer3();
	while(HW_getTimer3() < KPLORA_LBT1_TIMEOUT) { //If the interference doesn't stop after 5s, transmit anyway
		HW_DelayMs(KPLORA_MINIMUM_WAIT);
		HW_DelayMs((uint8_t)(RADIO_getRandInt(1)));
		int flag = 1;
		int i;
		for(i=0;i<10;i++) {
			if((RADIO_get_CAD()) || (RADIO_get_rssi() > -90)) {
				flag = 0;
				break;
			}
		}
		if(flag == 1) {
			break;
		}
	}
	return 1;
}

void KPLORA_transmitRelayBuffer() {
	int i;
	for(i=0;i<KPLORA_RELAYBUFFER_SIZE;i++) {
		if(KPLORA_relayBuffer[i].packet_id == KPLORA_PACKET_ID_FULL) {
			KPLORA_relayBuffer[i].packet_id = KPLORA_PACKET_ID_FULL_RETRANSMIT;
			KPLORA_listenBeforeTalk();
			HW_writeLED(1);
			RADIO_sendPacketLoRa(&KPLORA_relayBuffer[i], sizeof(DataPackageRF_t), 500);
			HW_writeLED(0);
			RADIO_clearIrqStatus();
			HW_DelayMs(5);
		}
	}
	for(i=0;i<KPLORA_RELAYBUFFER_SIZE;i++) {
		KPLORA_relayBuffer[i].packet_id = 0;
	}
}
