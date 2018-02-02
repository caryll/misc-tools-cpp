#pragma once
#ifndef RECORDS_H
#define RECORDS_H
#include <cstdint>
#include <istream>
#include <vector>
#include <memory>
#include "bigendian.h"

struct Measuable
{
	std::uint32_t offset = 0;
};

struct TableBlock : Measuable
{
	char *data;

	TableBlock(std::istream& stream, std::uint32_t offset, std::uint32_t length)
	{
		data = new char[length];
		stream.seekg(offset);
		stream.read(data, length);
	}
	~TableBlock()
	{
		if (data != nullptr)
			delete[] data;
	}
};

struct Table
{
	std::uint32_t tag;
	std::uint32_t checkSum;
	std::uint32_t length;
	std::uint32_t dataOffset;
	std::shared_ptr<TableBlock> dataBlock;

	void check(const Table& other)
	{
		if (length == other.length &&
			checkSum == other.checkSum &&
			memcmp(dataBlock->data, other.dataBlock->data, length) == 0)
			dataBlock = other.dataBlock;
	}

	void load(std::istream& stream)
	{
		dataBlock = std::make_shared<TableBlock>(stream, dataOffset, length);
	}
};

struct OTFHeader : Measuable
{
	std::uint32_t sfntVersion;
	std::uint16_t numTables;
	std::uint16_t searchRange;
	std::uint16_t entrySelector;
	std::uint16_t rangeShift;
	std::vector<Table> tables;
};

struct TTCHeader
{
	std::uint32_t tag;
	std::uint16_t majorVersion;
	std::uint16_t minorVersion;
	std::uint32_t numFonts;
	std::vector<OTFHeader> fonts;
};

inline std::istream& operator>>(std::istream& stream, OTFHeader& record)
{
	record.sfntVersion = ReadBE32(stream);
	record.numTables = ReadBE16(stream);
	record.searchRange = ReadBE16(stream);
	record.entrySelector = ReadBE16(stream);
	record.rangeShift = ReadBE16(stream);
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const OTFHeader& record)
{
	WriteBE32(stream, record.sfntVersion);
	WriteBE16(stream, record.numTables);
	WriteBE16(stream, record.searchRange);
	WriteBE16(stream, record.entrySelector);
	WriteBE16(stream, record.rangeShift);
	return stream;
}

inline std::istream& operator>>(std::istream& stream, Table& record)
{
	record.tag = ReadBE32(stream);
	record.checkSum = ReadBE32(stream);
	record.dataOffset = ReadBE32(stream);
	record.length = ReadBE32(stream);
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const Table& record)
{
	WriteBE32(stream, record.tag);
	WriteBE32(stream, record.checkSum);
	WriteBE32(stream, record.dataBlock->offset);
	WriteBE32(stream, record.length);
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const TTCHeader& header)
{
	WriteBE32(stream, header.tag);
	WriteBE16(stream, header.majorVersion);
	WriteBE16(stream, header.minorVersion);
	WriteBE32(stream, header.numFonts);
	for (auto o : header.fonts)
		WriteBE32(stream, o.offset);
	return stream;
}

#endif
