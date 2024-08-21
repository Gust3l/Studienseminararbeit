/*
 * goo.cpp
 *
 *  Created on: 18 Aug 2024
 *      Author: August
 */

#include "goo.h"
using namespace std;

goo::goo() {
	// TODO Auto-generated constructor stub
}

void goo::read(string dateiname) {
	ifstream datei {dateiname, ios::binary };
	char size_c[4];
	unsigned int size;
	datei.read(headerinfo, headerinfo_size);
	datei.read(small_image, small_image_size);
	datei.read(big_image, big_image_size);
	datei.read(headerinfo2, headerinfo2_size);

	while (datei.good()) {
		char *layerinfo = new char[layerinfo_size];
		datei.read(layerinfo, layerinfo_size);
		layerinfos.push_back(layerinfo);
		datei.read(size_c,4);
		size = (((unsigned char)size_c[0] << 24) |
		            ((unsigned char)size_c[1] << 16) |
		            ((unsigned char)size_c[2] << 8)  |
		            (unsigned char)size_c[3])+2;
		sizes.push_back(size);
		char *layer = new char[size];
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
			ofstream layerinfo { "C:\\Users\\August\\Studienseminararbeit\\layerinfos\\"
					+ name + "_layerinfo" + to_string(i) + ".bin" , ios::binary };
			layerinfo.write(layerinfos[i], layerinfo_size);
		}

	for (size_t i = 0; i < layers.size(); i++) {
		ofstream layer { "C:\\Users\\August\\Studienseminararbeit\\layers\\"
				+ name + "_layer" + to_string(i) + ".bin" , ios::binary };
		layer.write(layers[i], sizes[i]);
	}
}

