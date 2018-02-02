#include <iostream>
#include "records.h"
#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>
using namespace std;

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cerr << "Usage: glue-fonts output.ttc input1.ttf input2.ttf ..." << endl;
		return -1;
	}

	TTCHeader ttc;
	ttc.tag = 0x74746366; //ttcf
	ttc.majorVersion = 1;
	ttc.minorVersion = 0;
	ttc.numFonts = argc - 2;
	ttc.fonts.resize(ttc.numFonts);

	int index = 2;
	for (auto& ttf : ttc.fonts)
	{
		char *filename = argv[index++];
		ifstream input(filename, ifstream::binary);

		if (!input.is_open())
		{
			cerr << "Cannot open file: " << filename << endl;
			return -1;
		}

		input >> ttf;

		ttf.tables.resize(ttf.numTables);
		for (auto &t : ttf.tables)
			input >> t;

		for (auto &t : ttf.tables)
			t.load(input);

		if (input.fail())
		{
			cerr << "Error reading file: " << filename << endl;
			return -1;
		}
	}

	for (auto& ttf : ttc.fonts)
		for (auto& table : ttf.tables)
			for (const auto& otherFont : ttc.fonts)
			{
				if (&ttf == &otherFont)break;
				for (const auto& otherTable : otherFont.tables)
					table.check(otherTable);
			}

	uint32_t current = 0;
	current += 4 + 2 + 2 + 4; //ttc header
	current += 4 * ttc.numFonts; //fonts offset table
	for (auto& ttf : ttc.fonts)
	{
		ttf.offset = current;
		current += 4 + 2 + 2 + 2 + 2; //otf header
		current += (4 + 4 + 4 + 4)*ttf.numTables;
	}

	for (auto& ttf : ttc.fonts)
	{
		for (auto& table : ttf.tables)
			if (table.dataBlock->offset == 0)
			{
				table.dataBlock->offset = current;
				current += table.length;
			}
			else table.dataOffset = table.dataBlock->offset;
	}

	ofstream output(argv[1], ofstream::binary);
	if (!output.is_open())
	{
		cerr << "Cannot open output: " << argv[1] << endl;
		return -1;
	}

	output << ttc;
	for (const auto& font : ttc.fonts)
	{
		output << font;
		for (const auto& table : font.tables)
			output << table;
	}

	uint32_t position = 0;
	for (const auto& font : ttc.fonts)
		for (const auto& table : font.tables)
		{
			if (position >= table.dataBlock->offset)continue;
			position = table.dataBlock->offset;
			output.write(table.dataBlock->data, table.length);
		}

	if (output.fail())
	{
		cerr << "Error writing file: " << argv[1] << endl;
		return -1;
	}

	return 0;
}
