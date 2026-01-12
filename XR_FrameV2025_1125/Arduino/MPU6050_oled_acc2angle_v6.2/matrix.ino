/*
  void MatrixCopy( matrix_t dst, const matrix_t src );
  void MatrixPrint( const matrix_t m );
  void MatrixIdent( matrix_t m );
  void MatrixMult( matrix_t ab, const matrix_t a, const matrix_t b );
  void MatrixTrans( matrix_t m, const vector_t *p );
  void MatrixRotX( matrix_t rx, float pitch );
  void MatrixRotY( matrix_t ry, float yaw );
  void MatrixRotZ( matrix_t rz, float roll );
  void MatrixRot( matrix_t mr, float roll, float pitch, float yaw );
  void MatrixRot( matrix_t mr, euler_t *angle );
  void MatrixRot( matrix_t m, float a, float x, float y, float z );
  void MatrixRot( matrix_t m, float a, vector_t *v );

  void MatrixGetX( const matrix_t m, vector_t *xaxis );
  void MatrixGetY( const matrix_t m, vector_t *yaxis );
  void MatrixGetZ( const matrix_t m, vector_t *zaxis );
  void MatrixGetO( const matrix_t m, vector_t *pos );
  void MatrixGetRot( const matrix_t mr, float *roll, float *pitch, float *yaw );
  void MatrixGetRot( const matrix_t mr, euler_t *angle );
*/
//========
void MatrixCopy( matrix_t dst, const matrix_t src ) {
  memcpy( dst, src, sizeof(matrix_t) );
}//--------
void MatrixIdent( matrix_t m ) {
#if 0
  matrix_t e = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  memcpy( m, e, sizeof(matrix_t) );
#else
  float *p = m;
  int i, j;
  for ( i = 15; i > 0; i-- ) {
    *p++ = 0.0;
  }
  p = m;
  *p = 1.0;
  *(p + 5) = 1.0;
  *(p + 10) = 1.0;
  *(p + 15) = 1.0;
  /*
    for( i = 4; i >= 0; i-- ){
     p = 1.0;
    p+= 5;
    }
  */
  /*
    for ( i = 0; i < 4; i++ ) {
    for ( j = 0; j < 4; j++ ) {
      if ( i == j ) *p = 1.0;
      else *p = 0.0;
      p++;
    }
    }
  */
#endif
}//--------
void MatrixMult( matrix_t ab, const matrix_t a, const matrix_t b ) {
  float *mab, *ma, *mb;
  mab = ab;
  int i, j, k;
  for ( i = 0; i < 16; i += 4 ) {
    //ma = &a[ i ]; //slower
    for ( j = 0; j < 4; j++ ) {
      //mab = &ab[ i + j ];
      ma = &a[ i ]; //faster
      mb = &b[ j ];
#if 1 //faster than latter
      *mab = *ma * *mb
             + *(++ma) * *(mb += 4)
             + *(++ma) * *(mb += 4)
             + *(++ma) * *(mb += 4);
#else //slower
      *mab = 0.0;
      for ( k = 0; k < 4; k++ ) {
        *mab += *ma * *mb;
        ma++;
        mb += 4;
      }
#endif
      mab++;
    }
  }
}//--------
void MatrixPrint( const matrix_t m ) {
  float *p = m;
  Serial.println( F("matrix ----") );
  for ( int i = 0; i < 4; i++ ) {
    for ( int j = 0; j < 4; j++ ) {
      Serial.print( *p );
      Serial.print("_" );
      p++;
    }
    Serial.println( "" );
  }
}//--------
void MatrixTrans( matrix_t mt, const vector_t *t ) {
  MatrixTrans( mt, t->x, t->y, t->z );
}//--------
void MatrixTrans( matrix_t mt, float x, float y, float z ) {
  MatrixIdent( mt );
  mt[12] = x;
  mt[13] = y;
  mt[14] = z;
}
void MatrixRotX( matrix_t rx, float pitch )
{
  float c, s;
  MatrixIdent( rx );
  pitch *= RADIAN;
  c = cosf( pitch );
  s = sinf( pitch );
  rx[5] = c; rx[6] = s;
  rx[9] = -s; rx[10] = c;
}//--------
void MatrixRotY( matrix_t ry, float yaw )
{
  float c, s;
  MatrixIdent( ry );
  yaw *= RADIAN;
  c = cosf( yaw );
  s = sinf( yaw );
  ry[10] = c; ry[8] = s;
  ry[2] = -s; ry[0] = c;
}//--------
void MatrixRotZ( matrix_t rz, float roll )
{
  float c, s;
  MatrixIdent( rz );
  roll *= RADIAN;
  c = cosf( roll );
  s = sinf( roll );
  //s = 1.0 - c * c;
  rz[0] = c; rz[1] = s;
  rz[4] = -s; rz[5] = c;
}//--------
void MatrixRot( matrix_t mr, float roll, float pitch, float yaw )
{
  matrix_t m0, m1;
  MatrixRotZ( mr, roll ); //roll
  MatrixRotX( m0, pitch ); //pitch
  MatrixMult( m1, mr, m0  ); //roll*pitch
  MatrixRotY( m0, yaw ); //yaw
  MatrixMult( mr, m1, m0 ); //roll*pitch*yaw
}//--------
void MatrixRot( matrix_t m, float a, float x, float y, float z )
{
  float c = cosf( a * RADIAN );
  float c1 = 1.0 - c;
  float s = sinf( a * RADIAN );
  //----1st row
  m[0] = x * x * c1 + c;
  m[1] = x * y * c1 + z * s;
  m[2] = x * z * c1 - y * s;
  m[3] = 0.0;
  //----2nd row
  m[4] = y * x * c1 - z * s;
  m[5] = y * y * c1 + c;
  m[6] = y * z * c1 + x * s;
  m[7] = 0.0;
  //----3rd row
  m[8] = z * x * c1 + y * s;
  m[9] = z * y * c1 - x * s;
  m[10] = z * z * c1 + c;
  m[11] = 0.0;
  //----4th row
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
}//--------
void MatrixRot( matrix_t m, float a, const vector_t *v )
{
  MatrixRot( m, v->x, v->y, v->z, a );
}
//--------
void MatrixInvRot( matrix_t mr, float roll, float pitch, float yaw )
{
  matrix_t m0, m1;
  MatrixRotY( mr, -yaw ); // -yaw
  MatrixRotX( m0, -pitch ); // -pitch
  MatrixMult( m1, mr, m0  ); // -yaw * -pitch
  MatrixRotZ( m0, -roll ); // -roll
  MatrixMult( mr, m1, m0 ); // -yaw * -pitch * -roll
}//--------
void MatrixGetX( const matrix_t m, vector_t *xaxis )
{
  xaxis->x = m[0];
  xaxis->y = m[1];
  xaxis->z = m[2];
}
//--------
void MatrixGetY( const matrix_t m, vector_t *yaxis )
{
  yaxis->x = m[4];
  yaxis->y = m[5];
  yaxis->z = m[6];
}
//--------
void MatrixGetZ( const matrix_t m, vector_t *zaxis )
{
  zaxis->x = m[8];
  zaxis->y = m[9];
  zaxis->z = m[10];
}
//--------
void MatrixGetO( const matrix_t m, vector_t *pos )
{
  pos->x = m[12];
  pos->y = m[13];
  pos->z = m[14];
}
void MatrixSetO( matrix_t m, const vector_t *pos )
{
  m[12] = pos->x;
  m[13] = pos->y;
  m[14] = pos->z;
}
//--------
void MatrixRot( const matrix_t mr, euler_t *angle )
{
  MatrixRot( mr, angle->roll, angle->pitch, angle->yaw );
}
//--------
void MatrixGetRot( const matrix_t mr, float *roll, float *pitch, float *yaw )
{
  vector_t dir, right, up;
  vector_t xaxis, yaxis, zaxis;

  MatrixGetX( mr, &xaxis );
  MatrixGetY( mr, &yaxis );
  MatrixGetZ( mr, &zaxis );
  // dir vector
  VectorScale( &dir, -1.0, &zaxis );
  float l;
  //l = sqrtf( dir.x*dir.x + dir.y*dir.y + dir.z*dir.z );
  l = VectorNorm( &dir );
  if ( l <= 0 ) {
    VectorSet( &dir, 0.0, 0.0, -1.0 );
    l = 1.0;
  }
  //---- YAW
  *yaw = atan2f( - dir.x, - dir.z ) * DEGREE;
  //---- PITCH
  *pitch = asinf( dir.y / l ) * DEGREE;
  //---- ROLL
  // right vector on XZ plane
  VectorSet( &up, 0.0, 1.0, 0.0 );
  VectorCross( &right, &dir, &up );
  // up vector
  VectorCross( &up, &right, &dir );
  // roll
  VectorCross( &dir, &up, &yaxis );
  *roll = VectorAngle( &up, &yaxis );
  if ( VectorDot( &dir, &zaxis ) < 0 ) {
    *roll *= -1;
  }
}//--------

//--------
