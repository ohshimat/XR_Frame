void setAccRange( int accrange )
{
  mpu.setAccelerometerRange( accrange );
  if ( console && !plotter ) {
    Serial.print(F("Accelerometer range set to: "));
    switch (mpu.getAccelerometerRange()) {
      case MPU6050_RANGE_2_G:
        Serial.println(F("+-2G"));
        break;
      case MPU6050_RANGE_4_G:
        Serial.println(F("+-4G"));
        break;
      case MPU6050_RANGE_8_G:
        Serial.println(F("+-8G"));
        break;
      case MPU6050_RANGE_16_G:
        Serial.println(F("+-16G"));
        break;
    }
  }
}
void setGyroRange( int gyrorange )
{
  mpu.setGyroRange( gyrorange );
  if (console && !plotter) {
    Serial.print(F("Gyro range set to: "));
    switch (mpu.getGyroRange()) {
      case MPU6050_RANGE_250_DEG:
        Serial.println(F("+- 250 deg/s"));
        break;
      case MPU6050_RANGE_500_DEG:
        Serial.println(F("+- 500 deg/s"));
        break;
      case MPU6050_RANGE_1000_DEG:
        Serial.println(F("+- 1000 deg/s"));
        break;
      case MPU6050_RANGE_2000_DEG:
        Serial.println(F("+- 2000 deg/s"));
        break;
    }
  }
}
void setFilterBandwidth( int bandwidth )
{
  mpu.setFilterBandwidth(bandwidth);
  if ( console && !plotter ) {
    Serial.print(F("Filter bandwidth set to: "));
    switch (mpu.getFilterBandwidth()) {
      case MPU6050_BAND_260_HZ:
        Serial.println(F("260 Hz"));
        break;
      case MPU6050_BAND_184_HZ:
        Serial.println(F("184 Hz"));
        break;
      case MPU6050_BAND_94_HZ:
        Serial.println(F("94 Hz"));
        break;
      case MPU6050_BAND_44_HZ:
        Serial.println(F("44 Hz"));
        break;
      case MPU6050_BAND_21_HZ:
        Serial.println(F("21 Hz"));
        break;
      case MPU6050_BAND_10_HZ:
        Serial.println(F("10 Hz"));
        break;
      case MPU6050_BAND_5_HZ:
        Serial.println(F("5 Hz"));
        break;
    }
  }
}
void setCycleRate( int fps )
{
  mpu.setCycleRate( fps );
  if ( console && !plotter ) {
    Serial.print(F("Cycle rate set to: "));
    switch (mpu.getCycleRate()) {
      case MPU6050_CYCLE_1_25_HZ:
        Serial.println(F("1.25 Hz"));
        break;
      case MPU6050_CYCLE_5_HZ:
        Serial.println(F("5 Hz"));
        break;
      case MPU6050_CYCLE_20_HZ:
        Serial.println(F("20 Hz"));
        break;
      case MPU6050_CYCLE_40_HZ:
        Serial.println(F("40 Hz"));
        break;
    }
  }
}
