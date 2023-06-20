#ifndef _GPS_H
#define _GPS_H

#include<stdbool.h>
#include<stdint.h>
#include<stddef.h>

#define NMEA_MAX_SENTENCE_LENGTH 82
#define PMTK_SET_GPGGA "PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"
#define PMTK_SET_FAST_UPDATE "PMTK220,100"
#define PMTK_RESET "PMTK104"

extern int32_t GPS_lat;
extern int32_t GPS_lon;
extern uint32_t GPS_lat_unsigned;
extern uint32_t GPS_lon_unsigned;
extern int32_t GPS_alt;
extern uint8_t GPS_sat_count;
extern uint8_t GPS_fix;
extern char GPS_sentence[NMEA_MAX_SENTENCE_LENGTH];
extern uint8_t GPS_msg_id;
extern bool GPS_msg_started;
extern char GPS_txMessageBuffer[64];

uint32_t GPS_parse_lat(char *input);
uint32_t GPS_parse_lon(char *input);
int32_t GPS_parse_sign(char *input, uint32_t latlon);
bool GPS_parse(char c);
bool GPS_getStatus();
uint16_t GPS_crc_calc(char *message);
void GPS_sendCmd(char *message);

#endif
