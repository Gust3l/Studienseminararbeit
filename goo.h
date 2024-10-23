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
	uint16_t bm_width;
	uint16_t bm_height;
	char * headerinfo;
	char * small_image;
	char * big_image;
	char * headerinfo2;
	unsigned char * decoded;
	std::vector<char*> layers;
	std::vector<char*> layerinfos;
	std::vector<int> sizes;
public:
	goo();
	~goo();
	void read(std::string dateiname);
	void write(std::string name);
	void decoding(int layer);
	void write_pgm(std::string dateiname);
};


int to_int(char* size_c);

#endif /* GOO_H_ */
