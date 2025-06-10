//====== Copyright 1996-2014, Valve Corporation, All rights reserved. =======
//
// Purpose: Simple C++ protobuf manipulation routines. For a more advanced,
// fully-featured library, see https://developers.google.com/protocol-buffers/
//
//===========================================================================

#ifndef SIMPLEPROTOBUF_H
#define SIMPLEPROTOBUF_H

#pragma once

#include "steam/steamtypes.h"
#include <string>
#include <vector>
#include <string.h>


//
// This file contains some quick-and-dirty helpers that can encode and
// decode the protocol-buffer ("protobuf") serialization format. It's
// a reasonable way to communicate with simple protobuf-based services.
//
// However, if you are doing serious work with protobufs, you should
// take the time to understand and use the official C++ library. It
// provides a tool which copmiles protobuf descriptions directly into
// working C++ classes, which will save you time and effort, and help
// with parsing complicated message types.
//
// https://developers.google.com/protocol-buffers/
//

//
// The protobuf serialization format is a simple field-based encoding;
// a complete protobuf is the unordered concatenation of all its fields.
// Unknown or incorrectly-typed fields are ignored by protobuf parsers.
//
// Details: https://developers.google.com/protocol-buffers/docs/encoding
//
// All protobuf value types use one of these five encodings:
//  Integer:  bool, enum, int32, uint32, int64, uint64
//  SInteger: sint32, sint64
//  Fixed32:  fixed32, float
//  Fixed64:  fixed64, double
//  String:   string, bytes, nested message types
//
// Nested protobufs are built up independently, then encoded as string
// fields in the parent protobuf.
//
// Arrays ("repeated" field types) have two possible encodings: simple
// and packed. This utility file can parse both encodings, but only
// emits simple repeated fields (via multiple ProtobufWriteField calls
// with the same field number, one for every array element).
//

//
// Example usage
//
// If this is the protobuf definition of a message...
//
// message TestMessage {
//   optional uint32 index = 1;
//   optional string text = 2;
//   repeated double number = 3;
//   optional bool flag = 4;
// }
//
// ...then this is how to compose it:
//
// std::string msg;
// ProtobufWriteField_Integer( msg, 1, iIndex );
// ProtobufWriteField_Integer( msg, 4, true );
// ProtobufWriteField_Fixed64( msg, 3, 1.0 );
// ProtobufWriteField_Fixed64( msg, 3, 2.0 );
// ProtobufWriteField_Fixed64( msg, 3, 3.0 );
// ProtobufWriteField_String( msg, 2, "text field" );
//
// ...and this is how to extract individual fields:
//
// std::string strText;
// ProtobufExtractField_String( msg, 2, strText );
//
// ...and this is how to parse it with optimized low-level operations:
//
// bool bFlag = false;
// uint32 uIndex = 0;
// std::string strText;
// std::vector< double > vecNumbers;
//
// const char *pParse = msg.data(), *pEnd = msg.data() + msg.size();
// for ( uint32 uFieldTag = 0; ProtobufReadFieldTag( pParse, pEnd, uFieldTag ); ) {
//   switch ( uFieldTag ) {
//	 case PROTOBUF_FIELDTAG_INTEGER( 1 ): ProtobufReadInteger( pParse, pEnd, iIndex ); break;
//	 case PROTOBUF_FIELDTAG_STRING( 2 ):  ProtobufReadString( pParse, pEnd, strText ); break;
//   case PROTOBUF_FIELDTAG_FIXED64( 3 ): case PROTOBUF_FIELDTAG_REPEATED_FIXED64( 3 ):
//      ProtobufReadRepeatedFixed64( pParse, pEnd, uFieldTag, vecNumbers ); break;
//	 case PROTOBUF_FIELDTAG_STRING( 4 ):  ProtobufReadInteger( pParse, pEnd, bFlag ); break;
//   default: ProtobufSkipFieldValue( pParse, pEnd, uFieldTag ); break;
//   }
// }
//
//
// NOTE: it is important to handle both REPEATED and non-REPEATED cases when
// parsing repeated fields, for all types other than strings! There are two
// different possible encodings for repeated non-string fields, and failing
// to handle both cases can lead to invalid parse results.
//


// Encoding functions
//
// Note: C++ type promotion rules will automatically handle smaller integer types

void ProtobufWriteField_Integer( std::string& strProtobuf, uint32 uFieldNumber, uint64 ulVarIntData );
void ProtobufWriteField_SInteger( std::string& strProtobuf, uint32 uFieldNumber, int64 lSwizzleVarIntData );
void ProtobufWriteField_Fixed64( std::string& strProtobuf, uint32 uFieldNumber, uint64 ulFixed64Data );
void ProtobufWriteField_Fixed64( std::string& strProtobuf, uint32 uFieldNumber, double flFixed64Data );
void ProtobufWriteField_Fixed32( std::string& strProtobuf, uint32 uFieldNumber, uint32 ulFixed32Data );
void ProtobufWriteField_Fixed32( std::string& strProtobuf, uint32 uFieldNumber, float flFixed32Data );
void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const char *pchData, size_t cchData );
void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const char *pchData );
void ProtobufWriteField_String( std::string& strProtobuf, uint32 uFieldNumber, const std::string &strData );

// Decoding functions, high-level (not optimized for speed)
//

bool ProtobufExtractField_Integer( const std::string & strProtobuf, uint32 uFieldNumber, uint64 &ulData );
bool ProtobufExtractField_Integer( const std::string & strProtobuf, uint32 uFieldNumber, int64 &lData );
bool ProtobufExtractField_Integer( const std::string & strProtobuf, uint32 uFieldNumber, uint32 &uData );
bool ProtobufExtractField_Integer( const std::string & strProtobuf, uint32 uFieldNumber, int32 &iData );
bool ProtobufExtractField_Integer( const std::string & strProtobuf, uint32 uFieldNumber, bool &bData );
bool ProtobufExtractField_SInteger( const std::string & strProtobuf, uint32 uFieldNumber, int64 &lData );
bool ProtobufExtractField_SInteger( const std::string & strProtobuf, uint32 uFieldNumber, int32 &lData );
bool ProtobufExtractField_Fixed64( const std::string & strProtobuf, uint32 uFieldNumber, uint64 &ulData );
bool ProtobufExtractField_Fixed64( const std::string & strProtobuf, uint32 uFieldNumber, int64 &lData );
bool ProtobufExtractField_Fixed64( const std::string & strProtobuf, uint32 uFieldNumber, double &flData );
bool ProtobufExtractField_Fixed32( const std::string & strProtobuf, uint32 uFieldNumber, uint32 &uData );
bool ProtobufExtractField_Fixed32( const std::string & strProtobuf, uint32 uFieldNumber, int32 &iData );
bool ProtobufExtractField_Fixed32( const std::string & strProtobuf, uint32 uFieldNumber, float &flData );
bool ProtobufExtractField_String( const std::string & strProtobuf, uint32 uFieldNumber, std::string &strData );

// Decoding functions, low-level (see example usage and important NOTE in comments above)
//

bool ProtobufReadFieldTag( const char * &pParsePosition, const char *pParseEnd, uint32 &uFieldTag );
bool ProtobufSkipFieldValue( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag );
bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, uint64 &ulVarInt );
bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, int64 &lVarInt );
bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, uint32 &uVarInt );
bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, int32 &nVarInt );
bool ProtobufReadInteger( const char * &pParsePosition, const char *pParseEnd, bool &bVarInt );
bool ProtobufReadSInteger( const char * &pParsePosition, const char *pParseEnd, int64 &lVarInt );
bool ProtobufReadSInteger( const char * &pParsePosition, const char *pParseEnd, int32 &nVarInt );
bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, int64 &lValue );
bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, uint64 &ulValue );
bool ProtobufReadFixed64( const char * &pParsePosition, const char *pParseEnd, double &flValue );
bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, int32 &nValue );
bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 &uValue );
bool ProtobufReadFixed32( const char * &pParsePosition, const char *pParseEnd, float &flValue );
bool ProtobufReadString( const char * &pParsePosition, const char *pParseEnd, std::string &strValue );
bool ProtobufReadStringAlias( const char * &pParsePosition, const char *pParseEnd, const char * &pStringDataStart, const char * &pStringDataEnd );

bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint64> &vec );
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec );
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint32> &vec );
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec );
bool ProtobufReadRepeatedInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<bool> &vec );
bool ProtobufReadRepeatedSInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec );
bool ProtobufReadRepeatedSInteger( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec );
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int64> &vec );
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint64> &vec );
bool ProtobufReadRepeatedFixed64( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<double> &vec );
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<int32> &vec );
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<uint32> &vec );
bool ProtobufReadRepeatedFixed32( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<float> &vec );
bool ProtobufReadRepeatedString( const char * &pParsePosition, const char *pParseEnd, uint32 uFieldTag, std::vector<std::string> &vec );

#define PROTOBUF_FIELDTAG_INTEGER( Field )  ( (uint64)( Field ) << 3 )
#define PROTOBUF_FIELDTAG_SINTEGER( Field ) ( (uint64)( Field ) << 3 )
#define PROTOBUF_FIELDTAG_FIXED64( Field )  ( (uint64)( Field ) << 3 | (uint64)1 )
#define PROTOBUF_FIELDTAG_STRING( Field )   ( (uint64)( Field ) << 3 | (uint64)2 )
#define PROTOBUF_FIELDTAG_FIXED32( Field )  ( (uint64)( Field ) << 3 | (uint64)5 )

#define PROTOBUF_FIELDTAG_REPEATED_INTEGER PROTOBUF_FIELDTAG_STRING
#define PROTOBUF_FIELDTAG_REPEATED_SINTEGER PROTOBUF_FIELDTAG_STRING
#define PROTOBUF_FIELDTAG_REPEATED_FIXED32 PROTOBUF_FIELDTAG_STRING
#define PROTOBUF_FIELDTAG_REPEATED_FIXED64 PROTOBUF_FIELDTAG_STRING


#endif
