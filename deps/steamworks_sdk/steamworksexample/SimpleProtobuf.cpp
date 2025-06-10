//====== Copyright 1996-2014, Valve Corporation, All rights reserved. =======
//
// Purpose: Simple C++ protobuf manipulation routines. For a more advanced,
// fully-featured library, see https://developers.google.com/protocol-buffers/
//
//===========================================================================

#include "SimpleProtobuf.h"


//
// NOTE:
// You should probably be using the official protobuf library instead if you
// have any concerns about how this code works, or are planning to modify it.
//

#define CHECK_OVERRUN( ptr, end, len ) ( end < ptr || (size_t)( end - ptr ) < len )

static void ProtobufEncodeVarInt( std::string& strProtobuf, uint64 ulVarInt )
{
	for ( ; ulVarInt >= 128; ulVarInt >>= 7 )
		strProtobuf.append( 1, ((char)ulVarInt & (char)127) | (char)128 );
	strProtobuf.append( 1, (char)ulVarInt );
}

void ProtobufWriteField_Integer( std::string& strProtobuf, uint32 uFieldNumber, uint64 ulVarIntData )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ) );
	ProtobufEncodeVarInt( strProtobuf, ulVarIntData );
}

void ProtobufWriteField_SInteger( std::string& strProtobuf, uint32 uFieldNumber, int64 ulSwizzleVarIntData )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_SINTEGER( uFieldNumber ) );
	ProtobufEncodeVarInt( strProtobuf, (ulSwizzleVarIntData << 1) ^ (ulSwizzleVarIntData >> 63) );
}

void ProtobufWriteField_Fixed64( std::string& strProtobuf, uint32 uFieldNumber, uint64 ulFixed64Data )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ) );
#ifdef VALVE_BIG_ENDIAN
	ulFixed64Data = QWordSwap( ulFixed64Data );
#endif
	strProtobuf.append( reinterpret_cast<char*>(&ulFixed64Data), 8 );
}

void ProtobufWriteField_Fixed64( std::string& strProtobuf, uint32 uFieldNumber, double flFixed64Data )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ) );
	const char *pData = reinterpret_cast<const char*>(&flFixed64Data);
#ifdef VALVE_BIG_ENDIAN
	strProtobuf.append( std::const_reverse_iterator<const char*>( pData + 8 ), std::const_reverse_iterator<const char*>( pData ) );
#else
	strProtobuf.append( pData, 8 );
#endif
}

void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const char *pchData, size_t cchData )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_STRING( uFieldNumber ) );
	ProtobufEncodeVarInt( strProtobuf, cchData );
	strProtobuf.append( pchData, cchData );
}

void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const std::string &strData )
{
	ProtobufWriteField_String( strProtobuf, uFieldNumber, strData.data(), strData.size() );
}

void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const char *pchData )
{
	ProtobufWriteField_String( strProtobuf, uFieldNumber, pchData, strlen( pchData ) );
}

void ProtobufWriteField_Fixed32( std::string& strProtobuf, uint32 uFieldNumber, uint32 ulFixed32Data )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ) );
#ifdef VALVE_BIG_ENDIAN
	ulFixed32Data = DWordSwap( ulFixed32Data );
#endif
	strProtobuf.append( reinterpret_cast<char*>(&ulFixed32Data), 4 );
}

void ProtobufWriteField_Fixed32( std::string& strProtobuf, uint32 uFieldNumber, float flFixed32Data )
{
	ProtobufEncodeVarInt( strProtobuf, PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ) );
	const char *pData = reinterpret_cast<const char*>(&flFixed32Data);
#ifdef VALVE_BIG_ENDIAN
	strProtobuf.append( std::const_reverse_iterator<const char*>( pData + 4 ), std::const_reverse_iterator<const char*>( pData ) );
#else
	strProtobuf.append( pData, 4 );
#endif
}


static bool ProtobufDecodeVarInt( const char * &pParsePosition, const char *pParseEnd, uint64 &ulVarInt )
{
	const char * pStart = pParsePosition;
	while ( pParsePosition < pParseEnd && (*pParsePosition & 128) )
		++pParsePosition;
	if ( pParsePosition >= pParseEnd )
		return false;
	uint64 v = 0;
	for ( const char *p = pParsePosition++; p >= pStart; --p )
		v = (v << 7) + (*p & 127);
	ulVarInt = v;
	return true;
}

bool ProtobufReadFieldTag( const char * &pParsePosition, const char *pParseEnd, uint32 &uFieldTag )
{
	uint64 v;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, v ) || v == 0 || (v >> 32) != 0 )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	uFieldTag = (uint32)v;
	return true;
}

bool ProtobufSkipFieldValue( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag )
{
	switch ( uFieldTag & 7 )
	{
	case 0: // VARINT
		while ( pParsePosition < pParseEnd )
		{
			char c = *pParsePosition++;
			if ( !( c & 128 ) )
				return true;
		}
		return false;

	case 1: // FIXED64
		if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 8 ) )
		{
			pParsePosition = pParseEnd;
			return false;
		}
		pParsePosition += 8;
		return true;

	case 2: // LENGTH DELIM (string, etc)
	{
		uint64 ulLength = 0;
		if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulLength ) )
			return false;
		if ( CHECK_OVERRUN( pParsePosition, pParseEnd, ulLength ) )
		{
			pParsePosition = pParseEnd;
			return false;
		}
		pParsePosition += ulLength;
		return true;
	}

	case 5: // FIXED32
		if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 4 ) )
		{
			pParsePosition = pParseEnd;
			return false;
		}
		pParsePosition += 4;
		return true;

	default: // UNKNOWN
		pParsePosition = pParseEnd;
		return false;
	}
}

bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, int32 &nValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 4 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	memcpy( &nValue, pParsePosition, 4 );
#ifdef VALVE_BIG_ENDIAN
	nValue = DWordSwap( nValue );
#endif
	pParsePosition += 4;
	return true;
}

bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 &uValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 4 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	memcpy( &uValue, pParsePosition, 4 );
#ifdef VALVE_BIG_ENDIAN
	uValue = DWordSwap( uValue );
#endif
	pParsePosition += 4;
	return true;
}

bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, float &flValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 4 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
#ifdef VALVE_BIG_ENDIAN
	std::copy( std::const_reverse_iterator( pParsePosition + 4 ), std::const_reverse_iterator( pParsePosition ), reinterpret_cast<char *>(&flValue) );
#else
	memcpy( &flValue, pParsePosition, 4 );
#endif
	pParsePosition += 4;
	return true;
}

bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, int64 &lValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 8 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	memcpy( &lValue, pParsePosition, 8 );
#ifdef VALVE_BIG_ENDIAN
	lValue = QWordSwap( lValue );
#endif
	pParsePosition += 8;
	return true;
}

bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, uint64 &ulValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 8 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	memcpy( &ulValue, pParsePosition, 8 );
#ifdef VALVE_BIG_ENDIAN
	ulValue = QWordSwap( ulValue );
#endif
	pParsePosition += 8;
	return true;
}

bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, double &flValue )
{
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, 8 ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
#ifdef VALVE_BIG_ENDIAN
	std::copy( std::const_reverse_iterator( pParsePosition + 8 ), std::const_reverse_iterator( pParsePosition ), reinterpret_cast<char *>(&flValue) );
#else
	memcpy( &flValue, pParsePosition, 8 );
#endif
	pParsePosition += 8;
	return true;
}

bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, uint64 &ulVarInt )
{
	return ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulVarInt );
}

bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, int64 &lVarInt )
{
	return ProtobufDecodeVarInt( pParsePosition, pParseEnd, reinterpret_cast<uint64&>(lVarInt) );
}

bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, uint32 &uVarInt )
{
	uint64 ulVarInt;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulVarInt ) )
		return false;
	uVarInt = (uint32)ulVarInt;
	return (uint64)uVarInt == ulVarInt;
}

bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, int32 &nVarInt )
{
	uint64 ulVarInt;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulVarInt ) )
		return false;
	nVarInt = (int32)ulVarInt;
	return (uint64)(int64)nVarInt == ulVarInt;
}

bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, bool &bVarInt )
{
	uint64 ulVarInt;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulVarInt ) )
		return false;
	bVarInt = ( ulVarInt != 0 );
	return true;
}

bool ProtobufReadSInteger( const char * &pParsePosition, const char *pParseEnd, int64 &lVarInt )
{
	uint64 v;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, v ) )
		return false;
	lVarInt = (int64)(v >> 1) ^ -(int64)(v & 1);
	return true;
}

bool ProtobufReadSInteger( const char * &pParsePosition, const char *pParseEnd, int32 &nVarInt )
{
	uint64 v;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, v ) )
		return false;
	int64 lVarInt = (int64)(v >> 1) ^ -(int64)(v & 1);
	nVarInt = (int32)lVarInt;
	return (int64)nVarInt == lVarInt;
}

bool ProtobufReadString( const char * &pParsePosition, const char *pParseEnd, std::string &strValue )
{
	uint64 ulLength;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulLength ) )
		return false;
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, ulLength ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	strValue.assign( pParsePosition, (size_t)ulLength );
	pParsePosition += ulLength;
	return true;
}

bool ProtobufReadStringAlias( const char * &pParsePosition, const char *pParseEnd, const char * &pStringDataStart, const char * &pStringDataEnd )
{
	uint64 ulLength;
	if ( !ProtobufDecodeVarInt( pParsePosition, pParseEnd, ulLength ) )
		return false;
	if ( CHECK_OVERRUN( pParsePosition, pParseEnd, ulLength ) )
	{
		pParsePosition = pParseEnd;
		return false;
	}
	pStringDataStart = pParsePosition;
	pParsePosition += ulLength;
	pStringDataEnd = pParsePosition;
	return true;
}


template < typename T >
static bool ProtobufReadRepeated_T( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector< T > &vecData, bool( *pfnRead )(const char * &, const char *, T &) )
{
	if ( (uFieldTag & 7) == 2 )
	{
		const char *pStart = NULL, *pEnd = NULL;
		if ( !ProtobufReadStringAlias( pParsePosition, pParseEnd, pStart, pEnd ) )
			return false;

		while ( pStart != pEnd )
		{
			T v;
			if ( !pfnRead( pStart, pEnd, v ) )
				return false;
			vecData.push_back( v );
		}
	}
	else
	{
		T v;
		if ( !pfnRead( pParsePosition, pParseEnd, v ) )
			return false;
		vecData.push_back( v );
	}
	return true;
}

bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint64> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadInteger ); }
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadInteger ); }
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint32> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadInteger ); }
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadInteger ); }
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<bool> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadInteger ); }
bool ProtobufReadRepeatedSInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadSInteger ); }
bool ProtobufReadRepeatedSInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadSInteger ); }
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed32 ); }
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint32> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed32 ); }
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<float> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed32 ); }
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed64 ); }
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint64> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed64 ); }
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<double> &vec ) { return ProtobufReadRepeated_T( pParsePosition, pParseEnd, uFieldTag, vec, &ProtobufReadFixed64 ); }

bool ProtobufReadRepeatedString( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<std::string> &vec )
{
	vec.push_back( std::string() );
	if ( !ProtobufReadString( pParsePosition, pParseEnd, vec.back() ) )
	{
		vec.pop_back();
		return false;
	}
	return true;
}


template < typename T >
static bool ProtobufExtractField_T( const char *pParsePosition, const char *pParseEnd, uint32 uFieldTag, T &value, bool( *pfnRead )(const char * &, const char *, T &) )
{
	uint32 uCurrentTag = 0;
	bool bOK = false;
	while ( ProtobufReadFieldTag( pParsePosition, pParseEnd, uCurrentTag ) )
	{
		if ( uCurrentTag == uFieldTag )
			bOK = pfnRead( pParsePosition, pParseEnd, value );
		else
			ProtobufSkipFieldValue( pParsePosition, pParseEnd, uCurrentTag );
	}
	return bOK;
}

template < typename T >
static bool ProtobufExtractField_T( const char *pParsePosition, const char *pParseEnd, uint32 uFieldTag, T &value, bool( *pfnRead )(const char * &, const char *, uint32, T &) )
{
	uint32 uCurrentTag = 0;
	bool bOK = false;
	while ( ProtobufReadFieldTag( pParsePosition, pParseEnd, uCurrentTag ) )
	{
		if ( uCurrentTag == uFieldTag || uCurrentTag == PROTOBUF_FIELDTAG_STRING( uFieldTag >> 3 ) )
			bOK = pfnRead( pParsePosition, pParseEnd, uCurrentTag, value );
		else
			ProtobufSkipFieldValue( pParsePosition, pParseEnd, uCurrentTag );
	}
	return bOK;
}

bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, uint64 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), value, &ProtobufReadInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, int64 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), value, &ProtobufReadInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, uint32 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), value, &ProtobufReadInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, int32 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), value, &ProtobufReadInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, bool &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), value, &ProtobufReadInteger ); }
bool ProtobufExtractField_SInteger( const std::string &strProtobuf, uint32 uFieldNumber, int64 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_SINTEGER( uFieldNumber ), value, &ProtobufReadSInteger ); }
bool ProtobufExtractField_SInteger( const std::string &strProtobuf, uint32 uFieldNumber, int32 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_SINTEGER( uFieldNumber ), value, &ProtobufReadSInteger ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, int64 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), value, &ProtobufReadFixed64 ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, uint64 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), value, &ProtobufReadFixed64 ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, double &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), value, &ProtobufReadFixed64 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, int32 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), value, &ProtobufReadFixed32 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, uint32 &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), value, &ProtobufReadFixed32 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, float &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), value, &ProtobufReadFixed32 ); }
bool ProtobufExtractField_String( const std::string &strProtobuf, uint32 uFieldNumber, std::string &value ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_STRING( uFieldNumber ), value, &ProtobufReadString ); }

bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<uint64> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int64> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<uint32> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int32> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedInteger ); }
bool ProtobufExtractField_Integer( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<bool> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_INTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedInteger ); }
bool ProtobufExtractField_SInteger( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int64> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_SINTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedSInteger ); }
bool ProtobufExtractField_SInteger( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int32> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_SINTEGER( uFieldNumber ), vec, &ProtobufReadRepeatedSInteger ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int64> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), vec, &ProtobufReadRepeatedFixed64 ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<uint64> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), vec, &ProtobufReadRepeatedFixed64 ); }
bool ProtobufExtractField_Fixed64( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<double> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED64( uFieldNumber ), vec, &ProtobufReadRepeatedFixed64 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<int32> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), vec, &ProtobufReadRepeatedFixed32 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<uint32> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), vec, &ProtobufReadRepeatedFixed32 ); }
bool ProtobufExtractField_Fixed32( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<float> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_FIXED32( uFieldNumber ), vec, &ProtobufReadRepeatedFixed32 ); }
bool ProtobufExtractField_String( const std::string &strProtobuf, uint32 uFieldNumber, std::vector<std::string> &vec ) { return ProtobufExtractField_T( strProtobuf.data(), strProtobuf.data() + strProtobuf.size(), PROTOBUF_FIELDTAG_STRING( uFieldNumber ), vec, &ProtobufReadRepeatedString ); }

