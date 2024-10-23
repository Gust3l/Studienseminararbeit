/*
 * goo.cpp
 *
 *  Created on: 18 Aug 2024
 *      Author: August
 */

#include "goo.h"
using namespace std;

goo::goo() {
	headerinfo = new char[headerinfo_size];
	small_image = new char[small_image_size];
	big_image = new char[big_image_size];
	headerinfo2 = new char[headerinfo2_size];
	decoded = new unsigned char[bm_width * bm_height];
}


goo::~goo() {
	delete[] headerinfo;
	delete[] headerinfo2;
	delete[] small_image;
	delete[] big_image;
	delete[] decoded;
	for (size_t i = 0; i < layerinfos.size(); i++) {
		delete[] layerinfos[i];
		delete[] layers[i];
	}
}

void goo::read(string dateiname) {
	ifstream datei { dateiname, ios::binary };
	char size_c[4];
	unsigned int size;
	datei.read(headerinfo, headerinfo_size);
	datei.read(small_image, small_image_size);
	datei.read(big_image, big_image_size);
	datei.read(headerinfo2, headerinfo2_size);
	bm_width = (uint16_t) (headerinfo2[4]<<8) | (uint8_t) headerinfo2[5];
	bm_height = (uint16_t) (headerinfo2[6]<<8) | (uint8_t) headerinfo2[7];
	while (datei.good()) {
		char *layerinfo = new char[layerinfo_size];
		datei.read(layerinfo, layerinfo_size);
		layerinfos.push_back(layerinfo);
		if (!datei.good()) {
			break;
		}
		datei.read(size_c, 4);
		size = to_int(size_c)+6;
		sizes.push_back(size);
		char *layer = new char[size+4];
		datei.seekg(-4,ios_base::cur);
		datei.read(layer, size);
		layers.push_back(layer);
	}
}

void goo::write(string name) {
	ofstream headerinfo_d { name + "_headerinfo.bin", ios::binary };
	ofstream small_image_d { name + "_small_image.bin", ios::binary };
	ofstream big_image_d { name + "_big_image.bin", ios::binary };
	ofstream headerinfo2_d { name + "_headerinfo2.bin", ios::binary };

	headerinfo_d.write(headerinfo, headerinfo_size);
	small_image_d.write(small_image, small_image_size);
	big_image_d.write(big_image, big_image_size);
	headerinfo2_d.write(headerinfo2, headerinfo2_size);

	for (size_t i = 0; i < layerinfos.size(); i++) {
		ofstream layerinfo { "./layerinfos/" + name + "_layerinfo"
				+ to_string(i) + ".bin", ios::binary };
		layerinfo.write(layerinfos[i], layerinfo_size);
	}

	for (size_t i = 0; i < layers.size(); i++) {
		ofstream layer { "./layers/" + name + "_layer" + to_string(i) + ".bin",
				ios::binary };
		layer.write(layers[i], sizes[i]);
	}
}

void goo::decoding(int number) {
	int size = sizes[number];
	unsigned char *layer = (unsigned char*) layers[number];
	unsigned char value;
	unsigned int runlength;
	unsigned char tmp;
	unsigned char og;
	int set = 1;
	long count = 0;
	int i = 5;
	while (i < size - 3) {
		tmp = layer[i] << 2;
		og = layer[i];
		set = 1;
		// Bits 7,6 überprüfen um den Wert der Pixel herauszufinden
		if (og < 64)
			value = 0x00;
		else if (og < 128) {
			value = layer[i + 1];
			i++;
		} else if (og < 192) {
			set = 0;
		} else {
			value = 0xff;
		}

		// Bits 5,4 überprüfen für die run length
		if (set) {
			if (tmp < 64) {
				runlength = (og & 0x0f); // Byte0[3:0]
				i++;
			} else if (tmp < 128) {
				runlength = layer[i + 1] << 4 | (og & 0x0f); //Byte1[7:0] und byte0[3:0]
				i += 2;
			} else if (tmp < 192) {
				runlength = layer[i + 1] << 12 | layer[i + 2] << 4
						| (layer[i] & 0x0f); // byte1[7:0] ,byte2[7:0] and byte0[3:0]
				i += 3;
			} else {
				runlength = layer[i + 1] << 20 | layer[i + 2] << 12
						| layer[i + 3] << 4 | (og & 0x0f);
				i += 4;
			}
		} else {
			if (tmp < 64) {
				runlength = 1;
				value += og & 0x0f;
				i++;
			} else if (tmp < 128) {
				runlength = layer[i + 1];
				value += og & 0x0f;
				i += 2;
			} else if (tmp < 192) {
				runlength = 1;
				value -= og & 0x0f;
				i++;
			} else {
				runlength = layer[i + 1];
				value -= og & 0x0f;
				i += 2;
			}
		}
		for (size_t j = 0; j < runlength; j++) {
			decoded[count] = value;
			count++;
		}
	}
}


void goo::write_pgm(string dateiname) {
	for (size_t k = 0; k < layers.size(); k ++) {
		ofstream pgm { "./pgms/" + dateiname + to_string(k) + ".pgm" };
		this->decoding(k);
		char *array = (char*) decoded;
		pgm << "P5" << endl;
		pgm << bm_width << " " << bm_height << endl;
		pgm << (uint32_t) 0xff << endl;
		pgm.write(array, bm_height * bm_width);
		pgm.close();
	}
}

int to_int(char *size_c) {
	return (((unsigned char) size_c[0] << 24)
			| ((unsigned char) size_c[1] << 16)
			| ((unsigned char) size_c[2] << 8) | (unsigned char) size_c[3]);
}

