#if 0
void SendData( 
  float accx, float accy, float accz, 
  float gyrox, float gyroy, float gyroz
  )
{
  //unsigned char型は0～255の値しか入れられない
  unsigned char sensors[7];
  
  //読み取った加速度とジャイロの値をシリアル通信できる形に変換
  //範囲は[-32768～32768]→[0～255]に変換

  //加速度
  float gFX = map(accx,-20.0,20.0,0.00,254.00);
  float gFY = map(accy,-20.0,20.0,0.00,254.00);
  float gFZ = map(accz,-20.0,20.0,0.00,254.00);

  //ジャイロ
  float rtX = map(gyrox,-180.00,180.00,0.00,254.00);
  float rtY = map(gyroy,-180.00,180.00,0.00,254.00);
  float rtZ = map(gyroz,-180.00,180.00,0.00,254.00);
  
  //加速度XYZ(傾き)をsensorsに格納
  
  sensors[0] = 255;//0-255
  
  sensors[1] = gFX;//(int)gFX;//0-255
  sensors[2] = gFY;//(int)gFY;//0-255
  sensors[3] = gFZ;//(int)gFZ;//0-255

  //ジャイロXYZ(角度が変化する速度)をsensorsに格納
  
  sensors[4] = rtX;//(int)rtX;//0-255
  sensors[5] = rtY;//(int)rtY;//0-255
  sensors[6] = rtZ;//(int)rtZ;//0-255
  
  //// シリアル通信で送信する
  Serial.write(sensors,7);
  //データの送信がすべて完了するまで待つ
  Serial.flush();
}
/*
  if( 0 ){
      printData(); /// シリアルモニタでデータを確認するとき使う
  }
  else{
    
    while( Serial.available() < 1 ){
      delay(1);
    }
    rxdata = Serial.read();
    

    ////処理を休む
  }
}
*/
#endif
