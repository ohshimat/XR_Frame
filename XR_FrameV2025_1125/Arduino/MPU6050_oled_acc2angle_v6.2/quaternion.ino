/*
 * 
void QuatRot( quat_t *q, float a, float x, float y, float z );
void MatrixQuat( matrix_t m, float x, float y, float z, float w );
*/
void QuatRot( quat_t *q, float a, float x, float y, float z )
{
  float s = sinf( a * RADIAN * 0.5 );
  q->x = x * s;
  q->y = y * s;
  q->z = z * s;
  q->w = cosf( a * RADIAN * 0.5 );
}//----
void QuatRot( quat_t *q, float a, vector_t *v )
{ QuatRot( q, a, v->x, v->y, v->z ); }
float RotQuat( float a, vector_t *v, quat_t *q)
{
  
}
//----
void MatrixQuat( matrix_t m, float x, float y, float z, float w )
{
  //1st row
  m[0] =  1.0 -2.0*y*y -2.0*z*z;
  m[1] = 2.0*x*y +2.0*w*z;
  m[2] = 2.0*x*z -2.0*w*y;
  m[3] = 0.0;
  //2nd row
  m[4] = 2.0*x*y -2.0*w*z;
  m[5] = -2.0*x*x +1.0 -2.0*z*z;
  m[6] = 2.0*y*z +2.0*w*x;
  m[7] = 0.0;
  //3rd row
  m[8] = 2.0*x*z +2.0*w*y;
  m[9] = 2.0*y*z -2.0*w*x;
  m[10]= -2.0*x*x -2.0*y*y +1.0;
  m[11]= 0.0;
  //4th row
  m[12]= 0.0;
  m[13]= 0.0;
  m[14]= 0.0;
  m[15]= 1.0;
}//----
void MatrixQuat( matrix_t m, const quat_t *q )
{ MatrixQuat( m, q->x, q->y, q->z, q->w );}
//----

bool QuatSlert( quat_t *qo, const quat_t *qa, const quat_t *qb, float t )
{
  float la = sqrtf( qa->x*qa->x + qa->y*qa->y + qa->z*qa->z + qa->w*qa->w);
  float lb = sqrtf( qb->x*qb->x + qb->y*qb->y + qb->z*qb->z + qb->w*qb->w);
  if( la==0.0 || lb==0.0 ) return false;
  float c = (qa->x*qb->x + qa->y*qb->y + qa->z*qb->z + qa->w*qb->w)/(la*lb);
  float w = acosf( c );
  float sw = sinf( w );
  float stw = sinf( t*w );
  float _stw = sinf( (1.0-t)*w );
  float ka = _stw / sw;
  float kb = stw / sw;
  qo->x = ka * qa->x + kb * qb->x;
  qo->y = ka * qa->y + kb * qb->y;
  qo->z = ka * qa->z + kb * qb->z;
  qo->w = ka * qa->w + kb * qb->w; 
}
