// Class for running Assembler

#include "Assembler.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "Error: Invalid number of arguments" << endl;
    } else {
	    Assembler assembler(argv[1], argv[2]);
	    assembler.parse();
        assembler.write();
    }
}
