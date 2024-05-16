#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <stack>

#ifndef BMPREADER_H
#define BMPREADER_H

struct BMP {
	int x;
	int y;
	int colorDepth;
	int size;
	int startPointPixels;
	int padding;
};

void reader(std::vector<char>* bytes, BMP* pImage, std::vector<sf::Uint8>* pixels);
std::vector<char> getDataVector(std::string filename, BMP* pImage);
#endif
