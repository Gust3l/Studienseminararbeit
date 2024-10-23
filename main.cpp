#include "goo.h"
#include "pgmtogoo.h"
#include <bitset>
using namespace std;

int main(int argc, const char *argv[]) {
//	goo cube;
//	cube.read(argv[1]);
//	cube.write(argv[2]);
//	cube.write_pgm(argv[2]);

	int number_of_layers = stoi(argv[3]);

	pgm_to_goo back(number_of_layers);
	back.read_info(argv[2]);
	for(int i = 0; i<stoi(argv[3]); i++){
		back.read(i,"cube");
		back.compress(i,argv[2]);
	}
	back.glue(argv[2]);
}
