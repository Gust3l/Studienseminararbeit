/*
 * pgmtogoo.cpp
 *
 *  Created on: 11 Sept 2024
 *      Author: August
 */

#include "pgmtogoo.h"
#include "goo.h"
using namespace std;

pgm_to_goo::pgm_to_goo(int layers) {
	headerinfo = new char[headerinfo_size];
	small_image = new char[small_image_size];
	big_image = new char[big_image_size];
	headerinfo2 = new char[headerinfo2_size];
	pgm = new char[bm_height * bm_width];
	this->layers = layers;
}

pgm_to_goo::~pgm_to_goo() {
	delete[] headerinfo;
	delete[] headerinfo2;
	delete[] small_image;
	delete[] big_image;
	delete[] pgm;
}

void pgm_to_goo::read_info(string name) {
	ifstream headerinfo_d { name + "_headerinfo.bin", ios::binary };
	ifstream small_image_d { name + "_small_image.bin", ios::binary };
	ifstream big_image_d { name + "_big_image.bin", ios::binary };
	ifstream headerinfo2_d { name + "_headerinfo2.bin", ios::binary };
	headerinfo_d.read(headerinfo, headerinfo_size);
	small_image_d.read(small_image, small_image_size);
	big_image_d.read(big_image, big_image_size);
	headerinfo2_d.read(headerinfo2, headerinfo2_size);
	bm_width = (uint16_t) (headerinfo2[4]<<8) | (uint8_t) headerinfo2[5];
	bm_height = (uint16_t) (headerinfo2[6]<<8) | (uint8_t) headerinfo2[7];
}

void pgm_to_goo::read(int number, string name){
	ifstream datei_layer { "./pgms/" + name + to_string(number) + ".pgm",
			ios::binary };
	datei_layer.ignore(20);
	datei_layer.read(pgm, bm_height * bm_width);
	datei_layer.close();
}


void pgm_to_goo::compress(int number,string dateiname){
	uint8_t * layer = (uint8_t *) pgm;
	uint8_t checksum = 0;
	uint8_t value = layer[0];
	uint8_t pre_value = layer[0] + 0x10;
	uint8_t tmp = 0x00;
	uint32_t runlength = 1;
	uint32_t count = 0;
	bool check_bit = true;
	unsigned char byte = 0xff;
	ofstream datei { "./compressed_layers/" + dateiname + to_string(number)
			+ ".bin", ios::binary };
	datei.write("", 4);
	datei << (uint8_t) 0x55;
	count++;
	for (unsigned int i = 1; i <= bm_height * bm_width; i++) {
		if (i != bm_height * bm_width && value == layer[i]) {
			runlength++;
		} else {
			if (abs(value - pre_value) <= 15 && runlength < 256) {
				byte &= 0b10111111;
				check_bit = false;
			} else if (value == 0xff) {
				byte &= 0xff;
			} else if (value == 0x00) {
				byte &= 0b00111111;
			} else {
				byte &= 0b01111111;
				tmp = value;
			}
			if (runlength < 16 && check_bit) { //runlength < 2^4 ==> 4 bit
				byte &= 0b11001111 & (uint8_t) (runlength | 0xf0);
				datei << byte;
				count++;
				checksum += byte;
				if (tmp != 0) {
					datei << tmp;
					checksum += tmp;
					count++;
				}
			} else if (runlength < 4096 && check_bit) { //runlenght < 2^12 ==> 12 bit
				byte &= 0b11011111 & (uint8_t) (runlength | 0xf0);
				datei << byte;
				count++;
				checksum += byte;
				if (tmp != 0) {
					datei << tmp;
					checksum += tmp;
					count++;
				}
				datei << (uint8_t) (runlength >> 4);
				count++;
				checksum += (uint8_t) (runlength >> 4);
			} else if (runlength < 1048576 && check_bit) { //runlength < 2^20 ==> 20bit
				byte &= 0b11101111 & (uint8_t) (runlength | 0xf0);
				datei << byte;
				count++;
				checksum += byte;
				if (tmp != 0) {
					datei << tmp;
					checksum += tmp;
					count++;
				}
				datei << (uint8_t) (runlength >> 12);
				datei << (uint8_t) (runlength >> 4);
				checksum += (uint8_t) (runlength >> 12);
				checksum += (uint8_t) (runlength >> 4);
				count += 2;
			} else if (check_bit) {
				byte &= 0xff & (uint8_t) (runlength | 0xf0); // big ?
				datei << byte;
				checksum += byte;
				count++;
				if (tmp != 0) {
					datei << tmp;
					checksum += tmp;
					count++;
				}
				datei << (uint8_t) (runlength >> 20);
				datei << (uint8_t) (runlength >> 12);
				datei << (uint8_t) (runlength >> 4);
				checksum += (uint8_t) (runlength >> 20);
				checksum += (uint8_t) (runlength >> 12);
				checksum += (uint8_t) (runlength >> 4);
				count += 3;
			}

			// diff value handler
			if (runlength == 1 && !check_bit) {
				if (value - pre_value > 0) {
					byte &= 0b11001111
							& (uint8_t) (abs(value - pre_value) | 0xf0);
					datei << byte;
					count++;
					checksum += byte;
				} else {
					byte &= 0b11101111
							& (uint8_t) (abs(value - pre_value) | 0xf0);
					datei << byte;
					count++;
					checksum += byte;
				}
			} else if (!check_bit) {
				if (value - pre_value > 0) {
					byte &= 0b11011111
							& (uint8_t) (abs(value - pre_value) | 0xf0);
				} else {
					byte &= 0b11111111
							& (uint8_t) (abs(value - pre_value) | 0xf0);
				}
				do {
					datei << byte;
					count++;
					checksum += byte;
					if (runlength > 255) {
						datei << (uint8_t) 0xff;
						runlength -= 255;
						count++;
						checksum += (uint8_t) 0xff;
					}
					datei << (uint8_t) runlength;
					checksum += (uint8_t) runlength;
					runlength = 0;
					count++;
				} while (runlength > 0);
			}

			pre_value = value;
			value = layer[i];
			runlength = 1;
			byte = 0xff;
			tmp = 0x00;
			check_bit = true;
		}
	}
	char check[1] = { (uint8_t) ~checksum };
	datei.write(check, 1);
	count++;
	datei << (uint8_t) 0x0D;
	datei << (uint8_t) 0x0A;
	datei.seekp(ios_base::beg);
	datei << (uint8_t) (count >> 24);
	datei << (uint8_t) (count >> 16);
	datei << (uint8_t) (count >> 8);
	datei << (uint8_t) (count);
	datei.close();

}



void pgm_to_goo::glue(string dateiname) {
	char size_c[4];
	unsigned int size;
	ofstream datei { dateiname + ".goo", ios::binary };
	datei.write(headerinfo, headerinfo_size);
	datei.write(small_image, small_image_size);
	datei.write(big_image, big_image_size);
	datei.write(headerinfo2, headerinfo2_size);
	for (int i = 0; i < layers; i++) {
		char *layerinfo = new char[layerinfo_size];
		ifstream datei_info { "./layerinfos/" + dateiname + "_layerinfo"
				+ to_string(i) + ".bin", ios::binary };
		datei_info.read(layerinfo, layerinfo_size);
		datei_info.close();
		datei.write(layerinfo, layerinfo_size);
		ifstream c_layer { "./compressed_layers/" + dateiname + to_string(i)
				+ ".bin", ios::binary };
		c_layer.read(size_c, 4);
		size = to_int(size_c) + 2;
		char *layer = new char[size];
		c_layer.read(layer, size);
		datei.write(size_c, 4);
		datei.write(layer, size);
		delete[] layer;
		delete[] layerinfo;
		c_layer.close();
	}
	datei.write(end_string, 11);
	datei.close();
}

