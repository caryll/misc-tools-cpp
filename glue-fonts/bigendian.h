#pragma once
#ifndef BIGENDIAN_H
#define BIGENDIAN_H
#include <cstdint>
#include <iostream>
using ::std::int16_t;
using ::std::int32_t;
using ::std::istream;
using ::std::ostream;

inline uint16_t ReadBE16(istream& stream)
{
	uint8_t buff[2];
	stream.read(reinterpret_cast<char*>(buff), 2);
	return (buff[0] << 8) + buff[1];
}

inline uint32_t ReadBE32(istream& stream)
{
	uint8_t buff[4];
	stream.read(reinterpret_cast<char*>(buff), 4);
	return buff[3]
		+ (buff[2] << 8)
		+ (buff[1] << 16)
		+ (buff[0] << 24);
}

inline void WriteBE16(ostream& stream, uint16_t num)
{
	uint8_t buff[2];
	buff[0] = (num & 0xff00) >> 8;
	buff[1] = num & 0x00ff;
	stream.write(reinterpret_cast<char*>(buff), 2);
}

inline void WriteBE32(ostream& stream, uint32_t num)
{
	uint8_t buff[4];
	buff[0] = (num & 0xff000000) >> 24;
	buff[1] = (num & 0x00ff0000) >> 16;
	buff[2] = (num & 0x0000ff00) >> 8;
	buff[3] = num & 0x000000ff;
	stream.write(reinterpret_cast<char*>(buff), 4);
}

#endif
