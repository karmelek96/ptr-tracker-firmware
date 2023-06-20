#ifndef LORA_H
#define LORA_H

#include "sx126x.h"
#include "trackerHw.h"

int RADIO_setupLoRaTX(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t modParam3, uint8_t modParam4, uint8_t device);
int RADIO_modeLORA(uint32_t frequency, int8_t txpower);
int RADIO_init();
uint16_t RADIO_readIrqStatus();
int RADIO_clearIrqStatus();
int RADIO_sendPacketLoRa(uint8_t *txbuffer, uint16_t size, uint32_t txtimeout);
int RADIO_setRxSingle();
int RADIO_setStandby();
int16_t RADIO_get_rssi(const void* context);
int RADIO_getCRC();
int RADIO_getRxDone();
uint8_t RADIO_getRxPayloadSize();
void RADIO_getRxPayload(uint8_t *buffer);
int RADIO_setBufferBaseAddress(uint8_t tx, uint8_t rx);
uint8_t RADIO_getRandInt(int n);

#endif
