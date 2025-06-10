#ifndef MATHLITE_H
#define MATHLITE_H

//-----------------------------------------------------------------------------
// includes

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

#if defined( OSX ) && defined( __aarch64__ )
#include <simd/simd.h>
#else
#include <xmmintrin.h>
#endif

//-----------------------------------------------------------------------------
// macros

#define FLOAT32_NAN_BITS     (unsigned long)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )
#define VEC_T_NAN FLOAT32_NAN

//#define FastSqrt(x) sqrt(x)

#ifndef Assert
#define Assert(x)
#endif

#ifndef RAD2DEG
	#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
	#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

#ifndef M_PI
	#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.


//-----------------------------------------------------------------------------
// typedefs
typedef float vec_t;

enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

//-----------------------------------------------------------------------------
// inlines

inline float fpmin( float a, float b )
{
	return ( a < b ) ? a : b;
}

inline float fpmax( float a, float b )
{
	return ( a > b ) ? a : b;
}


inline unsigned long& FloatBits( vec_t& f )
{
	return *reinterpret_cast<unsigned long*>((char*)(&f));
}


inline unsigned long FloatBits( const vec_t &f )
{
	union Convertor_t
	{
		vec_t f;
		unsigned long ul;
	}tmp;
	tmp.f = f;
	return tmp.ul;
}


inline vec_t BitsToFloat( unsigned long i )
{
	union Convertor_t
	{
		vec_t f;
		unsigned long ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

inline bool IsFinite( const vec_t &f )
{
#if _X360
	return f == f && fabs(f) <= FLT_MAX;
#else
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
#endif
}

inline unsigned long FloatAbsBits( vec_t f )
{
	return FloatBits(f) & 0x7FFFFFFF;
}

inline float FloatMakeNegative( vec_t f )
{
	return BitsToFloat( FloatBits(f) | 0x80000000 );
}

inline float FloatMakePositive( vec_t f )
{
	return (float)fabs( f );
}

inline void SinCos( float radians, float *sine, float *cosine )
{
	*sine = sin(radians);
	*cosine = cos(radians);
}

//-----------------------------------------------------------------------------
// The following are not declared as macros because they are often used in limiting situations,
// and sometimes the compiler simply refuses to inline them for some reason
#ifndef FastSqrt
inline float FastSqrt( float x )
{
#if defined( OSX ) && defined( __aarch64__ )
	return simd::sqrt( x );
#else
	__m128 root = _mm_sqrt_ss( _mm_load_ss( &x ) );
	return *( reinterpret_cast<float *>( &root ) );
#endif
}
#endif

inline float FastRSqrtFast( float x )
{
#if defined( OSX ) && defined( __aarch64__ )
	return simd::fast::rsqrt( x );
#else
	// use intrinsics
	__m128 rroot = _mm_rsqrt_ss( _mm_load_ss( &x ) );
	return *( reinterpret_cast<float *>( &rroot ) );
#endif
}
// Single iteration NewtonRaphson reciprocal square root:
// 0.5 * rsqrtps * (3 - x * rsqrtps(x) * rsqrtps(x)) 	
// Very low error, and fine to use in place of 1.f / sqrtf(x).	
inline float FastRSqrt( float x )
{
	float rroot = FastRSqrtFast( x );
	return (0.5f * rroot) * (3.f - (x * rroot) * rroot);
}

//-----------------------------------------------------------------------------
// classes

// Used to make certain code easier to read.
#define X_INDEX	0
#define Y_INDEX	1
#define Z_INDEX	2


#ifdef VECTOR_PARANOIA
#define CHECK_VALID( _v)	Assert( (_v).IsValid() )
#else
#ifdef GNUC
#define CHECK_VALID( _v)
#else
#define CHECK_VALID( _v)	0
#endif
#endif

#define VecToString(v)	(static_cast<const char *>(CFmtStr("(%f, %f, %f)", (v).x, (v).y, (v).z))) // ** Note: this generates a temporary, don't hold reference!

class VectorByValue;

//=========================================================
// 3D Vector
//=========================================================
class Vector					
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction:
	Vector(void); 
	Vector(vec_t X, vec_t Y, vec_t Z);

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f);
	 // TODO (Ilya): Should there be an init that takes a single float for consistency?

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Cast to Vector2D...
	//Vector2D& AsVector2D();
	//const Vector2D& AsVector2D() const;

	// Initialization methods
	void Random( vec_t minVal, vec_t maxVal );
	inline void Zero(); ///< zero out a vector

	// equality
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;	

	// arithmetic operations
	inline Vector&	operator+=(const Vector &v);			
	inline Vector&	operator-=(const Vector &v);		
	inline Vector&	operator*=(const Vector &v);			
	inline Vector&	operator*=(float s);
	inline Vector&	operator/=(const Vector &v);		
	inline Vector&	operator/=(float s);	
	inline Vector&	operator+=(float fl) ; ///< broadcast add
	inline Vector&	operator-=(float fl) ; ///< broadcast sub			

// negate the vector components
	void	Negate(); 

	// Get the vector's magnitude.
	inline vec_t	Length() const;

	// Get the vector's magnitude squared.
	inline vec_t LengthSqr(void) const
	{ 
		CHECK_VALID(*this);
		return (x*x + y*y + z*z);		
	}

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance &&
				y > -tolerance && y < tolerance &&
				z > -tolerance && z < tolerance);
	}

	vec_t	NormalizeInPlace();
	Vector	Normalized() const;
	bool	IsLengthGreaterThan( float val ) const;
	bool	IsLengthLessThan( float val ) const;

	// check if a vector is within the box defined by two other vectors
	inline bool WithinAABox( Vector const &boxmin, Vector const &boxmax);
 
	// Get the distance from this vector to the other one.
	vec_t	DistTo(const Vector &vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' inline.  
	// may be able to tidy this up after switching to VC7
	inline vec_t DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	// Copy
	void	CopyToArray(float* rgfl) const;	

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void	MulAdd(const Vector& a, const Vector& b, float scalar);	

	// Dot product.
	vec_t	Dot(const Vector& vOther) const;			

	// assignment
	Vector& operator=(const Vector &vOther);

	// returns 0, 1, 2 corresponding to the component with the largest absolute value
	inline int LargestComponent() const;

	// 2d
	vec_t	Length2D(void) const;					
	vec_t	Length2DSqr(void) const;					

	operator VectorByValue &()				{ return *((VectorByValue *)(this)); }
	operator const VectorByValue &() const	{ return *((const VectorByValue *)(this)); }

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
//	Vector(const Vector &vOther);

	// arithmetic operations
	Vector	operator-(void) const;
				
	Vector	operator+(const Vector& v) const;	
	Vector	operator-(const Vector& v) const;	
	Vector	operator*(const Vector& v) const;	
	Vector	operator/(const Vector& v) const;	
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;			
	
	// Cross product between two vectors.
	Vector	Cross(const Vector &vOther) const;		

	// Returns a vector with the min or max in X, Y, and Z.
	Vector	Min(const Vector &vOther) const;
	Vector	Max(const Vector &vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector(const Vector& vOther);
#endif
};



#define USE_M64S ( ( !defined( _X360 ) ) )



//=========================================================
// 4D Short Vector (aligned on 8-byte boundary)
//=========================================================
#if 0
class ALIGN8 ShortVector
{
public:

	short x, y, z, w;

	// Initialization
	void Init(short ix = 0, short iy = 0, short iz = 0, short iw = 0 );


#if USE_M64S
	__m64 &AsM64() { return *(__m64*)&x; }
	const __m64 &AsM64() const { return *(const __m64*)&x; } 
#endif

	// Setter
	void Set( const ShortVector& vOther );
	void Set( const short ix, const short iy, const short iz, const short iw );

	// array access...
	short operator[](int i) const;
	short& operator[](int i);

	// Base address...
	short* Base();
	short const* Base() const;

	// equality
	bool operator==(const ShortVector& v) const;
	bool operator!=(const ShortVector& v) const;	

	// Arithmetic operations
	inline ShortVector& operator+=(const ShortVector &v);			
	inline ShortVector& operator-=(const ShortVector &v);		
	inline ShortVector& operator*=(const ShortVector &v);			
	inline ShortVector& operator*=(float s);
	inline ShortVector& operator/=(const ShortVector &v);		
	inline ShortVector& operator/=(float s);					
	inline ShortVector operator*(float fl) const;

private:

	// No copy constructors allowed if we're in optimal mode
//	ShortVector(ShortVector const& vOther);

	// No assignment operators either...
//	ShortVector& operator=( ShortVector const& src );

} ALIGN8_POST;
#endif




#if 0
//=========================================================
// 4D Integer Vector
//=========================================================
class IntVector4D
{
public:

	int x, y, z, w;

	// Initialization
	void Init(int ix = 0, int iy = 0, int iz = 0, int iw = 0 );

#if USE_M64S
	__m64 &AsM64() { return *(__m64*)&x; }
	const __m64 &AsM64() const { return *(const __m64*)&x; } 
#endif

	// Setter
	void Set( const IntVector4D& vOther );
	void Set( const int ix, const int iy, const int iz, const int iw );

	// array access...
	int operator[](int i) const;
	int& operator[](int i);

	// Base address...
	int* Base();
	int const* Base() const;

	// equality
	bool operator==(const IntVector4D& v) const;
	bool operator!=(const IntVector4D& v) const;	

	// Arithmetic operations
	inline IntVector4D& operator+=(const IntVector4D &v);			
	inline IntVector4D& operator-=(const IntVector4D &v);		
	inline IntVector4D& operator*=(const IntVector4D &v);			
	inline IntVector4D& operator*=(float s);
	inline IntVector4D& operator/=(const IntVector4D &v);		
	inline IntVector4D& operator/=(float s);					
	inline IntVector4D operator*(float fl) const;

private:

	// No copy constructors allowed if we're in optimal mode
	//	IntVector4D(IntVector4D const& vOther);

	// No assignment operators either...
	//	IntVector4D& operator=( IntVector4D const& src );

};

#endif

//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class VectorByValue : public Vector
{
public:
	// Construction/destruction:
	VectorByValue(void) : Vector() {} 
	VectorByValue(vec_t X, vec_t Y, vec_t Z) : Vector( X, Y, Z ) {}
	VectorByValue(const VectorByValue& vOther) { *this = vOther; }
};


//-----------------------------------------------------------------------------
// Utility to simplify table construction. No constructor means can use
// traditional C-style initialization
//-----------------------------------------------------------------------------
class TableVector
{
public:
	vec_t x, y, z;

	operator Vector &()				{ return *((Vector *)(this)); }
	operator const Vector &() const	{ return *((const Vector *)(this)); }

	// array access...
	inline vec_t& operator[](int i)
	{
		Assert( (i >= 0) && (i < 3) );
		return ((vec_t*)this)[i];
	}

	inline vec_t operator[](int i) const
	{
		Assert( (i >= 0) && (i < 3) );
		return ((vec_t*)this)[i];
	}
};


//-----------------------------------------------------------------------------
// Here's where we add all those lovely SSE optimized routines
//-----------------------------------------------------------------------------

#if 0
class ALIGN16 VectorAligned : public Vector
{
public:
	inline VectorAligned(void) {};
	inline VectorAligned(vec_t X, vec_t Y, vec_t Z) 
	{
		Init(X,Y,Z);
	}

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	VectorAligned(const VectorAligned& vOther);
	VectorAligned(const Vector &vOther);

#else
public:
	explicit VectorAligned(const Vector &vOther) 
	{
		Init(vOther.x, vOther.y, vOther.z);
	}
	
	VectorAligned& operator=(const Vector &vOther)	
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned& operator=(const VectorAligned &vOther)
	{
		// we know we're aligned, so use simd
		// we can't use the convenient abstract interface coz it gets declared later
#ifdef _X360
		XMStoreVector4A(Base(), XMLoadVector4A(vOther.Base()));
#elif _WIN32
		_mm_store_ps(Base(), _mm_load_ps( vOther.Base() ));
#else
		Init(vOther.x, vOther.y, vOther.z);
#endif
		return *this;
	}

	
#endif
	float w;	// this space is used anyway

	void* operator new[] ( size_t nSize)
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}

	void* operator new[] ( size_t nSize, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void* operator new[] ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void operator delete[] ( void* p) 
	{
		MemAlloc_FreeAligned(p,true);
	}

	void operator delete[] ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete[] ( void* p, int /*nBlockUse*/, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	// please don't allocate a single quaternion...
	void* operator new   ( size_t nSize )
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}
	void* operator new   ( size_t nSize, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void* operator new   ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void operator delete ( void* p) 
	{
		MemAlloc_FreeAligned(p,true);
	}

	void operator delete ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete ( void* p, int /*nBlockUse*/, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}
} ALIGN16_POST;

#endif

//-----------------------------------------------------------------------------
// Vector related operations
//-----------------------------------------------------------------------------

// Vector clear
inline void VectorClear( Vector& a );

// Copy
inline void VectorCopy( const Vector& src, Vector& dst );

// Vector arithmetic
inline void VectorAdd( const Vector& a, const Vector& b, Vector& result );
inline void VectorSubtract( const Vector& a, const Vector& b, Vector& result );
inline void VectorMultiply( const Vector& a, vec_t b, Vector& result );
inline void VectorMultiply( const Vector& a, const Vector& b, Vector& result );
inline void VectorDivide( const Vector& a, vec_t b, Vector& result );
inline void VectorDivide( const Vector& a, const Vector& b, Vector& result );

// Vector equality with tolerance
bool VectorsAreEqual( const Vector& src1, const Vector& src2, float tolerance = 0.0f );

#define VectorExpand(v) (v).x, (v).y, (v).z


// Normalization
// FIXME: Can't use quite yet
//vec_t VectorNormalize( Vector& v );

// Length
inline vec_t VectorLength( const Vector& v );

// Dot Product
inline vec_t DotProduct(const Vector& a, const Vector& b);

// Cross product
void CrossProduct(const Vector& a, const Vector& b, Vector& result );

// Store the min or max of each of x, y, and z into the result.
void VectorMin( const Vector &a, const Vector &b, Vector &result );
void VectorMax( const Vector &a, const Vector &b, Vector &result );

// Linearly interpolate between two vectors
void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest );
Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t );

inline Vector ReplicateToVector( float x )
{
	return Vector( x, x, x );
}

inline bool PointWithinViewAngle( Vector const &vecSrcPosition, 
									   Vector const &vecTargetPosition, 
									   Vector const &vecLookDirection, float flCosHalfFOV )
{
	Vector vecDelta = vecTargetPosition - vecSrcPosition;
	float cosDiff = DotProduct( vecLookDirection, vecDelta );
	
	if ( flCosHalfFOV <= 0 ) // >180
	{
		// signs are different, answer is implicit
		if ( cosDiff > 0 )
			return true;

		// a/sqrt(b) > c  == a^2 < b * c ^2
		// IFF left and right sides are <= 0
		float flLen2 = vecDelta.LengthSqr();
		return ( cosDiff * cosDiff <= flLen2 * flCosHalfFOV * flCosHalfFOV );
	}
	else // flCosHalfFOV > 0
	{
		// signs are different, answer is implicit
		if ( cosDiff < 0 )
			return false;

		// a/sqrt(b) > c  == a^2 > b * c ^2
		// IFF left and right sides are >= 0
		float flLen2 = vecDelta.LengthSqr();
		return ( cosDiff * cosDiff >= flLen2 * flCosHalfFOV * flCosHalfFOV );
	}
}


#ifndef VECTOR_NO_SLOW_OPERATIONS

// Cross product
Vector CrossProduct( const Vector& a, const Vector& b );

// Random vector creation
Vector RandomVector( vec_t minVal, vec_t maxVal );

#endif

//float RandomVectorInUnitSphere( Vector *pVector );
//float RandomVectorInUnitCircle( Vector2D *pVector );


//-----------------------------------------------------------------------------
//
// Inlined Vector methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline Vector::Vector(void)									
{ 
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

inline Vector::Vector(vec_t X, vec_t Y, vec_t Z)						
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}

//inline Vector::Vector(const float *pFloat)					
//{
//	Assert( pFloat );
//	x = pFloat[0]; y = pFloat[1]; z = pFloat[2];	
//	CHECK_VALID(*this);
//} 

#if 0
//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline Vector::Vector(const Vector &vOther)					
{ 
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
}
#endif

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

inline void Vector::Init( vec_t ix, vec_t iy, vec_t iz )    
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}

/*
inline void Vector::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	z = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	CHECK_VALID(*this);
}
*/

// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
inline void Vector::Zero()
{
	x = y = z = 0.0f;
}

inline void VectorClear( Vector& a )
{
	a.x = a.y = a.z = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector& Vector::operator=(const Vector &vOther)	
{
	CHECK_VALID(vOther);
	x=vOther.x; y=vOther.y; z=vOther.z; 
	return *this; 
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Vector::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

inline vec_t Vector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline vec_t* Vector::Base()
{
	return (vec_t*)this;
}

inline vec_t const* Vector::Base() const
{
	return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// Cast to Vector2D...
//-----------------------------------------------------------------------------

//inline Vector2D& Vector::AsVector2D()
//{
//	return *(Vector2D*)this;
//}

//inline const Vector2D& Vector::AsVector2D() const
//{
//	return *(const Vector2D*)this;
//}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

inline bool Vector::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

inline void Vector::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector::operator==( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool Vector::operator!=( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void VectorCopy( const Vector& src, Vector& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline void	Vector::CopyToArray(float* rgfl) const		
{ 
	Assert( rgfl );
	CHECK_VALID(*this);
	rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; 
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
// #pragma message("TODO: these should be SSE")

inline void Vector::Negate()
{ 
	CHECK_VALID(*this);
	x = -x; y = -y; z = -z; 
} 

inline  Vector& Vector::operator+=(const Vector& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}

inline  Vector& Vector::operator-=(const Vector& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}

inline  Vector& Vector::operator*=(float fl)	
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

inline  Vector& Vector::operator*=(const Vector& v)	
{ 
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}

// this ought to be an opcode.
inline Vector&	Vector::operator+=(float fl) 
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}

inline Vector&	Vector::operator-=(float fl) 
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}



inline  Vector& Vector::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

inline  Vector& Vector::operator/=(const Vector& v)	
{ 
	CHECK_VALID(v);
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}


#if 0
//-----------------------------------------------------------------------------
//
// Inlined Short Vector methods
//
//-----------------------------------------------------------------------------


inline void ShortVector::Init( short ix, short iy, short iz, short iw )    
{ 
	x = ix; y = iy; z = iz; w = iw;
}

inline void ShortVector::Set( const ShortVector& vOther )
{
   x = vOther.x;
   y = vOther.y;
   z = vOther.z;
   w = vOther.w;
}

inline void ShortVector::Set( const short ix, const short iy, const short iz, const short iw )
{
   x = ix;
   y = iy;
   z = iz;
   w = iw;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline short ShortVector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((short*)this)[i];
}

inline short& ShortVector::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((short*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline short* ShortVector::Base()
{
	return (short*)this;
}

inline short const* ShortVector::Base() const
{
	return (short const*)this;
}


//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool ShortVector::operator==( const ShortVector& src ) const
{
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

inline bool ShortVector::operator!=( const ShortVector& src ) const
{
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}



//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline  ShortVector& ShortVector::operator+=(const ShortVector& v)	
{ 
	x+=v.x; y+=v.y; z += v.z; w += v.w;
	return *this;
}

inline  ShortVector& ShortVector::operator-=(const ShortVector& v)	
{ 
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

inline  ShortVector& ShortVector::operator*=(float fl)	
{
	x = (short)(x * fl);
	y = (short)(y * fl);
	z = (short)(z * fl);
	w = (short)(w * fl);
	return *this;
}

inline  ShortVector& ShortVector::operator*=(const ShortVector& v)	
{ 
	x = (short)(x * v.x);
	y = (short)(y * v.y);
	z = (short)(z * v.z);
	w = (short)(w * v.w);
	return *this;
}

inline  ShortVector& ShortVector::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x = (short)(x * oofl);
	y = (short)(y * oofl);
	z = (short)(z * oofl);
	w = (short)(w * oofl);
	return *this;
}

inline  ShortVector& ShortVector::operator/=(const ShortVector& v)	
{ 
	Assert( v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0 );
	x = (short)(x / v.x);
	y = (short)(y / v.y);
	z = (short)(z / v.z);
	w = (short)(w / v.w);
	return *this;
}

inline void ShortVectorMultiply( const ShortVector& src, float fl, ShortVector& res )
{
	Assert( IsFinite(fl) );
	res.x = (short)(src.x * fl);
	res.y = (short)(src.y * fl);
	res.z = (short)(src.z * fl);
	res.w = (short)(src.w * fl);
}

inline ShortVector ShortVector::operator*(float fl) const
{ 
	ShortVector res;
	ShortVectorMultiply( *this, fl, res );
	return res;	
}

#endif



#if 0
//-----------------------------------------------------------------------------
//
// Inlined Integer Vector methods
//
//-----------------------------------------------------------------------------


inline void IntVector4D::Init( int ix, int iy, int iz, int iw )    
{ 
	x = ix; y = iy; z = iz; w = iw;
}

inline void IntVector4D::Set( const IntVector4D& vOther )
{
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	w = vOther.w;
}

inline void IntVector4D::Set( const int ix, const int iy, const int iz, const int iw )
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline int IntVector4D::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((int*)this)[i];
}

inline int& IntVector4D::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((int*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline int* IntVector4D::Base()
{
	return (int*)this;
}

inline int const* IntVector4D::Base() const
{
	return (int const*)this;
}


//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool IntVector4D::operator==( const IntVector4D& src ) const
{
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

inline bool IntVector4D::operator!=( const IntVector4D& src ) const
{
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}



//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline  IntVector4D& IntVector4D::operator+=(const IntVector4D& v)	
{ 
	x+=v.x; y+=v.y; z += v.z; w += v.w;
	return *this;
}

inline  IntVector4D& IntVector4D::operator-=(const IntVector4D& v)	
{ 
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

inline  IntVector4D& IntVector4D::operator*=(float fl)	
{
	x = (int)(x * fl);
	y = (int)(y * fl);
	z = (int)(z * fl);
	w = (int)(w * fl);
	return *this;
}

inline  IntVector4D& IntVector4D::operator*=(const IntVector4D& v)	
{ 
	x = (int)(x * v.x);
	y = (int)(y * v.y);
	z = (int)(z * v.z);
	w = (int)(w * v.w);
	return *this;
}

inline  IntVector4D& IntVector4D::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x = (int)(x * oofl);
	y = (int)(y * oofl);
	z = (int)(z * oofl);
	w = (int)(w * oofl);
	return *this;
}

inline  IntVector4D& IntVector4D::operator/=(const IntVector4D& v)	
{ 
	Assert( v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0 );
	x = (int)(x / v.x);
	y = (int)(y / v.y);
	z = (int)(z / v.z);
	w = (int)(w / v.w);
	return *this;
}

inline void IntVector4DMultiply( const IntVector4D& src, float fl, IntVector4D& res )
{
	Assert( IsFinite(fl) );
	res.x = (int)(src.x * fl);
	res.y = (int)(src.y * fl);
	res.z = (int)(src.z * fl);
	res.w = (int)(src.w * fl);
}

inline IntVector4D IntVector4D::operator*(float fl) const
{ 
	IntVector4D res;
	IntVector4DMultiply( *this, fl, res );
	return res;	
}

#endif

// =======================


inline void VectorAdd( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

inline void VectorMultiply( const Vector& a, vec_t b, Vector& c )
{
	CHECK_VALID(a);
	Assert( IsFinite(b) );
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

inline void VectorMultiply( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}

inline void VectorDivide( const Vector& a, vec_t b, Vector& c )
{
	CHECK_VALID(a);
	Assert( b != 0.0f );
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
}

inline void VectorDivide( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	Assert( (b.x != 0.0f) && (b.y != 0.0f) && (b.z != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
}

// FIXME: Remove
// For backwards compatability
inline void	Vector::MulAdd(const Vector& a, const Vector& b, float scalar)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
}

inline void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest )
{
	CHECK_VALID(src1);
	CHECK_VALID(src2);
	dest.x = src1.x + (src2.x - src1.x) * t;
	dest.y = src1.y + (src2.y - src1.y) * t;
	dest.z = src1.z + (src2.z - src1.z) * t;
}

inline Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t )
{
	Vector result;
	VectorLerp( src1, src2, t, result );
	return result;
}

#if 0
//-----------------------------------------------------------------------------
// Temporary storage for vector results so const Vector& results can be returned
//-----------------------------------------------------------------------------
inline Vector &AllocTempVector()
{
	static Vector s_vecTemp[128];
	static CInterlockedInt s_nIndex;

	int nIndex;
	for (;;)
	{
		int nOldIndex = s_nIndex;
		nIndex = ( (nOldIndex + 0x10001) & 0x7F );

		if ( s_nIndex.AssignIf( nOldIndex, nIndex ) )
		{
			break;
		}
		ThreadPause();
	} 
	return s_vecTemp[nIndex & 0xffff];
}
#endif



//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline vec_t DotProduct(const Vector& a, const Vector& b) 
{ 
	CHECK_VALID(a);
	CHECK_VALID(b);
	return( a.x*b.x + a.y*b.y + a.z*b.z ); 
}

// for backwards compatability
inline vec_t Vector::Dot( const Vector& vOther ) const
{
	CHECK_VALID(vOther);
	return DotProduct( *this, vOther );
}

inline int Vector::LargestComponent() const
{
	float flAbsx = fabs(x);
	float flAbsy = fabs(y);
	float flAbsz = fabs(z);
	if ( flAbsx > flAbsy )
	{
		if ( flAbsx > flAbsz )
			return X_INDEX;
		return Z_INDEX;
	}
	if ( flAbsy > flAbsz )
		return Y_INDEX;
	return Z_INDEX;
}

inline void CrossProduct(const Vector& a, const Vector& b, Vector& result )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	Assert( &a != &result );
	Assert( &b != &result );
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
}

inline vec_t DotProductAbs( const Vector &v0, const Vector &v1 )
{
	CHECK_VALID(v0);
	CHECK_VALID(v1);
	return FloatMakePositive(v0.x*v1.x) + FloatMakePositive(v0.y*v1.y) + FloatMakePositive(v0.z*v1.z);
}

inline vec_t DotProductAbs( const Vector &v0, const float *v1 )
{
	return FloatMakePositive(v0.x * v1[0]) + FloatMakePositive(v0.y * v1[1]) + FloatMakePositive(v0.z * v1[2]);
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------

inline vec_t VectorLength( const Vector& v )
{
	CHECK_VALID(v);
	return (vec_t)FastSqrt(v.x*v.x + v.y*v.y + v.z*v.z);		
}


inline vec_t Vector::Length(void) const	
{
	CHECK_VALID(*this);
	return VectorLength( *this );
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

/*
// FIXME: Can't use until we're un-macroed in mathlib.h
inline vec_t VectorNormalize( Vector& v )
{
	Assert( v.IsValid() );
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		// FIXME: 
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
*/


// check a point against a box
bool Vector::WithinAABox( Vector const &boxmin, Vector const &boxmax)
{
	return ( 
		( x >= boxmin.x ) && ( x <= boxmax.x) &&
		( y >= boxmin.y ) && ( y <= boxmax.y) &&
		( z >= boxmin.z ) && ( z <= boxmax.z)
		);
}

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------
inline vec_t Vector::DistTo(const Vector &vOther) const
{
	Vector delta;
	VectorSubtract( *this, vOther, delta );
	return delta.Length();
}


//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------
inline bool VectorsAreEqual( const Vector& src1, const Vector& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	return (FloatMakePositive(src1.z - src2.z) <= tolerance);
}


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
inline void ComputeClosestPoint( const Vector& vecStart, float flMaxDist, const Vector& vecTarget, Vector *pResult )
{
	Vector vecDelta;
	VectorSubtract( vecTarget, vecStart, vecDelta );
	float flDistSqr = vecDelta.LengthSqr();
	if ( flDistSqr <= flMaxDist * flMaxDist )
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt( flDistSqr );
		vecDelta *= flMaxDist;
		VectorAdd( vecStart, vecDelta, *pResult );
	}
}


//-----------------------------------------------------------------------------
// Takes the absolute value of a vector
//-----------------------------------------------------------------------------
inline void VectorAbs( const Vector& src, Vector& dst )
{
	dst.x = FloatMakePositive(src.x);
	dst.y = FloatMakePositive(src.y);
	dst.z = FloatMakePositive(src.z);
}


//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------
inline Vector Vector::Min(const Vector &vOther) const
{
	return Vector(x < vOther.x ? x : vOther.x, 
		y < vOther.y ? y : vOther.y, 
		z < vOther.z ? z : vOther.z);
}

inline Vector Vector::Max(const Vector &vOther) const
{
	return Vector(x > vOther.x ? x : vOther.x, 
		y > vOther.y ? y : vOther.y, 
		z > vOther.z ? z : vOther.z);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

inline Vector Vector::operator-(void) const
{ 
	return Vector(-x,-y,-z);				
}

inline Vector Vector::operator+(const Vector& v) const	
{ 
	Vector res;
	VectorAdd( *this, v, res );
	return res;	
}

inline Vector Vector::operator-(const Vector& v) const	
{ 
	Vector res;
	VectorSubtract( *this, v, res );
	return res;	
}

inline Vector Vector::operator*(float fl) const	
{ 
	Vector res;
	VectorMultiply( *this, fl, res );
	return res;	
}

inline Vector Vector::operator*(const Vector& v) const	
{ 
	Vector res;
	VectorMultiply( *this, v, res );
	return res;	
}

inline Vector Vector::operator/(float fl) const	
{ 
	Vector res;
	VectorDivide( *this, fl, res );
	return res;	
}

inline Vector Vector::operator/(const Vector& v) const	
{ 
	Vector res;
	VectorDivide( *this, v, res );
	return res;	
}

inline Vector operator*(float fl, const Vector& v)	
{ 
	return v * fl; 
}

//-----------------------------------------------------------------------------
// cross product
//-----------------------------------------------------------------------------

inline Vector Vector::Cross(const Vector& vOther) const
{ 
	Vector res;
	CrossProduct( *this, vOther, res );
	return res;
}

//-----------------------------------------------------------------------------
// 2D
//-----------------------------------------------------------------------------

inline vec_t Vector::Length2D(void) const
{ 
	return (vec_t)FastSqrt(x*x + y*y); 
}

inline vec_t Vector::Length2DSqr(void) const
{ 
	return (x*x + y*y); 
}

inline Vector CrossProduct(const Vector& a, const Vector& b) 
{ 
	return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); 
}

inline void VectorMin( const Vector &a, const Vector &b, Vector &result )
{
	result.x = fpmin(a.x, b.x);
	result.y = fpmin(a.y, b.y);
	result.z = fpmin(a.z, b.z);
}

inline void VectorMax( const Vector &a, const Vector &b, Vector &result )
{
	result.x = fpmax(a.x, b.x);
	result.y = fpmax(a.y, b.y);
	result.z = fpmax(a.z, b.z);
}

inline float ComputeVolume( const Vector &vecMins, const Vector &vecMaxs )
{
	Vector vecDelta;
	VectorSubtract( vecMaxs, vecMins, vecDelta );
	return DotProduct( vecDelta, vecDelta );
}

// Get a random vector.
inline Vector RandomVector( float minVal, float maxVal )
{
	Vector random;
	random.Random( minVal, maxVal );
	return random;
}

#endif //slow

//-----------------------------------------------------------------------------
// Helper debugging stuff....
//-----------------------------------------------------------------------------

inline bool operator==( float const* f, const Vector& v )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

inline bool operator==( const Vector& v, float const* f )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

inline bool operator!=( float const* f, const Vector& v )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

inline bool operator!=( const Vector& v, float const* f )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}


// return a vector perpendicular to another, with smooth variation. The difference between this and
// something like VectorVectors is that there are now discontinuities. _unlike_ VectorVectors,
// you won't get an "u
void VectorPerpendicularToVector( Vector const &in, Vector *pvecOut );

//-----------------------------------------------------------------------------
// AngularImpulse
//-----------------------------------------------------------------------------
// AngularImpulse are exponetial maps (an axis scaled by a "twist" angle in degrees)
typedef Vector AngularImpulse;

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline AngularImpulse RandomAngularImpulse( float minVal, float maxVal )
{
	AngularImpulse	angImp;
	angImp.Random( minVal, maxVal );
	return angImp;
}

#endif


//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------

class RadianEuler;

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void)	{ 
	
	// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

	inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f)	{ x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const;
	void Invalidate();

	bool operator==( const Quaternion &src ) const;
	bool operator!=( const Quaternion &src ) const;

	vec_t* Base() { return (vec_t*)this; }
	const vec_t* Base() const { return (vec_t*)this; }

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z, w;
};


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Quaternion::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

inline vec_t Quaternion::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------
inline bool Quaternion::operator==( const Quaternion &src ) const
{
	return ( x == src.x ) && ( y == src.y ) && ( z == src.z ) && ( w == src.w );
}

inline bool Quaternion::operator!=( const Quaternion &src ) const
{
	return !operator==( src );
}


//-----------------------------------------------------------------------------
// Quaternion equality with tolerance
//-----------------------------------------------------------------------------
inline bool QuaternionsAreEqual( const Quaternion& src1, const Quaternion& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	if (FloatMakePositive(src1.z - src2.z) > tolerance)
		return false;
	return (FloatMakePositive(src1.w - src2.w) <= tolerance);
}


#if 0
//-----------------------------------------------------------------------------
// Here's where we add all those lovely SSE optimized routines
//-----------------------------------------------------------------------------
class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	inline QuaternionAligned(void) {};
	inline QuaternionAligned(vec_t X, vec_t Y, vec_t Z, vec_t W) 
	{
		Init(X,Y,Z,W);
	}

	operator Quaternion * () { return this; } 
	operator const Quaternion * () { return this; } 

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	QuaternionAligned(const QuaternionAligned& vOther);
	QuaternionAligned(const Quaternion &vOther);

#else
public:
	explicit QuaternionAligned(const Quaternion &vOther) 
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
	}

	QuaternionAligned& operator=(const Quaternion &vOther)	
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
		return *this;
	}

	QuaternionAligned& operator=(const QuaternionAligned &vOther)
	{
		// we know we're aligned, so use simd
		// we can't use the convenient abstract interface coz it gets declared later
#ifdef _X360
		XMStoreVector4A(Base(), XMLoadVector4A(vOther.Base()));
#elif _WIN32
		_mm_store_ps(Base(), _mm_load_ps( vOther.Base() ));
#else
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
#endif
		return *this;
	}

#endif

	void* operator new[] ( size_t nSize)
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}

	void* operator new[] ( size_t nSize, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void* operator new[] ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void operator delete[] ( void* p) 
	{
		MemAlloc_FreeAligned(p,true);
	}

	void operator delete[] ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete[] ( void* p, int /*nBlockUse*/, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	// please don't allocate a single quaternion...
	void* operator new   ( size_t nSize )
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}
	void* operator new   ( size_t nSize, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void* operator new   ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAligned(nSize, 16);
		//return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void operator delete ( void* p) 
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p);
	}

	void operator delete ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete ( void* p, int /*nBlockUse*/, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p,true);
		//MemAlloc_FreeAligned(p, pFileName, nLine);
	}
} ALIGN16_POST;

#endif

//-----------------------------------------------------------------------------
// Radian Euler angle aligned to axis (NOT ROLL/PITCH/YAW)
//-----------------------------------------------------------------------------
class QAngle;
class RadianEuler
{
public:
	inline RadianEuler(void)							{ }
	inline RadianEuler(vec_t X, vec_t Y, vec_t Z)		{ x = X; y = Y; z = Z; }
	inline RadianEuler(Quaternion const &q);	// evil auto type promotion!!!
	inline RadianEuler(QAngle const &angles);	// evil auto type promotion!!!

	// Initialization
	inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f)	{ x = ix; y = iy; z = iz; }

	//	conversion to qangle
	QAngle ToQAngle( void ) const;
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};


extern void AngleQuaternion( RadianEuler const &angles, Quaternion &qt );
extern void QuaternionAngles( Quaternion const &q, RadianEuler &angles );
inline Quaternion::Quaternion(RadianEuler const &angle)
{
	AngleQuaternion( angle, *this );
}

inline bool Quaternion::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
}

inline void Quaternion::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = w = VEC_T_NAN;
//#endif
//#endif
}

inline RadianEuler::RadianEuler(Quaternion const &q)
{
	QuaternionAngles( q, *this );
}

inline void VectorCopy( RadianEuler const& src, RadianEuler &dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline bool RadianEuler::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

inline void RadianEuler::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& RadianEuler::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

inline vec_t RadianEuler::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Degree Euler QAngle pitch, yaw, roll
//-----------------------------------------------------------------------------
class QAngleByValue;

class QAngle					
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction
	QAngle(void);
	QAngle(vec_t X, vec_t Y, vec_t Z);
//	QAngle(RadianEuler const &angles);	// evil auto type promotion!!!

	// Allow pass-by-value
	operator QAngleByValue &()				{ return *((QAngleByValue *)(this)); }
	operator const QAngleByValue &() const	{ return *((const QAngleByValue *)(this)); }

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f);
	void Random( vec_t minVal, vec_t maxVal );

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;
	
	// equality
	bool operator==(const QAngle& v) const;
	bool operator!=(const QAngle& v) const;	

	// arithmetic operations
	QAngle&	operator+=(const QAngle &v);
	QAngle&	operator-=(const QAngle &v);
	QAngle&	operator*=(float s);
	QAngle&	operator/=(float s);

	// Get the vector's magnitude.
	vec_t	Length() const;
	vec_t	LengthSqr() const;

	// negate the QAngle components
	//void	Negate(); 

	// No assignment operators either...
	QAngle& operator=( const QAngle& src );

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors

	// arithmetic operations
	QAngle	operator-(void) const;
	
	QAngle	operator+(const QAngle& v) const;
	QAngle	operator-(const QAngle& v) const;
	QAngle	operator*(float fl) const;
	QAngle	operator/(float fl) const;
#else

private:
	// No copy constructors allowed if we're in optimal mode
	QAngle(const QAngle& vOther);

#endif
};

//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class QAngleByValue : public QAngle
{
public:
	// Construction/destruction:
	QAngleByValue(void) : QAngle() {} 
	QAngleByValue(vec_t X, vec_t Y, vec_t Z) : QAngle( X, Y, Z ) {}
	QAngleByValue(const QAngleByValue& vOther) { *this = vOther; }
};


inline void VectorAdd( const QAngle& a, const QAngle& b, QAngle& result )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
}

inline void VectorMA( const QAngle &start, float scale, const QAngle &direction, QAngle &dest )
{
	CHECK_VALID(start);
	CHECK_VALID(direction);
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle(void)									
{ 
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

inline QAngle::QAngle(vec_t X, vec_t Y, vec_t Z)						
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}


//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init( vec_t ix, vec_t iy, vec_t iz )    
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}

/*
inline void QAngle::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	z = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	CHECK_VALID(*this);
}
*/

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline QAngle RandomAngle( float minVal, float maxVal )
{
	Vector random;
	random.Random( minVal, maxVal );
	QAngle ret( random.x, random.y, random.z );
	return ret;
}

#endif


inline RadianEuler::RadianEuler(QAngle const &angles)
{
	Init(
		angles.z * 3.14159265358979323846f / 180.f,
		angles.x * 3.14159265358979323846f / 180.f, 
		angles.y * 3.14159265358979323846f / 180.f );
}




inline QAngle RadianEuler::ToQAngle( void) const
{
	return QAngle(
		y * 180.f / 3.14159265358979323846f,
		z * 180.f / 3.14159265358979323846f,
		x * 180.f / 3.14159265358979323846f );
}


//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=(const QAngle &vOther)	
{
	CHECK_VALID(vOther);
	x=vOther.x; y=vOther.y; z=vOther.z; 
	return *this; 
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& QAngle::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

inline vec_t QAngle::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline vec_t* QAngle::Base()
{
	return (vec_t*)this;
}

inline vec_t const* QAngle::Base() const
{
	return (vec_t const*)this;
}


//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------
inline bool QAngle::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

inline void QAngle::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==( const QAngle& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool QAngle::operator!=( const QAngle& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
inline void VectorCopy( const QAngle& src, QAngle& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}


//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=(const QAngle& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}

inline QAngle& QAngle::operator-=(const QAngle& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}

inline QAngle& QAngle::operator*=(float fl)	
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

inline QAngle& QAngle::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline vec_t QAngle::Length( ) const
{
	CHECK_VALID(*this);
	return (vec_t)FastSqrt( LengthSqr( ) );		
}


inline vec_t QAngle::LengthSqr( ) const
{
	CHECK_VALID(*this);
	return x * x + y * y + z * z;
}
	

//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------
inline bool QAnglesAreEqual( const QAngle& src1, const QAngle& src2, float tolerance = 0.0f )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	return (FloatMakePositive(src1.z - src2.z) <= tolerance);
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline QAngle QAngle::operator-(void) const
{ 
	QAngle ret(-x,-y,-z);
	return ret;
}

inline QAngle QAngle::operator+(const QAngle& v) const	
{ 
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;	
}

inline QAngle QAngle::operator-(const QAngle& v) const	
{ 
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;	
}

inline QAngle QAngle::operator*(float fl) const	
{ 
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;	
}

inline QAngle QAngle::operator/(float fl) const	
{ 
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;	
}

inline QAngle operator*(float fl, const QAngle& v)	
{ 
        QAngle ret( v * fl );
	return ret;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//-----------------------------------------------------------------------------
// NOTE: These are not completely correct.  The representations are not equivalent
// unless the QAngle represents a rotational impulse along a coordinate axis (x,y,z)
inline void QAngleToAngularImpulse( const QAngle &angles, AngularImpulse &impulse )
{
	impulse.x = angles.z;
	impulse.y = angles.x;
	impulse.z = angles.y;
}

inline void AngularImpulseToQAngle( const AngularImpulse &impulse, QAngle &angles )
{
	angles.x = impulse.y;
	angles.y = impulse.z;
	angles.z = impulse.x;
}

#if !defined( _X360 )

inline vec_t InvRSquared( const float* v )
{
	return 1.0 / fpmax( (float)1.0, (float)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) );
}

inline vec_t InvRSquared( const Vector &v )
{
	return InvRSquared( v.Base() );
}

#else

// call directly
inline float _VMX_InvRSquared( const Vector &v )
{
	XMVECTOR xmV = XMVector3ReciprocalLength( XMLoadVector3( v.Base() ) );
	xmV = XMVector3Dot( xmV, xmV );
	return xmV.x;
}

#define InvRSquared(x) _VMX_InvRSquared(x)

#endif // _X360

#if !defined( _X360 )

// FIXME: Change this back to a #define once we get rid of the vec_t version
float VectorNormalize( Vector& v );

// FIXME: Obsolete version of VectorNormalize, once we remove all the friggin float*s
inline float VectorNormalize( float * v )
{
	return VectorNormalize(*(reinterpret_cast<Vector *>(v)));
}

#else

// call directly
inline float _VMX_VectorNormalize( Vector &vec )
{
	float mag = XMVector3Length( XMLoadVector3( vec.Base() ) ).x;
	float den = 1.f / (mag + FLT_EPSILON );
	vec.x *= den;
	vec.y *= den;
	vec.z *= den;
	return mag;
}
// FIXME: Change this back to a #define once we get rid of the vec_t version
inline float VectorNormalize( Vector& v )
{
	return _VMX_VectorNormalize( v );
}
// FIXME: Obsolete version of VectorNormalize, once we remove all the friggin float*s
inline float VectorNormalize( float *pV )
{
	return _VMX_VectorNormalize(*(reinterpret_cast<Vector*>(pV)));
}

#endif // _X360

#if !defined( _X360 )
inline void VectorNormalizeFast (Vector& vec)
{
	float ool = FastRSqrt( FLT_EPSILON + vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );

	vec.x *= ool;
	vec.y *= ool;
	vec.z *= ool;
}
#else

// call directly
inline void VectorNormalizeFast( Vector &vec )
{
	XMVECTOR xmV = XMVector3LengthEst( XMLoadVector3( vec.Base() ) );
	float den = 1.f / (xmV.x + FLT_EPSILON);
	vec.x *= den;
	vec.y *= den;
	vec.z *= den;
}

#endif // _X360

inline vec_t Vector::NormalizeInPlace()
{
	return VectorNormalize( *this );
}

inline Vector Vector::Normalized() const
{
	Vector norm = *this;
	VectorNormalize( norm );
	return norm;
}

inline bool Vector::IsLengthGreaterThan( float val ) const
{
	return LengthSqr() > val*val;
}

inline bool Vector::IsLengthLessThan( float val ) const
{
	return LengthSqr() < val*val;
}


//--------------------------------------------------------------------------------------------------

// forward declarations
class Vector;
// class Vector2D;

//=========================================================
// 4D Vector4D
//=========================================================

class Vector4D					
{
public:
	// Members
	vec_t x, y, z, w;

	// Construction/destruction
	Vector4D(void);
	Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W);
	Vector4D(const float *pFloat);

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f);
	void Init( const Vector& src, vec_t iw=0.0f );

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	inline vec_t* Base();
	inline vec_t const* Base() const;

	// Cast to Vector and Vector2D...
	Vector& AsVector3D();
	Vector const& AsVector3D() const;

	//Vector2D& AsVector2D();
	//Vector2D const& AsVector2D() const;

	// Initialization methods
	void Random( vec_t minVal, vec_t maxVal );

	// equality
	bool operator==(const Vector4D& v) const;
	bool operator!=(const Vector4D& v) const;	

	// arithmetic operations
	Vector4D&	operator+=(const Vector4D &v);			
	Vector4D&	operator-=(const Vector4D &v);		
	Vector4D&	operator*=(const Vector4D &v);			
	Vector4D&	operator*=(float s);
	Vector4D&	operator/=(const Vector4D &v);		
	Vector4D&	operator/=(float s);					

	Vector4D	operator-( void ) const;
	Vector4D	operator*( float fl ) const;
	Vector4D	operator/( float fl ) const;
	Vector4D	operator*( const Vector4D& v ) const;
	Vector4D	operator+( const Vector4D& v ) const;
	Vector4D	operator-( const Vector4D& v ) const;

	// negate the Vector4D components
	void	Negate(); 

	// Get the Vector4D's magnitude.
	vec_t	Length() const;

	// Get the Vector4D's magnitude squared.
	vec_t	LengthSqr(void) const;

	// return true if this vector is (0,0,0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance &&
			w > -tolerance && w < tolerance);
	}

	// Get the distance from this Vector4D to the other one.
	vec_t	DistTo(const Vector4D &vOther) const;

	// Get the distance from this Vector4D to the other one squared.
	vec_t	DistToSqr(const Vector4D &vOther) const;		

	// Copy
	void	CopyToArray(float* rgfl) const;	

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector4D equation (because it's done per-component
	// rather than per-Vector4D).
	void	MulAdd(Vector4D const& a, Vector4D const& b, float scalar);	

	// Dot product.
	vec_t	Dot(Vector4D const& vOther) const;			

	// No copy constructors allowed if we're in optimal mode
#ifdef VECTOR_NO_SLOW_OPERATIONS
private:
#else
public:
#endif
	Vector4D(Vector4D const& vOther);

	// No assignment operators either...
	Vector4D& operator=( Vector4D const& src );
};

const Vector4D vec4_origin( 0.0f, 0.0f, 0.0f, 0.0f );
const Vector4D vec4_invalid( FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX );

#if 0
//-----------------------------------------------------------------------------
// SSE optimized routines
//-----------------------------------------------------------------------------

class ALIGN16 Vector4DAligned : public Vector4D
{
public:
	Vector4DAligned(void) {}
	Vector4DAligned( vec_t X, vec_t Y, vec_t Z, vec_t W );

	inline void Set( vec_t X, vec_t Y, vec_t Z, vec_t W );
	inline void InitZero( void );

	inline __m128 &AsM128() { return *(__m128*)&x; }
	inline const __m128 &AsM128() const { return *(const __m128*)&x; } 

private:
	// No copy constructors allowed if we're in optimal mode
	Vector4DAligned( Vector4DAligned const& vOther );

	// No assignment operators either...
	Vector4DAligned& operator=( Vector4DAligned const& src );
} ALIGN16_POST;

#endif

//-----------------------------------------------------------------------------
// Vector4D related operations
//-----------------------------------------------------------------------------

// Vector4D clear
void Vector4DClear( Vector4D& a );

// Copy
void Vector4DCopy( Vector4D const& src, Vector4D& dst );

// Vector4D arithmetic
void Vector4DAdd( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DSubtract( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DMultiply( Vector4D const& a, vec_t b, Vector4D& result );
void Vector4DMultiply( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DDivide( Vector4D const& a, vec_t b, Vector4D& result );
void Vector4DDivide( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DMA( Vector4D const& start, float s, Vector4D const& dir, Vector4D& result );

// Vector4DAligned arithmetic
//void Vector4DMultiplyAligned( Vector4DAligned const& a, vec_t b, Vector4DAligned& result );


#define Vector4DExpand( v ) (v).x, (v).y, (v).z, (v).w

// Normalization
vec_t Vector4DNormalize( Vector4D& v );

// Length
vec_t Vector4DLength( Vector4D const& v );

// Dot Product
vec_t DotProduct4D(Vector4D const& a, Vector4D const& b);

// Linearly interpolate between two vectors
void Vector4DLerp(Vector4D const& src1, Vector4D const& src2, vec_t t, Vector4D& dest );


//-----------------------------------------------------------------------------
//
// Inlined Vector4D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------

inline Vector4D::Vector4D(void)									
{ 
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	x = y = z = w = VEC_T_NAN;
#endif
}

inline Vector4D::Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

inline Vector4D::Vector4D(const float *pFloat)					
{
	Assert( pFloat );
	x = pFloat[0]; y = pFloat[1]; z = pFloat[2]; w = pFloat[3];	
	Assert( IsValid() );
}


//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline Vector4D::Vector4D(const Vector4D &vOther)					
{ 
	Assert( vOther.IsValid() );
	x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void Vector4D::Init( vec_t ix, vec_t iy, vec_t iz, vec_t iw )
{ 
	x = ix; y = iy; z = iz;	w = iw;
	Assert( IsValid() );
}

inline void Vector4D::Init( const Vector& src, vec_t iw )
{
	x = src.x; y = src.y; z = src.z; w = iw;
	Assert( IsValid() );
}


/*
inline void Vector4D::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + ((vec_t)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((vec_t)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	z = minVal + ((vec_t)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	w = minVal + ((vec_t)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
}
*/

inline void Vector4DClear( Vector4D& a )
{
	a.x = a.y = a.z = a.w = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector4D& Vector4D::operator=(const Vector4D &vOther)	
{
	Assert( vOther.IsValid() );
	x=vOther.x; y=vOther.y; z=vOther.z; w=vOther.w;
	return *this; 
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline vec_t& Vector4D::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

inline vec_t Vector4D::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Cast to Vector and Vector2D...
//-----------------------------------------------------------------------------

inline Vector& Vector4D::AsVector3D()
{
	return *(Vector*)this;
}

inline Vector const& Vector4D::AsVector3D() const
{
	return *(Vector const*)this;
}

//inline Vector2D& Vector4D::AsVector2D()
//{
//	return *(Vector2D*)this;
//}
//
//inline Vector2D const& Vector4D::AsVector2D() const
//{
//	return *(Vector2D const*)this;
//}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline vec_t* Vector4D::Base()
{
	return (vec_t*)this;
}

inline vec_t const* Vector4D::Base() const
{
	return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

inline bool Vector4D::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector4D::operator==( Vector4D const& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

inline bool Vector4D::operator!=( Vector4D const& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void Vector4DCopy( Vector4D const& src, Vector4D& dst )
{
	Assert( src.IsValid() );
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
	dst.w = src.w;
}

inline void	Vector4D::CopyToArray(float* rgfl) const		
{ 
	Assert( IsValid() );
	Assert( rgfl );
	rgfl[0] = x; rgfl[1] = y; rgfl[2] = z; rgfl[3] = w;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void Vector4D::Negate()
{ 
	Assert( IsValid() );
	x = -x; y = -y; z = -z; w = -w;
} 

inline Vector4D& Vector4D::operator+=(const Vector4D& v)	
{ 
	Assert( IsValid() && v.IsValid() );
	x+=v.x; y+=v.y; z += v.z; w += v.w;	
	return *this;
}

inline Vector4D& Vector4D::operator-=(const Vector4D& v)	
{ 
	Assert( IsValid() && v.IsValid() );
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

inline Vector4D& Vector4D::operator*=(float fl)	
{
	x *= fl;
	y *= fl;
	z *= fl;
	w *= fl;
	Assert( IsValid() );
	return *this;
}

inline Vector4D& Vector4D::operator*=(Vector4D const& v)	
{ 
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	Assert( IsValid() );
	return *this;
}

inline Vector4D Vector4D::operator-(void) const
{ 
	return Vector4D(-x,-y,-z,-w);				
}

inline Vector4D Vector4D::operator+(const Vector4D& v) const	
{ 
	Vector4D res;
	Vector4DAdd( *this, v, res );
	return res;	
}

inline Vector4D Vector4D::operator-(const Vector4D& v) const	
{ 
	Vector4D res;
	Vector4DSubtract( *this, v, res );
	return res;	
}


inline Vector4D Vector4D::operator*(float fl) const	
{ 
	Vector4D res;
	Vector4DMultiply( *this, fl, res );
	return res;	
}

inline Vector4D Vector4D::operator*(const Vector4D& v) const	
{ 
	Vector4D res;
	Vector4DMultiply( *this, v, res );
	return res;	
}

inline Vector4D Vector4D::operator/(float fl) const	
{ 
	Vector4D res;
	Vector4DDivide( *this, fl, res );
	return res;	
}

inline Vector4D operator*( float fl, const Vector4D& v )	
{ 
	return v * fl; 
}

inline Vector4D& Vector4D::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	w *= oofl;
	Assert( IsValid() );
	return *this;
}

inline Vector4D& Vector4D::operator/=(Vector4D const& v)	
{ 
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
	Assert( IsValid() );
	return *this;
}

inline void Vector4DAdd( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	c.w = a.w + b.w;
}

inline void Vector4DSubtract( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	c.w = a.w - b.w;
}

inline void Vector4DMultiply( Vector4D const& a, vec_t b, Vector4D& c )
{
	Assert( a.IsValid() && IsFinite(b) );
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
	c.w = a.w * b;
}

inline void Vector4DMultiply( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	c.w = a.w * b.w;
}

inline void Vector4DDivide( Vector4D const& a, vec_t b, Vector4D& c )
{
	Assert( a.IsValid() );
	Assert( b != 0.0f );
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
	c.w = a.w * oob;
}

inline void Vector4DDivide( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() );
	Assert( (b.x != 0.0f) && (b.y != 0.0f) && (b.z != 0.0f) && (b.w != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
	c.w = a.w / b.w;
}

inline void Vector4DMA( Vector4D const& start, float s, Vector4D const& dir, Vector4D& result )
{
	Assert( start.IsValid() && IsFinite(s) && dir.IsValid() );
	result.x = start.x + s*dir.x;
	result.y = start.y + s*dir.y;
	result.z = start.z + s*dir.z;
	result.w = start.w + s*dir.w;
}

// FIXME: Remove
// For backwards compatability
inline void	Vector4D::MulAdd(Vector4D const& a, Vector4D const& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
	w = a.w + b.w * scalar;
}

inline void Vector4DLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest )
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
	dest[2] = src1[2] + (src2[2] - src1[2]) * t;
	dest[3] = src1[3] + (src2[3] - src1[3]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------

inline vec_t DotProduct4D(const Vector4D& a, const Vector4D& b) 
{ 
	Assert( a.IsValid() && b.IsValid() );
	return( a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w ); 
}

// for backwards compatability
inline vec_t Vector4D::Dot( Vector4D const& vOther ) const
{
	return DotProduct4D( *this, vOther );
}


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------

inline vec_t Vector4DLength( Vector4D const& v )
{				   
	Assert( v.IsValid() );
	return (vec_t)FastSqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);		
}

inline vec_t Vector4D::LengthSqr(void) const	
{ 
	Assert( IsValid() );
	return (x*x + y*y + z*z + w*w);		
}

inline vec_t Vector4D::Length(void) const	
{
	return Vector4DLength( *this );
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

// FIXME: Can't use until we're un-macroed in mathlib.h
inline vec_t Vector4DNormalize( Vector4D& v )
{
	Assert( v.IsValid() );
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = v.z = v.w = 0.0f;
	}
	return l;
}

//-----------------------------------------------------------------------------
// Get the distance from this Vector4D to the other one 
//-----------------------------------------------------------------------------

inline vec_t Vector4D::DistTo(const Vector4D &vOther) const
{
	Vector4D delta;
	Vector4DSubtract( *this, vOther, delta );
	return delta.Length();
}

inline vec_t Vector4D::DistToSqr(const Vector4D &vOther) const
{
	Vector4D delta;
	Vector4DSubtract( *this, vOther, delta );
	return delta.LengthSqr();
}


#if 0
//-----------------------------------------------------------------------------
// Vector4DAligned routines
//-----------------------------------------------------------------------------

inline Vector4DAligned::Vector4DAligned( vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

inline void Vector4DAligned::Set( vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

inline void Vector4DAligned::InitZero( void )
{ 
#if !defined( _X360 )
	this->AsM128() = _mm_set1_ps( 0.0f );
#else
	this->AsM128() = __vspltisw( 0 );
#endif
	Assert( IsValid() );
}

inline void Vector4DMultiplyAligned( Vector4DAligned const& a, Vector4DAligned const& b, Vector4DAligned& c )
{
	Assert( a.IsValid() && b.IsValid() );
#if !defined( _X360 )
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	c.w = a.w * b.w;
#else
	c.AsM128() = __vmulfp( a.AsM128(), b.AsM128() );
#endif
}

inline void Vector4DWeightMAD( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB )
{
	Assert( vInA.IsValid() && vInB.IsValid() && IsFinite(w) );

#if !defined( _X360 )
	vOutA.x += vInA.x * w;
	vOutA.y += vInA.y * w;
	vOutA.z += vInA.z * w;
	vOutA.w += vInA.w * w;

	vOutB.x += vInB.x * w;
	vOutB.y += vInB.y * w;
	vOutB.z += vInB.z * w;
	vOutB.w += vInB.w * w;
#else
	__vector4 temp;

	temp = __lvlx( &w, 0 );
	temp = __vspltw( temp, 0 );

	vOutA.AsM128() = __vmaddfp( vInA.AsM128(), temp, vOutA.AsM128() );
	vOutB.AsM128() = __vmaddfp( vInB.AsM128(), temp, vOutB.AsM128() );
#endif
}

inline void Vector4DWeightMADSSE( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB )
{
	Assert( vInA.IsValid() && vInB.IsValid() && IsFinite(w) );

#if !defined( _X360 )
	// Replicate scalar float out to 4 components
	__m128 packed = _mm_set1_ps( w );

	// 4D SSE Vector MAD
	vOutA.AsM128() = _mm_add_ps( vOutA.AsM128(), _mm_mul_ps( vInA.AsM128(), packed ) );
	vOutB.AsM128() = _mm_add_ps( vOutB.AsM128(), _mm_mul_ps( vInB.AsM128(), packed ) );
#else
	__vector4 temp;

	temp = __lvlx( &w, 0 );
	temp = __vspltw( temp, 0 );

	vOutA.AsM128() = __vmaddfp( vInA.AsM128(), temp, vOutA.AsM128() );
	vOutB.AsM128() = __vmaddfp( vInB.AsM128(), temp, vOutB.AsM128() );
#endif
}

#endif

//--------------------------------------------------------------------------------------------------

typedef int SideType;

// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2

#define VP_EPSILON	0.01f


class VPlane
{
public:
				VPlane();
				VPlane(const Vector &vNormal, vec_t dist);

	void		Init(const Vector &vNormal, vec_t dist);

	// Return the distance from the point to the plane.
	vec_t		DistTo(const Vector &vVec) const;

	// Copy.
	VPlane&		operator=(const VPlane &thePlane);

	// Returns SIDE_ON, SIDE_FRONT, or SIDE_BACK.
	// The epsilon for SIDE_ON can be passed in.
	SideType	GetPointSide(const Vector &vPoint, vec_t sideEpsilon=VP_EPSILON) const;

	// Returns SIDE_FRONT or SIDE_BACK.
	SideType	GetPointSideExact(const Vector &vPoint) const;

	// Classify the box with respect to the plane.
	// Returns SIDE_ON, SIDE_FRONT, or SIDE_BACK
	SideType	BoxOnPlaneSide(const Vector &vMin, const Vector &vMax) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Flip the plane.
	VPlane		Flip();

	// Get a point on the plane (normal*dist).
	Vector		GetPointOnPlane() const;

	// Snap the specified point to the plane (along the plane's normal).
	Vector		SnapPointToPlane(const Vector &vPoint) const;
#endif

public:
	Vector		m_Normal;
	vec_t		m_Dist;

#ifdef VECTOR_NO_SLOW_OPERATIONS
private:
	// No copy constructors allowed if we're in optimal mode
	VPlane(const VPlane& vOther);
#endif
};


//-----------------------------------------------------------------------------
// Inlines.
//-----------------------------------------------------------------------------
inline VPlane::VPlane()
{
}

inline VPlane::VPlane(const Vector &vNormal, vec_t dist)
{
	m_Normal = vNormal;
	m_Dist = dist;
}

inline void	VPlane::Init(const Vector &vNormal, vec_t dist)
{
	m_Normal = vNormal;
	m_Dist = dist;
}

inline vec_t VPlane::DistTo(const Vector &vVec) const
{
	return vVec.Dot(m_Normal) - m_Dist;
}

inline VPlane& VPlane::operator=(const VPlane &thePlane)
{
	m_Normal = thePlane.m_Normal;
	m_Dist = thePlane.m_Dist;
	return *this;
}

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline VPlane VPlane::Flip()
{
	return VPlane(-m_Normal, -m_Dist);
}

inline Vector VPlane::GetPointOnPlane() const
{
	return m_Normal * m_Dist;
}

inline Vector VPlane::SnapPointToPlane(const Vector &vPoint) const
{
	return vPoint - m_Normal * DistTo(vPoint);
}

#endif

inline SideType VPlane::GetPointSide(const Vector &vPoint, vec_t sideEpsilon) const
{
	vec_t fDist;

	fDist = DistTo(vPoint);
	if(fDist >= sideEpsilon)
		return SIDE_FRONT;
	else if(fDist <= -sideEpsilon)
		return SIDE_BACK;
	else
		return SIDE_ON;
}

inline SideType VPlane::GetPointSideExact(const Vector &vPoint) const
{
	return DistTo(vPoint) > 0.0f ? SIDE_FRONT : SIDE_BACK;
}


// BUGBUG: This should either simply use the implementation in mathlib or cease to exist.
// mathlib implementation is much more efficient.  Check to see that VPlane isn't used in
// performance critical code.
inline SideType VPlane::BoxOnPlaneSide(const Vector &vMin, const Vector &vMax) const
{
	int i, firstSide, side;
	TableVector vPoints[8] = 
	{
		{ vMin.x, vMin.y, vMin.z },
		{ vMin.x, vMin.y, vMax.z },
		{ vMin.x, vMax.y, vMax.z },
		{ vMin.x, vMax.y, vMin.z },

		{ vMax.x, vMin.y, vMin.z },
		{ vMax.x, vMin.y, vMax.z },
		{ vMax.x, vMax.y, vMax.z },
		{ vMax.x, vMax.y, vMin.z },
	};

	firstSide = GetPointSideExact(vPoints[0]);
	for(i=1; i < 8; i++)
	{
		side = GetPointSideExact(vPoints[i]);

		// Does the box cross the plane?
		if(side != firstSide)
			return SIDE_ON;
	}

	// Ok, they're all on the same side, return that.
	return firstSide;
}

//--------------------------------------------------------------------------------------------------


//struct cplane_t;

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t( 
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin )
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void Invalidate( void )
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_flMatVal[i][j] = VEC_T_NAN;
			}
		}
	}

	float *operator[]( int i )				{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	const float *operator[]( int i ) const	{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	float *Base()							{ return &m_flMatVal[0][0]; }
	const float *Base() const				{ return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};



class VMatrix
{
public:

	VMatrix();
	VMatrix(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
		);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix( const Vector& forward, const Vector& left, const Vector& up );
	
	// Construct from a 3x4 matrix
	VMatrix( const matrix3x4_t& matrix3x4 );

	// Set the values in the matrix.
	void		Init( 
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33 
		);


	// Initialize from a 3x4
	void		Init( const matrix3x4_t& matrix3x4 );

	// array access
	inline float* operator[](int i)
	{ 
		return m[i]; 
	}

	inline const float* operator[](int i) const
	{ 
		return m[i]; 
	}

	// Get a pointer to m[0][0]
	inline float *Base()
	{
		return &m[0][0];
	}

	inline const float *Base() const
	{
		return &m[0][0];
	}

	void		SetLeft(const Vector &vLeft);
	void		SetUp(const Vector &vUp);
	void		SetForward(const Vector &vForward);

	void		GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const;
	void		SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp);

	// Get/set the translation.
	Vector &	GetTranslation( Vector &vTrans ) const;
	void		SetTranslation(const Vector &vTrans);

	void		PreTranslate(const Vector &vTrans);
	void		PostTranslate(const Vector &vTrans);

	matrix3x4_t& As3x4();
	const matrix3x4_t& As3x4() const;
	void		CopyFrom3x4( const matrix3x4_t &m3x4 );
	void		Set3x4( matrix3x4_t& matrix3x4 ) const;

	bool		operator==( const VMatrix& src ) const;
	bool		operator!=( const VMatrix& src ) const { return !( *this == src ); }

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Access the basis vectors.
	Vector		GetLeft() const;
	Vector		GetUp() const;
	Vector		GetForward() const;
	Vector		GetTranslation() const;
#endif


// Matrix->vector operations.
public:
	// Multiply by a 3D vector (same as operator*).
	void		V3Mul(const Vector &vIn, Vector &vOut) const;

	// Multiply by a 4D vector.
	void		V4Mul(const Vector4D &vIn, Vector4D &vOut) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	Vector		ApplyRotation(const Vector &vVec) const;

	// Multiply by a vector (divides by w, assumes input w is 1).
	Vector		operator*(const Vector &vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	Vector		VMul3x3(const Vector &vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	Vector		VMul3x3Transpose(const Vector &vVec) const;

	// Multiply by the upper 3 rows.
	Vector		VMul4x3(const Vector &vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	Vector		VMul4x3Transpose(const Vector &vVec) const;
#endif


// Matrix->plane operations.
public:
	// Transform the plane. The matrix can only contain translation and rotation.
	void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Just calls TransformPlane and returns the result.
	VPlane		operator*(const VPlane &thePlane) const;
#endif

// Matrix->matrix operations.
public:

	VMatrix&	operator=(const VMatrix &mOther);
	
	// Multiply two matrices (out = this * vm).
	void		MatrixMul( const VMatrix &vm, VMatrix &out ) const;

	// Add two matrices.
	const VMatrix& operator+=(const VMatrix &other);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Just calls MatrixMul and returns the result.	
	VMatrix		operator*(const VMatrix &mOther) const;

	// Add/Subtract two matrices.
	VMatrix		operator+(const VMatrix &other) const;
	VMatrix		operator-(const VMatrix &other) const;

	// Negation.
	VMatrix		operator-() const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix		operator~() const;
#endif

// Matrix operations.
public:
	// Set to identity.
	void		Identity();

	bool		IsIdentity() const;

	// Setup a matrix for origin and angles.
	void		SetupMatrixOrgAngles( const Vector &origin, const QAngle &vAngles );
	
	// General inverse. This may fail so check the return!
	bool		InverseGeneral(VMatrix &vInverse) const;
	
	// Does a fast inverse, assuming the matrix only contains translation and rotation.
	void		InverseTR( VMatrix &mRet ) const;

	// Usually used for debug checks. Returns true if the upper 3x3 contains
	// unit vectors and they are all orthogonal.
	bool		IsRotationMatrix() const;
	
#ifndef VECTOR_NO_SLOW_OPERATIONS
	// This calls the other InverseTR and returns the result.
	VMatrix		InverseTR() const;

	// Get the scale of the matrix's basis vectors.
	Vector		GetScale() const;

	// (Fast) multiply by a scaling matrix setup from vScale.
	VMatrix		Scale(const Vector &vScale);	

	// Normalize the basis vectors.
	VMatrix		NormalizeBasisVectors() const;

	// Transpose.
	VMatrix		Transpose() const;

	// Transpose upper-left 3x3.
	VMatrix		Transpose3x3() const;
#endif

public:
	// The matrix.
	vec_t		m[4][4];
};



//-----------------------------------------------------------------------------
// Helper functions.
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

// Setup an identity matrix.
VMatrix		SetupMatrixIdentity();

// Setup as a scaling matrix.
VMatrix		SetupMatrixScale(const Vector &vScale);

// Setup a translation matrix.
VMatrix		SetupMatrixTranslation(const Vector &vTranslation);

// Setup a matrix to reflect around the plane.
VMatrix		SetupMatrixReflection(const VPlane &thePlane);

// Setup a matrix to project from vOrigin onto thePlane.
VMatrix		SetupMatrixProjection(const Vector &vOrigin, const VPlane &thePlane);

// Setup a matrix to rotate the specified amount around the specified axis.
VMatrix		SetupMatrixAxisRot(const Vector &vAxis, vec_t fDegrees);

// Setup a matrix from euler angles. Just sets identity and calls MatrixAngles.
VMatrix		SetupMatrixAngles(const QAngle &vAngles);

// Setup a matrix for origin and angles.
VMatrix		SetupMatrixOrgAngles(const Vector &origin, const QAngle &vAngles);

#endif

#define VMatToString(mat)	(static_cast<const char *>(CFmtStr("[ (%f, %f, %f), (%f, %f, %f), (%f, %f, %f), (%f, %f, %f) ]", mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3] ))) // ** Note: this generates a temporary, don't hold reference!

//-----------------------------------------------------------------------------
// Returns the point at the intersection on the 3 planes.
// Returns false if it can't be solved (2 or more planes are parallel).
//-----------------------------------------------------------------------------
bool PlaneIntersection( const VPlane &vp1, const VPlane &vp2, const VPlane &vp3, Vector &vOut );


//-----------------------------------------------------------------------------
// These methods are faster. Use them if you want faster code
//-----------------------------------------------------------------------------
void MatrixSetIdentity( VMatrix &dst );
void MatrixTranspose( const VMatrix& src, VMatrix& dst );
void MatrixCopy( const VMatrix& src, VMatrix& dst );
void MatrixMultiply( const VMatrix& src1, const VMatrix& src2, VMatrix& dst );

// Accessors
void MatrixGetColumn( const VMatrix &src, int nCol, Vector *pColumn );
void MatrixSetColumn( VMatrix &src, int nCol, const Vector &column );
void MatrixGetRow( const VMatrix &src, int nCol, Vector *pColumn );
void MatrixSetRow( VMatrix &src, int nCol, const Vector &column );

// Vector3DMultiply treats src2 as if it's a direction vector
void Vector3DMultiply( const VMatrix& src1, const Vector& src2, Vector& dst );

// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
inline void Vector3DMultiplyPosition( const VMatrix& src1, const VectorByValue src2, Vector& dst );

// Vector3DMultiplyPositionProjective treats src2 as if it's a point 
// and does the perspective divide at the end
void Vector3DMultiplyPositionProjective( const VMatrix& src1, const Vector &src2, Vector& dst );

// Vector3DMultiplyPosition treats src2 as if it's a direction 
// and does the perspective divide at the end
// NOTE: src1 had better be an inverse transpose to use this correctly
void Vector3DMultiplyProjective( const VMatrix& src1, const Vector &src2, Vector& dst );

void Vector4DMultiply( const VMatrix& src1, const Vector4D& src2, Vector4D& dst );

// Same as Vector4DMultiply except that src2 has an implicit W of 1
void Vector4DMultiplyPosition( const VMatrix& src1, const Vector &src2, Vector4D& dst );

// Multiplies the vector by the transpose of the matrix
void Vector3DMultiplyTranspose( const VMatrix& src1, const Vector& src2, Vector& dst );
void Vector4DMultiplyTranspose( const VMatrix& src1, const Vector4D& src2, Vector4D& dst );

// Transform a plane
// void MatrixTransformPlane( const VMatrix &src, const cplane_t &inPlane, cplane_t &outPlane );

// Transform a plane that has an axis-aligned normal
// void MatrixTransformAxisAlignedPlane( const VMatrix &src, int nDim, float flSign, float flDist, cplane_t &outPlane );

void MatrixBuildTranslation( VMatrix& dst, float x, float y, float z );
void MatrixBuildTranslation( VMatrix& dst, const Vector &translation );

inline void MatrixTranslate( VMatrix& dst, const Vector &translation )
{
	VMatrix matTranslation, temp;
	MatrixBuildTranslation( matTranslation, translation );
	MatrixMultiply( dst, matTranslation, temp );
	dst = temp;
}


void MatrixBuildRotationAboutAxis( VMatrix& dst, const Vector& vAxisOfRot, float angleDegrees );
void MatrixBuildRotateZ( VMatrix& dst, float angleDegrees );

inline void MatrixRotate( VMatrix& dst, const Vector& vAxisOfRot, float angleDegrees )
{
	VMatrix rotation, temp;
	MatrixBuildRotationAboutAxis( rotation, vAxisOfRot, angleDegrees );
	MatrixMultiply( dst, rotation, temp );
	dst = temp;
}

// Builds a rotation matrix that rotates one direction vector into another
void MatrixBuildRotation( VMatrix &dst, const Vector& initialDirection, const Vector& finalDirection );

// Builds a scale matrix
void MatrixBuildScale( VMatrix &dst, float x, float y, float z );
void MatrixBuildScale( VMatrix &dst, const Vector& scale );

// Build a perspective matrix.
// zNear and zFar are assumed to be positive.
// You end up looking down positive Z, X is to the right, Y is up.
// X range: [0..1]
// Y range: [0..1]
// Z range: [0..1]
void MatrixBuildPerspective( VMatrix &dst, float fovX, float fovY, float zNear, float zFar );

//-----------------------------------------------------------------------------
// Given a projection matrix, take the extremes of the space in transformed into world space and
// get a bounding box.
//-----------------------------------------------------------------------------
void CalculateAABBFromProjectionMatrix( const VMatrix &worldToVolume, Vector *pMins, Vector *pMaxs );

//-----------------------------------------------------------------------------
// Given a projection matrix, take the extremes of the space in transformed into world space and
// get a bounding sphere.
//-----------------------------------------------------------------------------
void CalculateSphereFromProjectionMatrix( const VMatrix &worldToVolume, Vector *pCenter, float *pflRadius );

//-----------------------------------------------------------------------------
// Given an inverse projection matrix, take the extremes of the space in transformed into world space and
// get a bounding box.
//-----------------------------------------------------------------------------
void CalculateAABBFromProjectionMatrixInverse( const VMatrix &volumeToWorld, Vector *pMins, Vector *pMaxs );

//-----------------------------------------------------------------------------
// Given an inverse projection matrix, take the extremes of the space in transformed into world space and
// get a bounding sphere.
//-----------------------------------------------------------------------------
void CalculateSphereFromProjectionMatrixInverse( const VMatrix &volumeToWorld, Vector *pCenter, float *pflRadius );

//-----------------------------------------------------------------------------
// Calculate frustum planes given a clip->world space transform.
//-----------------------------------------------------------------------------
// void FrustumPlanesFromMatrix( const VMatrix &clipToWorld, Frustum_t &frustum );

//-----------------------------------------------------------------------------
// Setup a matrix from euler angles. 
//-----------------------------------------------------------------------------
void MatrixFromAngles( const QAngle& vAngles, VMatrix& dst );

//-----------------------------------------------------------------------------
// Creates euler angles from a matrix 
//-----------------------------------------------------------------------------
void MatrixToAngles( const VMatrix& src, QAngle& vAngles );

//-----------------------------------------------------------------------------
// Does a fast inverse, assuming the matrix only contains translation and rotation.
//-----------------------------------------------------------------------------
void MatrixInverseTR( const VMatrix& src, VMatrix &dst );

//-----------------------------------------------------------------------------
// Inverts any matrix at all
//-----------------------------------------------------------------------------
bool MatrixInverseGeneral(const VMatrix& src, VMatrix& dst);

//-----------------------------------------------------------------------------
// Computes the inverse transpose
//-----------------------------------------------------------------------------
void MatrixInverseTranspose( const VMatrix& src, VMatrix& dst );



//-----------------------------------------------------------------------------
// VMatrix inlines.
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix()
{
}

inline VMatrix::VMatrix(
	vec_t m00, vec_t m01, vec_t m02, vec_t m03,
	vec_t m10, vec_t m11, vec_t m12, vec_t m13,
	vec_t m20, vec_t m21, vec_t m22, vec_t m23,
	vec_t m30, vec_t m31, vec_t m32, vec_t m33)
{
	Init(
		m00, m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33
		);
}


inline VMatrix::VMatrix( const matrix3x4_t& matrix3x4 )
{
	Init( matrix3x4 );
}


//-----------------------------------------------------------------------------
// Creates a matrix where the X axis = forward
// the Y axis = left, and the Z axis = up
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis )
{
	Init(
		xAxis.x, yAxis.x, zAxis.x, 0.0f,
		xAxis.y, yAxis.y, zAxis.y, 0.0f,
		xAxis.z, yAxis.z, zAxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}


inline void VMatrix::Init(
	vec_t m00, vec_t m01, vec_t m02, vec_t m03,
	vec_t m10, vec_t m11, vec_t m12, vec_t m13,
	vec_t m20, vec_t m21, vec_t m22, vec_t m23,
	vec_t m30, vec_t m31, vec_t m32, vec_t m33
	)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;

	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;

	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;

	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}


//-----------------------------------------------------------------------------
// Initialize from a 3x4
//-----------------------------------------------------------------------------
inline void VMatrix::Init( const matrix3x4_t& matrix3x4 )
{
	memcpy(m, matrix3x4.Base(), sizeof( matrix3x4_t ) );

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;	
}


//-----------------------------------------------------------------------------
// Methods related to the basis vectors of the matrix
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline Vector VMatrix::GetForward() const
{
	return Vector(m[0][0], m[1][0], m[2][0]);
}

inline Vector VMatrix::GetLeft() const
{
	return Vector(m[0][1], m[1][1], m[2][1]);
}

inline Vector VMatrix::GetUp() const
{
	return Vector(m[0][2], m[1][2], m[2][2]);
}

#endif

inline void VMatrix::SetForward(const Vector &vForward)
{
	m[0][0] = vForward.x;
	m[1][0] = vForward.y;
	m[2][0] = vForward.z;
}

inline void VMatrix::SetLeft(const Vector &vLeft)
{
	m[0][1] = vLeft.x;
	m[1][1] = vLeft.y;
	m[2][1] = vLeft.z;
}

inline void VMatrix::SetUp(const Vector &vUp)
{
	m[0][2] = vUp.x;
	m[1][2] = vUp.y;
	m[2][2] = vUp.z;
}

inline void VMatrix::GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const
{
	vForward.Init( m[0][0], m[1][0], m[2][0] );
	vLeft.Init( m[0][1], m[1][1], m[2][1] );
	vUp.Init( m[0][2], m[1][2], m[2][2] );
}

inline void VMatrix::SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp)
{
	SetForward(vForward);
	SetLeft(vLeft);
	SetUp(vUp);
}


//-----------------------------------------------------------------------------
// Methods related to the translation component of the matrix
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline Vector VMatrix::GetTranslation() const
{
	return Vector(m[0][3], m[1][3], m[2][3]);
}

#endif

inline Vector& VMatrix::GetTranslation( Vector &vTrans ) const
{
	vTrans.x = m[0][3];
	vTrans.y = m[1][3];
	vTrans.z = m[2][3];
	return vTrans;
}

inline void VMatrix::SetTranslation(const Vector &vTrans)
{
	m[0][3] = vTrans.x;
	m[1][3] = vTrans.y;
	m[2][3] = vTrans.z;
}

		  
//-----------------------------------------------------------------------------
// appply translation to this matrix in the input space
//-----------------------------------------------------------------------------
inline void VMatrix::PreTranslate(const Vector &vTrans)
{
	Vector tmp;
	Vector3DMultiplyPosition( *this, vTrans, tmp );
	m[0][3] = tmp.x;
	m[1][3] = tmp.y;
	m[2][3] = tmp.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the output space
//-----------------------------------------------------------------------------
inline void VMatrix::PostTranslate(const Vector &vTrans)
{
	m[0][3] += vTrans.x;
	m[1][3] += vTrans.y;
	m[2][3] += vTrans.z;
}

inline const matrix3x4_t& VMatrix::As3x4() const
{
	return *((const matrix3x4_t*)this);
}

inline matrix3x4_t& VMatrix::As3x4()
{
	return *((matrix3x4_t*)this);
}

inline void VMatrix::CopyFrom3x4( const matrix3x4_t &m3x4 )
{
	memcpy( m, m3x4.Base(), sizeof( matrix3x4_t ) );
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1;
}

inline void	VMatrix::Set3x4( matrix3x4_t& matrix3x4 ) const
{
	memcpy(matrix3x4.Base(), m, sizeof( matrix3x4_t ) );
}


//-----------------------------------------------------------------------------
// Matrix math operations
//-----------------------------------------------------------------------------
inline const VMatrix& VMatrix::operator+=(const VMatrix &other)
{
	for(int i=0; i < 4; i++)
	{
		for(int j=0; j < 4; j++)
		{
			m[i][j] += other.m[i][j];
		}
	}

	return *this;
}


#ifndef VECTOR_NO_SLOW_OPERATIONS

inline VMatrix VMatrix::operator+(const VMatrix &other) const
{
	VMatrix ret;
	for(int i=0; i < 16; i++)
	{
		((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
	}
	return ret;
}

inline VMatrix VMatrix::operator-(const VMatrix &other) const
{
	VMatrix ret;

	for(int i=0; i < 4; i++)
	{
		for(int j=0; j < 4; j++)
		{
			ret.m[i][j] = m[i][j] - other.m[i][j];
		}
	}

	return ret;
}

inline VMatrix VMatrix::operator-() const
{
	VMatrix ret;
	for( int i=0; i < 16; i++ )
	{
		((float*)ret.m)[i] = ((float*)m)[i];
	}
	return ret;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//-----------------------------------------------------------------------------
// Vector transformation
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline Vector VMatrix::operator*(const Vector &vVec) const
{
	Vector vRet;
	vRet.x = m[0][0]*vVec.x + m[0][1]*vVec.y + m[0][2]*vVec.z + m[0][3];
	vRet.y = m[1][0]*vVec.x + m[1][1]*vVec.y + m[1][2]*vVec.z + m[1][3];
	vRet.z = m[2][0]*vVec.x + m[2][1]*vVec.y + m[2][2]*vVec.z + m[2][3];

	return vRet;
}

inline Vector VMatrix::VMul4x3(const Vector &vVec) const
{
	Vector vResult;
	Vector3DMultiplyPosition( *this, vVec, vResult );
	return vResult;
}


inline Vector VMatrix::VMul4x3Transpose(const Vector &vVec) const
{
	Vector tmp = vVec;
	tmp.x -= m[0][3];
	tmp.y -= m[1][3];
	tmp.z -= m[2][3];

	return Vector(
		m[0][0]*tmp.x + m[1][0]*tmp.y + m[2][0]*tmp.z,
		m[0][1]*tmp.x + m[1][1]*tmp.y + m[2][1]*tmp.z,
		m[0][2]*tmp.x + m[1][2]*tmp.y + m[2][2]*tmp.z
		);
}

inline Vector VMatrix::VMul3x3(const Vector &vVec) const
{
	return Vector(
		m[0][0]*vVec.x + m[0][1]*vVec.y + m[0][2]*vVec.z,
		m[1][0]*vVec.x + m[1][1]*vVec.y + m[1][2]*vVec.z,
		m[2][0]*vVec.x + m[2][1]*vVec.y + m[2][2]*vVec.z
		);
}

inline Vector VMatrix::VMul3x3Transpose(const Vector &vVec) const
{
	return Vector(
		m[0][0]*vVec.x + m[1][0]*vVec.y + m[2][0]*vVec.z,
		m[0][1]*vVec.x + m[1][1]*vVec.y + m[2][1]*vVec.z,
		m[0][2]*vVec.x + m[1][2]*vVec.y + m[2][2]*vVec.z
		);
}

#endif // VECTOR_NO_SLOW_OPERATIONS


inline void VMatrix::V3Mul(const Vector &vIn, Vector &vOut) const
{
	vec_t rw;

	rw = 1.0f / (m[3][0]*vIn.x + m[3][1]*vIn.y + m[3][2]*vIn.z + m[3][3]);
	vOut.x = (m[0][0]*vIn.x + m[0][1]*vIn.y + m[0][2]*vIn.z + m[0][3]) * rw;
	vOut.y = (m[1][0]*vIn.x + m[1][1]*vIn.y + m[1][2]*vIn.z + m[1][3]) * rw;
	vOut.z = (m[2][0]*vIn.x + m[2][1]*vIn.y + m[2][2]*vIn.z + m[2][3]) * rw;
}

inline void VMatrix::V4Mul(const Vector4D &vIn, Vector4D &vOut) const
{
	vOut[0] = m[0][0]*vIn[0] + m[0][1]*vIn[1] + m[0][2]*vIn[2] + m[0][3]*vIn[3];
	vOut[1] = m[1][0]*vIn[0] + m[1][1]*vIn[1] + m[1][2]*vIn[2] + m[1][3]*vIn[3];
	vOut[2] = m[2][0]*vIn[0] + m[2][1]*vIn[1] + m[2][2]*vIn[2] + m[2][3]*vIn[3];
	vOut[3] = m[3][0]*vIn[0] + m[3][1]*vIn[1] + m[3][2]*vIn[2] + m[3][3]*vIn[3];
}


//-----------------------------------------------------------------------------
// Plane transformation
//-----------------------------------------------------------------------------
inline void	VMatrix::TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const
{
	Vector vTrans;
	Vector3DMultiply( *this, inPlane.m_Normal, outPlane.m_Normal );
	outPlane.m_Dist = inPlane.m_Dist * DotProduct( outPlane.m_Normal, outPlane.m_Normal );
	outPlane.m_Dist += DotProduct( outPlane.m_Normal, GetTranslation( vTrans ) );
}


//-----------------------------------------------------------------------------
// Other random stuff
//-----------------------------------------------------------------------------
inline void VMatrix::Identity()
{
	MatrixSetIdentity( *this );
}


inline bool VMatrix::IsIdentity() const
{
	return 
		m[0][0] == 1.0f && m[0][1] == 0.0f && m[0][2] == 0.0f && m[0][3] == 0.0f &&
		m[1][0] == 0.0f && m[1][1] == 1.0f && m[1][2] == 0.0f && m[1][3] == 0.0f &&
		m[2][0] == 0.0f && m[2][1] == 0.0f && m[2][2] == 1.0f && m[2][3] == 0.0f &&
		m[3][0] == 0.0f && m[3][1] == 0.0f && m[3][2] == 0.0f && m[3][3] == 1.0f;
}

#ifndef VECTOR_NO_SLOW_OPERATIONS

inline Vector VMatrix::ApplyRotation(const Vector &vVec) const
{
	return VMul3x3(vVec);
}

inline VMatrix VMatrix::operator~() const
{
	VMatrix mRet;
	InverseGeneral(mRet);
	return mRet;
}

#endif


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------
inline void MatrixGetColumn( const VMatrix &src, int nCol, Vector *pColumn )
{
	Assert( (nCol >= 0) && (nCol <= 3) );

	pColumn->x = src[0][nCol];
	pColumn->y = src[1][nCol];
	pColumn->z = src[2][nCol];
}

inline void MatrixSetColumn( VMatrix &src, int nCol, const Vector &column )
{
	Assert( (nCol >= 0) && (nCol <= 3) );

	src.m[0][nCol] = column.x;
	src.m[1][nCol] = column.y;
	src.m[2][nCol] = column.z;
}

inline void MatrixGetRow( const VMatrix &src, int nRow, Vector *pRow )
{
	Assert( (nRow >= 0) && (nRow <= 3) );
	*pRow = *(Vector*)src[nRow];
}

inline void MatrixSetRow( VMatrix &dst, int nRow, const Vector &row )
{
	Assert( (nRow >= 0) && (nRow <= 3) );
	*(Vector*)dst[nRow] = row;
}


//-----------------------------------------------------------------------------
// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
//-----------------------------------------------------------------------------
// NJS: src2 is passed in as a full vector rather than a reference to prevent the need
// for 2 branches and a potential copy in the body.  (ie, handling the case when the src2
// reference is the same as the dst reference ).
inline void Vector3DMultiplyPosition( const VMatrix& src1, const VectorByValue src2, Vector& dst )
{
	dst[0] = src1[0][0] * src2.x + src1[0][1] * src2.y + src1[0][2] * src2.z + src1[0][3];
	dst[1] = src1[1][0] * src2.x + src1[1][1] * src2.y + src1[1][2] * src2.z + src1[1][3];
	dst[2] = src1[2][0] * src2.x + src1[2][1] * src2.y + src1[2][2] * src2.z + src1[2][3];
}


#if 0
//-----------------------------------------------------------------------------
// Transform a plane that has an axis-aligned normal
//-----------------------------------------------------------------------------
inline void MatrixTransformAxisAlignedPlane( const VMatrix &src, int nDim, float flSign, float flDist, cplane_t &outPlane )
{
	// See MatrixTransformPlane in the .cpp file for an explanation of the algorithm.
	MatrixGetColumn( src, nDim, &outPlane.normal );
	outPlane.normal *= flSign;
	outPlane.dist = flDist * DotProduct( outPlane.normal, outPlane.normal );

	// NOTE: Writing this out by hand because it doesn't inline (inline depth isn't large enough)
	// This should read outPlane.dist += DotProduct( outPlane.normal, src.GetTranslation );
	outPlane.dist += outPlane.normal.x * src.m[0][3] + outPlane.normal.y * src.m[1][3] + outPlane.normal.z * src.m[2][3];
}
#endif


//-----------------------------------------------------------------------------
// Matrix equality test
//-----------------------------------------------------------------------------
inline bool MatricesAreEqual( const VMatrix &src1, const VMatrix &src2, float flTolerance )
{
	for ( int i = 0; i < 3; ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			if ( fabs( src1[i][j] - src2[i][j] ) > flTolerance )
				return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MatrixBuildOrtho( VMatrix& dst, double left, double top, double right, double bottom, double zNear, double zFar );
void MatrixBuildPerspectiveX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar );
void MatrixBuildPerspectiveOffCenterX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar, double bottom, double top, double left, double right );

inline void MatrixOrtho( VMatrix& dst, double left, double top, double right, double bottom, double zNear, double zFar )
{
	VMatrix mat;
	MatrixBuildOrtho( mat, left, top, right, bottom, zNear, zFar );

	VMatrix temp;
	MatrixMultiply( dst, mat, temp );
	dst = temp;
}

inline void MatrixPerspectiveX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar )
{
	VMatrix mat;
	MatrixBuildPerspectiveX( mat, flFovX, flAspect, flZNear, flZFar );

	VMatrix temp;
	MatrixMultiply( dst, mat, temp );
	dst = temp;
}

inline void MatrixPerspectiveOffCenterX( VMatrix& dst, double flFovX, double flAspect, double flZNear, double flZFar, double bottom, double top, double left, double right )
{
	VMatrix mat;
	MatrixBuildPerspectiveOffCenterX( mat, flFovX, flAspect, flZNear, flZFar, bottom, top, left, right );

	VMatrix temp;
	MatrixMultiply( dst, mat, temp );
	dst = temp;
}

#endif // MATHLITE_H
