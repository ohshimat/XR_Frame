#include <stdlib.h>
#include <string.h>

#include "ezTrack_Vicon.h"

using namespace std;
using namespace ViconDataStreamSDK::CPP;

/*===========================================================================*/
ezTracker_Vicon::ezTracker_Vicon(bool use)
{
	this->use = use;	
}

ezTracker_Vicon::~ezTracker_Vicon()
{
}

/// <summary>
/// ezTracker_Viconの初期化
/// </summary>
void ezTracker_Vicon::init() 
{
	return;
}

#ifdef PLATFORM_WINDOWS
/// <summary>
/// VICONシステムに接続
/// </summary>
/// <param name="key">Viconシステムのホスト名:Port番号(例 127.0.0.1:801)</param>
/// <param name="w">使用しない</param>
/// <returns></returns>
bool ezTracker_Vicon::open(char* key, bool w)
{
	//- VICONに接続
	if (use) {
		cout << "Connecting to VICON " << key << " ..." << flush;
		while (!MyClient.IsConnected().Connected) {
			// Direct connection
			const Output_Connect ConnectResult = MyClient.Connect(key);
			const bool ok = (ConnectResult.Result == Result::Success);

			if (!ok)
			{
				cout << "Warning - connect failed... ";
				switch (ConnectResult.Result)
				{
				case Result::ClientAlreadyConnected:
					cout << "Client Already Connected" << endl;
					break;
				case Result::InvalidHostName:
					cout << "Invalid Host Name" << endl;
					break;
				case Result::ClientConnectionFailed:
					cout << "Client Connection Failed" << endl;
					break;
				default:
					cout << "Unrecognized Error: " << ConnectResult.Result << endl;
					break;
				}

				return false;
			}

			cout << ".";
#ifdef WIN32
			Sleep(1000);
#else
			sleep(1);
#endif

			// Segment(≒リジッドボディ)情報のみ有効化
			MyClient.EnableSegmentData();

			// 座標系をZ-UPに設定
			// ezTrackDataTへ格納する際に座標変換は行う
			MyClient.SetAxisMapping(Direction::Forward,
				Direction::Left,
				Direction::Up); // Z-up

			// [参考] Y-UPに設定する場合
			//MyClient.SetAxisMapping(Direction::Forward,
			//	Direction::Up,
			//	Direction::Right); // Y-up

			// Clientのバージョン情報を表示
			Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
			cout << "ViconDataStream Client Version: " << _Output_GetVersion.Major << "."
				<< _Output_GetVersion.Minor << "."
				<< _Output_GetVersion.Point << "."
				<< _Output_GetVersion.Revision << endl;

		}

		return true;
	}
	else {
		return true; // useがfalseのときには、ダミーでtrueを返す
	}
}

/// <summary>
/// VICONシステムから最新データを取得して、トラッキング情報を
/// </summary>
void ezTracker_Vicon::read()
{
	if (use)
	{
		//- VICONから最新データを取得
		if (MyClient.GetFrame().Result != Result::Success)
			return;

		n_tracks = 0;
		for (int i = 0; i < _n_tracks; i++) {
			trackarray.data[i].id = -1; //////// IDを-1（リジッドボディが観測されていない）に初期化
		}

		// Subjects (≒ リジッドボディ)の更新
		unsigned int SubjectCount = MyClient.GetSubjectCount().SubjectCount;
		if (OUTPUT_FLAG == 0) {
			cout << "Subjects (" << SubjectCount << "):" << endl;
		}
		for (unsigned int SubjectIndex = 0; SubjectIndex < SubjectCount; ++SubjectIndex)
		{			
			string SubjectName = MyClient.GetSubjectName(SubjectIndex).SubjectName;

			if (OUTPUT_FLAG == 0) {
				cout << "  Subject #" << SubjectIndex << endl;

				// Get the subject name
				//string SubjectName = MyClient.GetSubjectName(SubjectIndex).SubjectName;
				cout << "    Name: " << SubjectName << endl;

				// Get the root segment
				string RootSegment = MyClient.GetSubjectRootSegmentName(SubjectName).SegmentName;
				cout << "    Root Segment: " << RootSegment << endl;
			}

			// Count the number of segments
			unsigned int SegmentCount = MyClient.GetSegmentCount(SubjectName).SegmentCount;
			if (OUTPUT_FLAG == 0) {
				cout << "    Segments (" << SegmentCount << "):" << endl;
			}
			for (unsigned int SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
			{
				if (OUTPUT_FLAG == 0) {
					cout << "      Segment #" << SegmentIndex << endl;
				}

				// Get the segment name
				string SegmentName = MyClient.GetSegmentName(SubjectName, SegmentIndex).SegmentName;
				if (OUTPUT_FLAG == 0) {
					cout << "        Name: " << SegmentName << endl;
				}

				if (OUTPUT_FLAG == 0) {
					// Get the segment parent
					string SegmentParentName = MyClient.GetSegmentParentName(SubjectName, SegmentName).SegmentName;
					cout << "        Parent: " << SegmentParentName << endl;

					// Get the segment's children
					unsigned int ChildCount = MyClient.GetSegmentChildCount(SubjectName, SegmentName).SegmentCount;
					cout << "     Children (" << ChildCount << "):" << endl;
					for (unsigned int ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
					{
						string ChildName = MyClient.GetSegmentChildName(SubjectName, SegmentName, ChildIndex).SegmentName;
						cout << "       " << ChildName << endl;
					}
				}

				// Get the global segment translation
				Output_GetSegmentGlobalTranslation _Output_GetSegmentGlobalTranslation =
					MyClient.GetSegmentGlobalTranslation(SubjectName, SegmentName);
				if (OUTPUT_FLAG == 0) {
					cout << "        Global Translation: (" << _Output_GetSegmentGlobalTranslation.Translation[0] << ", "
						<< _Output_GetSegmentGlobalTranslation.Translation[1] << ", "
						<< _Output_GetSegmentGlobalTranslation.Translation[2] << ") "
						/*<< Adapt(_Output_GetSegmentGlobalTranslation.Occluded)*/ << endl;
				}

				// Get the global segment rotation as a matrix
				Output_GetSegmentGlobalRotationMatrix _Output_GetSegmentGlobalRotationMatrix =
					MyClient.GetSegmentGlobalRotationMatrix(SubjectName, SegmentName);
				if (OUTPUT_FLAG == 0) {
					cout << "        Global Rotation Matrix: (" << _Output_GetSegmentGlobalRotationMatrix.Rotation[0] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[1] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[2] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[3] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[4] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[5] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[6] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[7] << ", "
						<< _Output_GetSegmentGlobalRotationMatrix.Rotation[8] << ") "
						/*<< Adapt(_Output_GetSegmentGlobalRotationMatrix.Occluded)*/ << endl;
				}

				//- trackarrayの更新
				// IDを格納
				n_tracks = SubjectIndex;
				trackarray.data[n_tracks].id = n_tracks;

				// 名前情報を格納
				strcpy(trackarray.data[n_tracks].name, SubjectName.c_str());

				// 位置・姿勢情報を格納
				trackarray.data[n_tracks].x = -_Output_GetSegmentGlobalTranslation.Translation[1] * 0.001; // [mm] -> [m]
				trackarray.data[n_tracks].y =  _Output_GetSegmentGlobalTranslation.Translation[2] * 0.001; // [mm] -> [m]
				trackarray.data[n_tracks].z = -_Output_GetSegmentGlobalTranslation.Translation[0] * 0.001; // [mm] -> [m]				

				getRot(_Output_GetSegmentGlobalRotationMatrix.Rotation,
					&(trackarray.data[n_tracks].roll),
					&(trackarray.data[n_tracks].pitch),
					&(trackarray.data[n_tracks].yaw));
				
				// Consoleに出力
				if (OUTPUT_FLAG == 0) {
					printf("POS: %f\t%f\t%f\n", trackarray.data[n_tracks].x, trackarray.data[n_tracks].y, trackarray.data[n_tracks].z);
					printf("ROT: \n%f\n%f\n%f\n", trackarray.data[n_tracks].roll, trackarray.data[n_tracks].pitch, trackarray.data[n_tracks].yaw);
				}
			}
		}
	};
}

/// <summary>
/// 
/// </summary>
void ezTracker_Vicon::write()
{
	return;
}

/// <summary>
/// VICONシステムからの切断
/// </summary>
void ezTracker_Vicon::close()
{
	if (use && MyClient.IsConnected().Connected)
	{
		cout << " Disconnecting..." << endl;
		MyClient.Disconnect();
	};

	return;
}
#else
bool ezTracker_Vicon::open(char* key, bool w) { return false; }
void ezTracker_Vicon::read() {}
void ezTracker_Vicon::write() {}
void ezTracker_Vicon::close() {}
#endif

/// <summary>
/// 回転行列からRoll, Pitch, Yawに変換
/// </summary>
/// <param name="src"></param>
/// <param name="roll"></param>
/// <param name="pitch"></param>
/// <param name="yaw"></param>
void ezTracker_Vicon::getRot(const double src[],
	float* roll, float* pitch, float* yaw)
{
	float x[3], y[3], z[3];
	float buf[3];
	double m[3][3];

	memcpy((void*)m, (void*)src, sizeof(m));

	x[0] = m[1][1];
	x[1] = -m[2][1];
	x[2] = m[0][1];

	y[0] = -m[1][2];
	y[1] = m[2][2];
	y[2] = -m[0][2];

	z[0] = m[1][0];
	z[1] = -m[2][0];
	z[2] = m[0][0];

	/*---- PITCH ----*/
	*pitch = EZ_DEGREE * asinf(-z[1]); // [-90,90]
	/*---- YAW & ROLL ----*/
	if (-90.0 < *pitch && *pitch < 90.0) {
		*yaw = EZ_DEGREE * atan2f(z[0], z[2]); // [-180,180]
		buf[0] = z[2];
		buf[1] = 0.;
		buf[2] = -z[0];
		float dot = x[0] * buf[0] + x[1] * buf[1] + x[2] * buf[2];
		float norm = sqrtf((x[0] * x[0] + x[1] * x[1] + x[2] * x[2])
			* (buf[0] * buf[0] + buf[1] * buf[1] + buf[2] * buf[2]));
		float c = dot / norm;
		if (c < -1.) c = -1.;
		if (1. < c) c = 1.;
		*roll = EZ_DEGREE * acosf(c);
		if (x[1] < 0.) *roll = -*roll;
	}
	else {
		if (z[1] < 0.) *yaw = EZ_DEGREE * atan2f(y[0], y[2]);
		else            *yaw = EZ_DEGREE * atan2f(-y[0], -y[2]);
		*roll = 0.;
	}
	if (*roll == -0.) *roll = 0.;
	if (*pitch == -0.) *pitch = 0.;
	if (*yaw == -0.) *yaw = 0.;
	return;

}