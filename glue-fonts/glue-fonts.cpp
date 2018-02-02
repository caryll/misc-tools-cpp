#include <iostream>
#include "bigendian.h"
using ::std::cerr;
using ::std::endl;

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cerr << "Usage: glue-fonts output.ttc input1.ttf input2.ttf ..." << endl;
		return -1;
	}

	return 0;
}
