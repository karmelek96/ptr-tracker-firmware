#ifndef __KPLORA_H
#define __KPLORA_H

#define KPLORA_RELAYBUFFER_SIZE 5
#define KPLORA_LBT1_TIMEOUT 5000
#define KPLORA_PACKET_ID_FULL 0x00AA
#define KPLORA_PACKET_ID_FULL_RETRANSMIT 0x00AB

#include "telemetrypacket.h"
#include "lora.h"
#include "trackerHw.h"
#include "config.h"

typedef enum KPLORA_ProtocolTier {
	TIER_1 = 1,
	TIER_2 = 2,
	TIER_3 = 3
}KPLORA_ProtocolTier;

extern int KPLORA_LBTCounter;

void KPLORA_pack_data_standard(int _state, uint8_t _vbat, uint32_t _lat, uint32_t _lon, uint32_t _alt, uint8_t _fix, uint8_t _sats);
void KPLORA_send_data_lora();
void KPLORA_fillRelayBuffer(DataPackageRF_t newData, DataPackageRF_t* buffer);
void KPLORA_listenForPackets();
int KPLORA_listenBeforeTalk();
void KPLORA_transmitRelayBuffer();

#endif
