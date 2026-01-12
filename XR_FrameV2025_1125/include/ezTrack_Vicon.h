#ifndef __EZ_TRACK_VICON_H__
#define __EZ_TRACK_VICON_H__

#include <iostream>

#include "ezTrack.h"

#include "Vicon/DataStreamClient.h"

#define OUTPUT_FLAG	1/* (1 GroblTranslationとGrobalEulerXYZデータのみ表示) (0 すべて表示)*/
#define EZ_DEGREE (180.0/3.14159)

class ezTracker_Vicon :
    public ezTracker
{
public:
	ezTracker_Vicon(bool use = true);
	~ezTracker_Vicon();
    void init();
	bool open(char* key, bool w);
	void read();
	void write();
	void close();

private:
	void getRot(const double src[], float* roll, float* pitch, float* yaw);

	//- for ViconDataStreamSDK
	ViconDataStreamSDK::CPP::Client MyClient;
};

#endif //__EZ_TRACK_VICON_H__