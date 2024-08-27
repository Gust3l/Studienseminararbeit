#include "goo.h"
#include <bitset>
using namespace std;

int main(int argc, const char *argv[]) {
	goo cube;
	cube.read(argv[1]);
	cube.write(argv[2]);
	cube.write_pgm(argv[2], stoi(argv[3]));
}
