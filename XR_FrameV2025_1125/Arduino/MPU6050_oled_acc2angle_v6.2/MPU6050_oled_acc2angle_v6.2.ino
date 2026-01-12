//#define USE_OLED
#if 0
bool console = false;
bool plotter = false;
#else
bool console = true;
bool plotter = true;
#endif

unsigned int baudrate = 57600;

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
Adafruit_MPU6050 mpu;

#if 0
const int N_MPUS = 1;
const int ado[N_MPUS] = {0};
#else
const int N_MPUS = 2; //1
const int ado[N_MPUS] = {2, 4}; //{0}
#endif

#ifdef USE_OLED
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
const bool use_oled = true;
#else
const bool use_oled = false;
#endif

const float DEGREE = 180.0 / 3.141592;
const float RADIAN = 3.141592 / 180.0;
typedef float matrix_t[16];

typedef struct {
  float x, y, z;
} vector_t;
typedef struct {
  float roll, pitch, yaw;
} euler_t;
typedef struct _quat_t{
  float x, y, z, w;
} quat_t;

void delay_500( void ) {
  delay( 100 );
}

vector_t acc[N_MPUS];
vector_t a0[N_MPUS];
euler_t rot0[N_MPUS];
vector_t gyro[N_MPUS];
vector_t g0[N_MPUS];

matrix_t mat[N_MPUS];
vector_t vel[N_MPUS];
vector_t pos[N_MPUS];
euler_t rot[N_MPUS];

//--------------------------------
void setup() {
  Serial.begin( baudrate );
  while (!Serial) delay(10);

  bool status;
  status = mpu.begin();
  if ( console && !plotter ) {
    testmatrix();

    Serial.println(F("MPU6050 OLED demo"));
    if (!status) {
      Serial.println(F("Sensor init failed"));
      while (1)
        yield(); // do nothing, idling
    }
    Serial.println(F("Found a MPU-6050 sensor"));
  }
  for ( int i = 0; i < N_MPUS; i++ ) {
    if ( ado[i] > 0 ) {
      pinMode( ado[i], OUTPUT );
      digitalWrite( ado[i], LOW );
    }
    setAccRange( MPU6050_RANGE_2_G);
    delay_500();
    setGyroRange( MPU6050_RANGE_500_DEG);
    delay_500();
    setFilterBandwidth( MPU6050_BAND_21_HZ);
    delay_500();
    setCycleRate( MPU6050_CYCLE_40_HZ);
    delay_500();
    if ( ado[i] > 0 ) digitalWrite( ado[i], HIGH );

    VectorSet( &acc[i], 0.0, 1.0, 0.0 );
    VectorSet( &gyro[i], 0.0, 0.0, 0.0 );
    VectorSet( &pos[i], 0.0, 0.0, 0.0 );
    MatrixIdent( mat[i] );
    EulerSet( &rot[i], 0.0, 0.0, 0.0 );
  }

#ifdef USE_OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    if ( console ) Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  delay_500(); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
#endif

  if ( console && plotter ) {
    Serial.print( "ROLL" ); Serial.print(",");
    Serial.print( "PITCH" ); Serial.print(",");
    Serial.println( "YAW");
  }

#ifdef USE_JOYSTICK
#endif

}

unsigned int select = 0;

void loop() {
  static unsigned long tstart = 0, t0, t1;
  float dt;
  static int count[N_MPUS] = {0, 0};
  sensors_event_t a, g, temp;
  static float av0[N_MPUS] = {0.0, 0.0};

  float av, ax, ay, az; // acceleration

  float roll = 0.0, pitch = 0.0, yaw = 0.0; // rotation
  float gx, gy, gz; // gyro, angler velocity
  float k;

  t0 = t1;
  t1 = millis();
  dt = 0.001 * (t1 - t0);

  for ( select = 0; select < N_MPUS; select++ ) {
  //for ( select = N_MPUS-1; select >= 0; select-- ) {    
    if ( ado[select] > 0 ) digitalWrite( ado[select], LOW);
    mpu.getEvent(&a, &g, &temp);
    if ( ado[select] > 0 ) digitalWrite( ado[select], HIGH);
    //-------- extract acceleration
    const float c1 = 0.75;
    ax = - a.acceleration.y;
    acc[select].x *= c1;
    acc[select].x += (1.0 - c1) * ax;
    ay =   a.acceleration.z;
    acc[select].y *= c1;
    acc[select].y += (1.0 - c1) * ay;
    az = - a.acceleration.x;
    acc[select].z *= c1;
    acc[select].z += (1.0 - c1) * az;  
    //-------- extract gyro
    const float c2 = 0.5;
    gx = - g.gyro.y * DEGREE;
    gyro[select].x *= c2;
    gyro[select].x += (1.0 - c2 ) * gx;    
    gyro[select].y = gy =   g.gyro.z * DEGREE;
    gyro[select].y *= c2;
    gyro[select].y += (1.0 - c2 ) * gy;
    gyro[select].z = gz = - g.gyro.x * DEGREE;
    gyro[select].z *= c2;
    gyro[select].z += (1.0 - c2 )* gz;

    count[select]++;
    if ( count[select] < 50 ) {
      tstart = millis();
      VectorCopy( &a0[select], &acc[select] );
      av0[select] = VectorNorm( &a0[select] );
      VectorCopy( &g0[select], &gyro[select] );
      count[select]++;
    }
    else {
      if ( count[select] < 200 ) {
        /*
          VectorAdd( &a0[select], &a0[select], &acc[select]);
          VectorAdd( &g0[select], &g0[select], &gyro[select]);
        */
        /**/
        VectorBlend( &a0[select], 0.5, &a0[select], &acc[select]);
        av0[select] = VectorNorm( &a0[select] );
        if ( av0[select] == NAN ) av0[select] = 9.8;
        VectorBlend( &g0[select], 0.5, &g0[select], &gyro[select]);
        count[select]++;
        /**/
        /*
          if ( count == 100 ) {
          VectorScale( &a0[select], 1.0/91.0, &a0[select] );
          VectorScale( &g0[select], 1.0/91.0, &g0[select] );
          VectorSub( &acc[select], &acc[select], &a0[select] );
          VectorSub( &gyro[select], &gyro[select], &g0[select] );
          }
        */
      }
      else {
        //VectorSub( &acc[select], &acc[select], &a0[select] );
        //VectorSub( &gyro[select], &gyro[select], &g0[select] );
        //VectorScale( &acc[select], 9.8/av0, &acc[select] );
      }
    }
    //VectorSub( &acc[select], &acc[select], &a0[select] );
    VectorSub( &gyro[select], &gyro[select], &g0[select] );
    VectorScale( &acc[select], 9.8 / av0[select], &acc[select] );
    av = VectorNorm( &acc[select] );
    //VectorSub( &gyro[select], &gyro[select], &g0[select] );

    matrix_t dm, m, mt;
    MatrixRot( dm, gyro[select].z * dt, gyro[select].x * dt, gyro[select].y * dt );
    /*
      MatrixTrans( mt, acc[select].x * dt, acc[select].y * dt, acc[select].z * dt);
      MatrixMult( dm, mt, m );
    */
    MatrixMult( m, dm, mat[select] );
    MatrixGetRot( m, &roll, &pitch, &yaw );
    
    if ( roll == NAN ) {
      roll = 0.0;
    }
    if ( pitch == NAN ) {
      pitch = 0.0;
    }
    if( pitch >= 90.0 ) pitch = 0.0;
    if( pitch <= -90.0 ) pitch = 0.0;
    
    if ( yaw == NAN ) {
      yaw = 0.0;
    }
    

    //MatrixCopy( mat[select], m );
    //MatrixRot( mat[select], roll, pitch, yaw );

    //k = 1.0;
    //k = 0.0;
    //k = 0.1;
    float w;//振動判定
    w = ( 1 - av / av0[select] ) * 9.5;
    w *= w;
    k = 0.1 * ( 1 - expf( - w ) );
    //k = 0.1;
    //-------- estimate rotation from acceleration
    float az_av;
    az_av = az / av;
    if( az_av == NAN ) az_av = 0.0;
    if( az_av >= 1.0 ) az_av = 0.999;
    if( az_av <= -1.0) az_av = -0.999;
    pitch = (1 - k) * pitch + k * asinf( - az_av ) * DEGREE;
    if( ax == 0.0 && ay == 0.0 ){
      roll = 0.0;
    }
    else{
      roll  = (1 - k) * roll  + k * atan2f( ax, ay ) * DEGREE;
    }
    yaw = yaw * 0.995;/////////yawの値を0にする速度を変換

    MatrixRot( mat[select], roll, pitch, yaw );
    //MatrixSetO( mat[select], &pos[select] );

    rot[select].roll = roll;
    rot[select].pitch = pitch;
    rot[select].yaw = yaw;

    if( count[select] == 200 ){
      matrix_t m;
      MatrixIdent( m );
      //MatrixRot( m, roll, pitch, yaw );
      vector_t tmp;
      VectorXform( &tmp, &a0[select], m );
      VectorCopy( &a0[select], &tmp );
      VectorScale( &a0[select], 9.8 / av0[select], &a0[select] );
      VectorSet( &pos[select], 0.0, 0.0, 0.0 );
      VectorSet( &vel[select], 0.0, 0.0, 0.0 );
    }

    vector_t da, v, dir, dist, vert;
    matrix_t inv;
    MatrixInvRot( inv, roll, pitch, yaw );
    //VectorCopy( &vert, &a0[select] );
    //VectorCopy( &dir, &acc[select] );
    //VectorXform( &vert, &a0[select], inv );
    //VectorXform( &dir, &acc[select], inv );
    //VectorSub( &da, &dir, &vert );
    VectorSub( &dir, &acc[select], &a0[select] );
    VectorXform( &da, &dir, inv );
 
    VectorBlend( &a0[select], 0.998, &a0[select], &acc[select] );////////
    VectorScale( &v, dt, &da );
    
    VectorAdd( &vel[select], &vel[select], &v ); 
    VectorScale( &v, dt, &vel[select] );
    VectorScale( &vel[select], 0.98, &vel[select] );

    //VectorBlend( &pos[select], 0.95, &pos[select], &v );
    VectorAdd( &pos[select], &pos[select], &v );
    
    /*
    MatrixGetO( m, &pos[select] );
    if ( pos[select].x == NAN || pos[select].y == NAN || pos[select].y == NAN ) {
      VectorSet( &pos[select], 0.0, 0.0, 0.0 );
    }
    */
#if 1
    pos[select].x = - yaw / 180.0;
    pos[select].y = - roll/ 180.0;
    pos[select].z =  pitch / 90.0;
    /*
    pos[select].x = sinf( - yaw / 180.0 * 3.14 * 0.5 );
    pos[select].y = sinf( - roll/ 180.0 * 3.14 * 0.5 );
    pos[select].z = sinf( - pitch / 90.0*3.14 * 0.5 );
    */
#endif
    if( pos[select].x >= 1.0 ) pos[select].x = 0.95;
    if( pos[select].x <=-1.0 ) pos[select].x =-0.95;
    if( pos[select].y >= 1.0 ) pos[select].y = 0.95;
    if( pos[select].y <=-1.0 ) pos[select].y =-0.95;
    if( pos[select].z >= 1.0 ) pos[select].z = 0.95;
    if( pos[select].z <=-1.0 ) pos[select].z =-0.95;
    pos[select].x = sinf( pos[select].x * 3.14 * 0.5 ) * 0.95;
    pos[select].y = sinf( pos[select].y * 3.14 * 0.5 ) * 0.95; 
    pos[select].z = sinf( pos[select].z * 3.14 * 0.5 ) * 0.95;
    VectorScale( &pos[select], 0.9, &pos[select] );
    
    /*
        matrix_t mat;
        MatrixRot( mat, roll, pitch, yaw );
        MatrixGetRot( mat, &roll, &pitch, &yaw );
    */

    if ( !console ) {
      //SendData( ax, ay, az, roll, pitch, yaw );
      //SendData2( ax, ay, az, gx, gy, gz, roll, pitch, yaw, 99.9 );
      vector_t sendpos;
      VectorCopy( &sendpos, &pos[select] );
      VectorScale( &sendpos, 10.0, &sendpos );
      SendData2( &sendpos, &gyro[select], &rot[select], 99.0 ); 
      //SendData2( &pos[select], &gyro[select], &rot[select], 99.0 );
      //SendData2( &acc[select], &gyro[select], &rot[select], 99.0 );
    }
    else {
      if ( !plotter ) {
        Serial.print(F("-------- MPU --------")); Serial.println( select );
        Serial.print(F("Accelerometer[m/sec2] "));
        Serial.print(F("X: ")); Serial.print(ax, 1); Serial.print(F(" , "));
        Serial.print(F("Y: ")); Serial.print(ay, 1); Serial.print(F(" , "));
        Serial.print(F("Z: ")); Serial.print(az, 1); Serial.println(F(" "));
        Serial.print(F("Gyroscope[deg/sec] "));
        Serial.print(F("X: ")); Serial.print(gx, 1); Serial.print(F(" , "));
        Serial.print(F("Y: ")); Serial.print(gy, 1); Serial.print(F(" , "));
        Serial.print(F("Z: ")); Serial.print(gz, 1); Serial.println(F(" "));
        Serial.print(F("Temperature ")); Serial.print(temp.temperature, 1); Serial.println(F(" degree C"));
      }
      else
      {
        if ( select == 1 ) {
#if 1
          /*
                    Serial.print( gyro[select].x ); Serial.print(",");
                    Serial.print( gyro[select].y ); Serial.print(",");
                    Serial.println( gyro[select].z );
          */
          /*
            Serial.print( da.x ); Serial.print(",");
            Serial.print( da.y ); Serial.print(",");
            Serial.println( da.z );
          */          
          /*
            Serial.print( a0[select].x ); Serial.print(",");
            Serial.print( a0[select].y ); Serial.print(",");
            Serial.print( a0[select].z ); Serial.print(",");
            Serial.println( av0[select] );
          */
          /*
            Serial.print( acc[select].x ); Serial.print(",");
            Serial.print( acc[select].y ); Serial.print(",");
            Serial.print( acc[select].z ); Serial.print(",");
            Serial.println( av0[select] );
          */
          /**/
            Serial.print( pos[select].x * 10); Serial.print(",");
            Serial.print( pos[select].y * 10); Serial.print(",");
            Serial.println( pos[select].z * 10);
          /**/
          /*
          Serial.print( rot[select].roll / 18 ); Serial.print(",");
          Serial.print( rot[select].pitch / 9 ); Serial.print(",");
          Serial.println( rot[select].yaw / 18 );
          */
          /*
            Serial.print( roll / 18 ); Serial.print(",");
            Serial.print( pitch / 9); Serial.print(",");
            Serial.print( yaw / 18); Serial.print(",");
            Serial.println( k * 100 );
          */
#else
          Serial.print( av ), Serial.print(",");
          Serial.println( k );
#endif
        }

      }
    }
#ifdef USE_OLED
    if (select == 0 ) {
      display.clearDisplay();
      display.setCursor(0, 0);

      display.print(F("Accelerometer - m/s^2")); display.println("");
      display.print(ax, 1); display.print(", ");
      display.print(ay, 1); display.print(", ");
      display.print(az, 1); display.println("");

      if ( 1 ) {
        display.print(F("rotation - degree")); display.println("");
        display.print(roll, 1); display.print(", ");
        display.print(pitch, 1); display.print(", ");
        display.print(yaw, 1); display.println("");
      }
      else {
        display.print(F("Gyroscope - deg/s")); display.println("");
        display.print(gx, 1); display.print(", ");
        display.print(gy, 1); display.print(", ");
        display.print(gz, 1); display.println("");
      }
      display.display();
    }
    else {
      //display.clearDisplay();

    }
#endif
    delay( 1 );
  }
  delay( 30 );
}
