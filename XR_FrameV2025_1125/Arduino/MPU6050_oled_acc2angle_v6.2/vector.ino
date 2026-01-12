/*
void VectorSet( vector_t *v, float x, float y, float z );
void VectorCopy( vector_t *dst, const vector_t *src );
float VectorNorm( const vector_t *v );
float VectorDot( const vector_t *a, cosnt vector_t *b );
void VectorCross( vector_t *aXb, const vector_t *a, const vector_t *b );
void VectorAdd( vector_t *v, const vector_t *a, const vector_t *b );
void VectorSub( vector_t *v, const vector_t *a, const vector_t *b );
void VectorBlend( vector_t *v, float k, const vector_t *a, const vector_t *b );
float VectorAngle( const vector_t *a, const vector_t *b );
void VectorScale( vector_t *sv, float s, const vector_t *v );
void VectorPrint( const vector_t *v );
*/
//========
void VectorPrint( const vector_t *v ){
  //Serial.print("Vector: ");
  Serial.print( v->x ); Serial.print(F(","));
  Serial.print( v->y ); Serial.print(F(","));
  Serial.println( v->z ); //Serial.println(F(""));
}
void VectorSet( vector_t *v, float x, float y, float z ){
  v->x = x; v->y = y; v->z = z;
}//--------
void VectorCopy( vector_t *dst, const vector_t *src ){
  dst->x = src->x; dst->y = src->y; dst->z = src->z;
}//--------
float VectorNorm( const vector_t *v ){
  return sqrtf( v->x*v->x + v->y*v->y + v->z*v->z );
}//--------
float VectorNorm2( const vector_t *v ){
  return v->x*v->x + v->y*v->y + v->z*v->z;
}//--------
float VectorNormalize( vector_t *v ){
  float l = VectorNorm( v );
  v->x /= l; v->y /= l; v->z /= 1;
  return l;
}//--------
float VectorDot( const vector_t *a, const vector_t *b ){
  return a->x*b->x + a->y*b->y + a->z*b->z;
}//--------
void VectorCross( vector_t *aXb, const vector_t *a, const vector_t *b ){
  aXb->x = a->y*b->z - a->z*b->y;
  aXb->y = a->z*b->x - a->x*b->z;
  aXb->z = a->x*b->y - a->y*b->x;
}//--------
void VectorAdd( vector_t *v, const vector_t *a, const vector_t *b ){
  v->x = a->x + b->x;
  v->y = a->y + b->y;
  v->z = a->z + b->z;
}//--------
void VectorSub( vector_t *v, const vector_t *a, const vector_t *b ){
  v->x = a->x - b->x;
  v->y = a->y - b->y;
  v->z = a->z - b->z;
}//--------
void VectorBlend( vector_t *v, float k, const vector_t *a, const vector_t *b ){
  float _k = 1.0 - k;
  v->x = k* a->x + _k* b->x;
  v->y = k* a->y + _k* b->y;
  v->z = k* a->z + _k* b->z;
}//--------
float VectorAngle( const vector_t *a, const vector_t *b ){
  return acosf( VectorDot( a, b ) / sqrtf( VectorNorm2( a ) * VectorNorm2( b ) ) ) * DEGREE;
}//--------
float VectorAngle( const vector_t *a, const vector_t *b, const vector_t *ref ){
  float angle;
  vector_t aXb;
  angle = acosf( VectorDot( a, b ) / sqrtf( VectorNorm2( a ) * VectorNorm2( b ) ) ) * DEGREE;
  VectorCross( &aXb, a, b );
  if( VectorAngle( &aXb, ref ) < 90 ) return angle;
  else return - angle;
  return angle;
}//--------
void VectorScale( vector_t *sv, float s, const vector_t *v ){
  sv->x = s * v->x;
  sv->y = s * v->y;
  sv->z = s * v->z;
}//--------
void VectorScale( vector_t *ab, const vector_t *a, const vector_t *b ){
  ab->x = a->x * b->x;
  ab->y = a->y * b->y;
  ab->z = a->z * b->z;
}//--------
void VectorXform( vector_t *vo, const vector_t *vi, const matrix_t m ){
  vo->x = vi->x * m[0] + vi->y * m[4] + vi->z * m[8] + m[12];
  vo->y = vi->x * m[1] + vi->y * m[5] + vi->z * m[9] + m[13];
  vo->z = vi->x * m[2] + vi->y * m[6] + vi->z * m[10] + m[14];
}//--------
void EulerSet( euler_t *rot, float roll, float pitch, float yaw ){
  rot->roll = roll; rot->pitch = pitch; rot->yaw = yaw;
}
//========
