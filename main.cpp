#include "goo.h"
#include "pgmtogoo.h"
//#include <bitset>
using namespace std;

int main(int argc, const char *argv[]) {
	// argv[1] Spezifiziert ob zu pgm oder zu goo
	//Bei argv[1] == pgm:
	// argv[2] Dateiname oder Pfad der einzulesenden Datei
	//argv[3] Dateiname unter dem die Schichten abgespeichert werden soll
	//Bei argv[1] == goo:
	// argv[2] Dateiname unter dem die Schichten zu finden sind
	//Dieser ist an die Konventionen der Abspeicherung von der Zerlegung zu pgm angepasst
	//Daher am Besten den gleichen Namen wie in argv[3] bei der Zerlegung benutzen
	//argv[3] Anzahl der Schichten des Objektes

	if (strcmp(argv[1], "pgm") == 0) {
		goo cube;
		cube.read(argv[2]);
		cube.write(argv[3]);
		cube.write_pgm(argv[3]);
	} else if (strcmp(argv[1], "goo") == 0) {
		int number_of_layers = stoi(argv[3]);
		pgm_to_goo back(number_of_layers);
		back.read_info(argv[2]);
		for (int i = 0; i < stoi(argv[3]); i++) {
			back.read(i, argv[2]);
			back.compress(i, argv[2]);
		}
		back.glue(argv[2]);
	} else {
		cerr << "Unknown Option " << argv[1] << endl;
		return -1;
	}
	return 0;
}
