#include <iostream>
#include "records.h"
#include <fstream>
#include <vector>
#include <memory>
using namespace std;

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cerr << "Usage: glue-fonts output.ttc input1.ttf input2.ttf ..." << endl;
		return -1;
	}

	TTCHeader ttc;
	ttc.fonts.resize(argc - 2);

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

	return 0;
}
