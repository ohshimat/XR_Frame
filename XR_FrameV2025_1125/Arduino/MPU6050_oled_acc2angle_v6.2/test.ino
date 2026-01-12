void testmatrix( void ){

  matrix_t mat, mat1, mat2;
  vector_t v0, v1, v2, v3;
  
  Serial.println(F("MatrixIdent"));
  MatrixIdent( mat );
  MatrixPrint( mat );
  Serial.println(F("MatrixMult"));
  MatrixIdent( mat1);
  MatrixMult( mat2, mat, mat1);
  MatrixPrint( mat2 );
  
  matrix_t rx;
  Serial.println(F("MatrixRotX") );
  MatrixRotX( rx, 30.0 );
  MatrixPrint( rx );
  
  matrix_t ry;
  Serial.println(F("MatrixRotY") );
  MatrixRotY( ry, 30.0 );
  MatrixPrint( ry );

  matrix_t rz;
  Serial.println(F("MatrixRotZ") );
  MatrixRotZ( rz, 30.0 );
  MatrixPrint( rz );

  matrix_t mt;
  Serial.println(F("MatrixTrans") );
  vector_t v = { 1.0, 2.0, 3.0 };
  MatrixTrans( mt, &v );
  MatrixPrint( mt );

  matrix_t mr;
  Serial.println(F("MatrixRot"));
  MatrixRot( mr, 30.0, 30.0, 30 );
  MatrixPrint( mr );
  VectorSet( &v0, 1.0, 1.0, 1.0 );
  VectorXform( &v1, &v0, mr );
  VectorPrint( &v1 );
  
  //Roll->Pitch->Yaw
  MatrixMult( mat, rz, rx );
  MatrixMult( mat1, mat, ry );
  MatrixPrint( mat1 );
  VectorSet( &v0, 1.0, 1.0, 1.0 );  
  VectorXform( &v1, &v0, mat1 );
  VectorPrint( &v1 );

  VectorSet( &v0, 1.0, 1.0, 1.0 );
  VectorXform( &v1, &v0, rz );
  VectorXform( &v0, &v1, rx );
  VectorXform( &v1, &v0, ry );
  VectorPrint( &v1 );
  
  delay_500();
  unsigned long time_start;
  unsigned long time_stop;
  Serial.println(F("START"));
  time_start = millis();
  matrix_t ab, a, b;
  for( int i=0; i<1000; i++ ){
    //Serial.println(F("MatrixRotZ") );
    //MatrixRotZ( mat, 30.0 );
    //MatrixRot( mat, 30.0, 30.0, 30.0 );
    //MatrixMult( ab, a, b );
    MatrixIdent( ab );
    //MatrixPrint( mat ); 
  }
  time_stop = millis();
  Serial.println( time_stop - time_start );

#if 0
  vector_t vo, vi;
  VectorSet( &vi, 1.0, 0.0, 0.0 );
  Serial.print("x");
  Serial.print(",");
  Serial.print("y");
  Serial.print(",");    
  Serial.println("z");
  VectorSet( &vi, 0.0, 1.0, 0.0 );
  MatrixRotX( rx, 3.0 );  
  for( int i=0; i<360; i+=3 ){
    VectorPrint( &vo );    
    VectorXform( &vo, &vi, rx );
    VectorCopy( &vi, &vo );
  }
  delay_500();
  VectorSet( &vi, 0.0, 0.0, 1.0 );  
  MatrixRotY( ry, 3.0 );   
  for( int i=0; i<360; i+=3 ){
    VectorPrint( &vo );   
    VectorXform( &vo, &vi, ry );
    VectorCopy( &vi, &vo );
  }
  delay_500();
  VectorSet( &vi, 1.0, 0.0, 0.0 );  
  MatrixRotZ( rz, 3.0 ); 
  for( int i=0; i<360; i+=3 ){
    VectorPrint( &vo );    
    VectorXform( &vo, &vi, rz );
    VectorCopy( &vi, &vo );
  }
  delay( 5000 );
  while(1) yield();
#endif
}
