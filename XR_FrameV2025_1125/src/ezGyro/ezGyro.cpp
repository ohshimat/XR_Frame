#define _CRT_SECURE_NO_WARNINGS
#include "calc.h"
#include "stdio.h"
#include "ezGyro/ezGyro.h"
#include "ezGyro/MPU6050.h"

WitMotionT* ConnectWitMotion( int port, int baudrate )
{
	WitMotionT *wmdata = new WitMotionT();

	wmdata->jy = new CJY901();

	wmdata->jy->open(port, baudrate);

	return wmdata;
}

void convert(WitMotionT *wmdata, CJY901 *jy)
{
	wmdata->clock.year = jy->stcTime.ucYear;
	wmdata->clock.month = jy->stcTime.ucMonth;
	wmdata->clock.day  = jy->stcTime.ucDay,
	wmdata->clock.hour = jy->stcTime.ucHour,
	wmdata->clock.minute = jy->stcTime.ucMinute,
	wmdata->clock.second = 
		(float)jy->stcTime.ucSecond 
		+ (float)jy->stcTime.usMiliSecond / 1000;

	wmdata->status[0] =	jy->stcDStatus.sDStatus[0],
	wmdata->status[1] = jy->stcDStatus.sDStatus[1],
	wmdata->status[2] = jy->stcDStatus.sDStatus[2],
	wmdata->status[3] = jy->stcDStatus.sDStatus[3];

	// ACCELEROMETER [m/s2]
	const float g = 9.80665;
	wmdata->acc.x =  g * (float)jy->stcAcc.a[1] / 32768 * 16;
	wmdata->acc.y = -g * (float)jy->stcAcc.a[2] / 32768 * 16;
	wmdata->acc.z =  g * (float)jy->stcAcc.a[0] / 32768 * 16;
	// GYROSCOPE [degree/sec]
	wmdata->gyro.roll =  -(float)jy->stcGyro.w[0] / 32768 * 2000;
	wmdata->gyro.pitch = -(float)jy->stcGyro.w[1] / 32768 * 2000;
	wmdata->gyro.yaw  =   (float)jy->stcGyro.w[2] / 32768 * 2000;
	// ANGLER ORIENTATION [degree]
	wmdata->angle.roll = -(float)jy->stcAngle.Angle[0] / 32768 * 180;
	wmdata->angle.pitch= -(float)jy->stcAngle.Angle[1] / 32768 * 180;
	wmdata->angle.yaw  =  (float)jy->stcAngle.Angle[2] / 32768 * 180;
	// MAGNETIC FIELDS [micro Tesra]
	wmdata->mag.x = -(float)jy->stcMag.h[1] / 300.0 * 2;
	wmdata->mag.y =  (float)jy->stcMag.h[2] / 300.0 * 2;
	wmdata->mag.z = -(float)jy->stcMag.h[0] / 300.0 * 2;
	// ATMOSPHERIC PARAMETERS
	wmdata->atmos.temp = (float)jy->stcGyro.T * 0.01; //[degree]
	wmdata->atmos.pressure = (float)jy->stcPress.lPressure * 0.01; //[hPa]
	wmdata->atmos.altitude = (float)jy->stcPress.lAltitude / 100; //[m]

	// GLOBAL POSITIONING
	wmdata->gps.latitude =
		(float)jy->stcLonLat.lLon / 10000000
		+ (float)(jy->stcLonLat.lLon % 10000000) / 1e5; //[degree]
	wmdata->gps.longitude =
		(float)jy->stcLonLat.lLat / 10000000
		+ (float)(jy->stcLonLat.lLat % 10000000) / 1e5; //[degree]
	wmdata->gps.altitude = (float)jy->stcGPSV.sGPSHeight / 10; //[m]
	wmdata->gps.azimuth = (float)jy->stcGPSV.sGPSYaw / 10; //[degree]
	wmdata->gps.velocity = (float)jy->stcGPSV.lGPSVelocity / 1000; //[m/s]

	wmdata->gps.n_sat = jy->dop.nsat;
	wmdata->gps.pdop = (float)jy->dop.pdop / 100;
	wmdata->gps.hdop = (float)jy->dop.hdop / 100;
	wmdata->gps.vdop = (float)jy->dop.vdop / 100;

	wmdata->quatern.q0 = (float)jy->quatern.q0 / 32768;
	wmdata->quatern.q1 = (float)jy->quatern.q1 / 32768;
	wmdata->quatern.q2 = (float)jy->quatern.q2 / 32768;
	wmdata->quatern.q3 = (float)jy->quatern.q3 / 32768;
}

void PrintWitMotion(WitMotionT *wmdata)
{
	times_t *clock = &wmdata->clock;
	atmos_t *atmos = &wmdata->atmos;
	euler_t *angle = &wmdata->angle;
	euler_t *gyro = &wmdata->gyro;
	vector_t *mag = &wmdata->mag;
	vector_t *acc = &wmdata->acc;
	gps_t *gps = &wmdata->gps;
	quatern_t *quat = &wmdata->quatern;

	printf("\n>DATE: 20%02d/%02d/%02d %02d:%02d:%06.3f\n",
		clock->year, clock->month, clock->day,
		clock->hour, clock->minute, clock->second);

	printf("\n>ATMOSPHERE:\n");
	printf("    PRES:\t%8.3f[hPa]\n", atmos->pressure );
	printf("    TEMP:\t%8.3f[degree]\n",atmos->temp);
	printf("    ALT:\t%8.3f[m]\n", atmos->altitude);

	printf("\n> STATUS: %lx %lx %lx %lx\n",
		wmdata->status[0], wmdata->status[1],
		wmdata->status[2], wmdata->status[3]);

	printf("\n> ANGLE[degree]:\n");
	printf("\tRoll:\t%8.3f\n", angle->roll);
	printf("\tPitch:\t%8.3f\n", angle->pitch);
	printf("\tYaw:\t%8.3f\n", angle->yaw);

	printf("\n> QUATERNION:%8.4f %8.4f %8.4f %8.4f\n",
		quat->q0, quat->q1, quat->q2, quat->q3);

	printf("\n> GYRO[degree/sec]:\tACC[m/s2]:\tMAG[uT]:\n");
	printf("\tRX:%7.2f\t", gyro->pitch);
	printf("X:%7.2f\t", acc->x);
	printf("X:%7.2f\n", mag->x);
	printf("\tRY:%7.2f\t", gyro->yaw);
	printf("Y:%7.2f\t", acc->y);
	printf("Y:%7.2f\n", mag->y);


	printf("\tRZ:%7.2f\t", gyro->roll);
	printf("Z:%7.2f\t", acc->z);
	printf("Z:%7.2f\n", mag->z);

	printf("\n> GPS\n");
	printf("\tLONGITUDE:%7.2f\n", gps->longitude);
	printf("\tLATITUDE:%7.2f\n", gps->latitude);
	printf("\tAZIMUTH:%7.2f\n", gps->azimuth);
	printf("\tALTITUDE:%7.2f\n", gps->altitude);
	printf("\tVELOCITY:%7.2f\n", gps->velocity);
	printf("\tSATELLITES: %d\n", gps->n_sat);
	printf("\tPDOP:%5.2 HDOP:%5.2 VDOP:%5.2\n", 
		gps->pdop, gps->vdop, gps->hdop);
}

bool ReadWitMotion( WitMotionT *wmdata )
{
	wmdata->jy->read();
	convert( wmdata, wmdata->jy );
	return true;
}

bool DisconnectWitMotion(WitMotionT *wmdata)
{
	return true;
}
