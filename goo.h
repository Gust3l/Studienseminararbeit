/*
 * goo.h
 *
 *  Created on: 18 Aug 2024
 *      Author: August
 */

#ifndef GOO_H_
#define GOO_H_
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>


class goo {
	int headerinfo_size = 194;
	int small_image_size = 26914;
	int big_image_size = 168202;
	int headerinfo2_size = 167;
	int layerinfo_size = 66;
	uint32_t bm_width = 5448;
	uint32_t bm_height = 3064;
	char * headerinfo = new char[headerinfo_size];
	char * small_image = new char[small_image_size];
	char * big_image = new char[big_image_size];
	char * headerinfo2 = new char[headerinfo2_size];
	uint32_t * decoded = new uint32_t [bm_width * bm_height];
	std::vector<char*> layers;
	std::vector<char*> layerinfos;
	std::vector<int> sizes;
public:
	goo();
	~goo();
	void read(std::string dateiname);
	void write(std::string name);
	void decoding(int layer);
	void write_pgm(std::string dateiname, int step);
	void write_pgm2(std::string dateiname, int step);
};


int to_int(char* size_c);

#endif /* GOO_H_ */
