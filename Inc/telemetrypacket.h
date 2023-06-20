#ifndef __TELEMETRYPACKET_H__
#define __TELEMETRYPACKET_H__

#include <stdint.h>


typedef struct __attribute__((__packed__)){
	uint16_t packet_id;
	uint16_t id;
	uint16_t packet_no;
	uint32_t timestamp_ms;
	uint8_t state;
	uint8_t flags;

	uint8_t vbat_10;	//Vbat*10

	int16_t accX_100;	//Acc*100 [g]
	int16_t accY_100;
	int16_t accZ_100;

	int16_t gyroX_10;	//Gyro*10 [deg/s]
	int16_t gyroY_10;
	int16_t gyroZ_10;

	int16_t tilt_100;	//Tilt*100 [deg]
	float pressure;		//Pressure [Pa]
	int16_t velocity_10;	//Velocity*10 [m/s]
	uint16_t altitude;		//Altitude [m]

	int32_t lat;		//[1e-7 deg]
	int32_t lon;		//[1e-7 deg]
	int32_t alti_gps;	//Height above ellipsoid [- mm]
	uint8_t sats_fix;	//6b - sats + 2b fix
} DataPackageRF_t;


#endif
