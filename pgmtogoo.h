/*
 * pgmtogoo.h
 *
 *  Created on: 11 Sept 2024
 *      Author: August
 */

#ifndef PGMTOGOO_H_
#define PGMTOGOO_H_

#include <vector>
#include <deque>
#include <fstream>
#include <iostream>
//#include <cstring>

class pgm_to_goo {
	char * headerinfo;
	char * small_image;
	char * big_image;
	char * headerinfo2;
	char * pgm;
	int headerinfo_size = 194;
	int small_image_size = 26914;
	int big_image_size = 168202;
	int headerinfo2_size = 167;
	int layerinfo_size = 66;
	int layers;
	uint32_t bm_width;
	uint32_t bm_height;

	char end_string[11] = { (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00,
				(uint8_t) 0x07, (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00,
				(uint8_t) 0x44, (uint8_t) 0x4c, (uint8_t) 0x50, (uint8_t) 0x00 };

public:
	pgm_to_goo(int layers);
	~pgm_to_goo();
	void read_info(std::string name);
	void write_compressed_layers(int start, int end, std::string dateiname);
	void glue(std::string dateiname);
	void read(int number, std::string name);
	void compress(int number, std::string dateiname);
};


#endif /* PGMTOGOO_H_ */
