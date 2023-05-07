#include "gps.h"

uint32_t GPS_lat_unsigned = 0;
uint32_t GPS_lon_unsigned = 0;
int32_t GPS_lat = 0;
int32_t GPS_lon = 0;
int32_t GPS_alt = 0;
uint8_t GPS_sat_count = 0;
uint8_t GPS_fix = 0;
char GPS_sentence[NMEA_MAX_SENTENCE_LENGTH];
uint8_t GPS_msg_id = 0;
bool GPS_msg_started = 0;
char txMessageBuffer[64];

uint32_t GPS_parse_lat(char *input) {
    int degrees = (input[0] - '0') * 10 + (input[1] - '0');
    uint32_t minutes = (input[2] - '0') * 100000 + (input[3] - '0') * 10000 + (input[5] - '0') * 1000 + (input[6] - '0') * 100 + (input[7] - '0') * 10 + (input[8] - '0');
    uint32_t result = (degrees * 10000000) + (uint32_t)((double)minutes / 60.0 * 1000);
    return result;
}

uint32_t GPS_parse_lon(char *input) {
    int degrees = (input[1] - '0') * 10 + (input[2] - '0');
    uint32_t minutes = (input[3] - '0') * 100000 + (input[4] - '0') * 10000 + (input[6] - '0') * 1000 + (input[7] - '0') * 100 + (input[8] - '0') * 10 + (input[9] - '0');
    uint32_t result = (degrees * 10000000) + (uint32_t)((double)minutes / 60.0 * 1000);
    return result;
}


int32_t GPS_parse_sign(char *input, uint32_t latlon) {
	int32_t result = latlon;
	if((*input == 83) || (*input == 87)) {
		result = (int32_t)(0 - latlon);
	}
	return result;
}

bool GPS_parse(char c) {
	if(c == '$') {
		GPS_msg_id = 0;
		GPS_msg_started = true;
	}
	else if(c == '\r' && GPS_msg_started) {
		GPS_sentence[GPS_msg_id] = '\0';
		GPS_msg_id = 0;
		GPS_msg_started = false;

		if ((GPS_sentence[2] == 'G') && (GPS_sentence[3] == 'G') && (GPS_sentence[4] == 'A')) {
			char *token = strtok(GPS_sentence, ",");
			int field_count = 0;
			while (token != NULL && field_count < 15) {
				switch (field_count) {
					case 2:
						GPS_lat_unsigned = GPS_parse_lat(token);
						break;
					case 3:
						GPS_lat = GPS_parse_sign(token, GPS_lat_unsigned);
						break;
					case 4:
						GPS_lon_unsigned = GPS_parse_lon(token);
						break;
					case 5:
						GPS_lon = GPS_parse_sign(token, GPS_lon_unsigned);
						break;
					case 6:
						GPS_fix = atoi(token);
						break;
					case 7:
						GPS_sat_count = atoi(token);
						break;
					case 9:
						GPS_alt = atol(token);
						break;
				}
				token = strtok(NULL, ",");
				field_count++;
			}
		}
	}
	else if(GPS_msg_started && GPS_msg_id < NMEA_MAX_SENTENCE_LENGTH) {
		GPS_sentence[GPS_msg_id] = c;
		GPS_msg_id++;
	}
	return 0;
}

bool GPS_getStatus() {
	return GPS_msg_started;
}

uint16_t GPS_crc_calc(char *message)
{
	uint8_t crc = message[0];
	int i = 0;

	for(i; i < strlen(message) ; i++){
		 crc ^= message[i];
	}
	return crc;
}

void GPS_sendCmd(char *message) {
	sprintf(txMessageBuffer, "$%s*%02X\r\n", message, GPS_crc_calc(message));
	HW_send_UART(message, sizeof(message));
}
