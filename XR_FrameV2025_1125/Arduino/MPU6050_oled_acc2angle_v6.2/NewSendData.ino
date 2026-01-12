//unsigned int select;

void short2bytes( unsigned short in, unsigned char *out )
{
  out[0] = ( in & 0xff00 ) >> 8;
  out[1] = in & 0x00ff;
}

//const float ACCMAX = 20.0; //POSMAX
const float ACCMAX = 20.0;
const float GYROMAX = 500.0;
const float K = 32767;

void SendData2( vector_t *acc, vector_t *gyro, euler_t *rot, float temp ){
  SendData2( acc->x, acc->y, acc->z, gyro->x, gyro->y, gyro->z,
  rot->roll, rot->pitch, rot->yaw, temp );
}

void SendData2( float ax, float ay, float az, float gx, float gy, float gz,
float roll, float pitch, float yaw, float temp)
{
    unsigned short data[10];
    unsigned char buf[22];
    data[0] = ( ax / ACCMAX + 1.0 ) * K;
    data[1] = ( ay / ACCMAX + 1.0 ) * K;
    data[2] = ( az / ACCMAX + 1.0 ) * K;
    data[3] = ( gx / GYROMAX + 1.0 )* K;
    data[4] = ( gy / GYROMAX + 1.0 )* K;
    data[5] = ( gz / GYROMAX + 1.0 )* K;
    data[6] = ( roll  / 180 + 1.0 ) * K;
    data[7] = ( pitch / 90 + 1.0 ) * K;
    data[8] = ( yaw   / 180 + 1.0 ) * K;
    data[9] = temp  * 100;

    buf[0] = 0xff;
    buf[1] = (unsigned int)select;
    for( int i = 0; i < 10; i++ ){
      short2bytes( data[i], &buf[i*2+2] );
    }
    Serial.write( buf, 22 );
    Serial.flush();
}
