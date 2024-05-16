#include "bmpreader.h"

#include <fstream>
#include <iostream>
#include <stack>
#include <algorithm>
#include <bitset>

int mergeBytes(std::stack<char> byteStack, int elementSize) {
	int value = 0;
	//3shift left
	int stackLen = byteStack.size();
	for (int x = 0; x < stackLen; x++) {
		if (x < elementSize) value = (value << 8) | byteStack.top();
		byteStack.pop();
	}
	return value;
}

int mergeBytesLeftOrder(std::vector<sf::Uint8>* vectorToMerge, int numBytes){
	std::reverse(vectorToMerge->begin(), vectorToMerge->end());
	int value = 0;
	size_t Len = vectorToMerge->size();
	for (int x = 0; x < Len; x++) {
		if (x < numBytes) value = (value << 8) | vectorToMerge->data()[x];
	}
	
	return sf::Int32(value);
}

void setPadding(BMP* pImage) {
	pImage->padding = (pImage->x*3)%4;
}

void setWidth(long* head, BMP* pImage, std::vector<char>* bytes) {
	std::vector<sf::Uint8> byteVector;
	int lastAddr = *head + 4;
	
	while (*head < lastAddr) {
		byteVector.push_back(bytes->data()[*head]);
		(*head)++;
	}
	pImage->x = mergeBytesLeftOrder(&byteVector, 4);
}

void setHeight(long* head, BMP* pImage, std::vector<char>* bytes) {
	std::vector<sf::Uint8> byteVector;
	int lastAddr = *head + 4;
	while (*head < lastAddr) {
		byteVector.push_back(bytes->data()[*head]);
		(*head)++;
	}
	pImage->y = mergeBytesLeftOrder(&byteVector,4);
}

void setColorDepth(long* head, BMP* pImage, std::vector<char>* bytes) {
	std::stack<char> byteStack;
	int lastAddr = *head + 2;
	while (*head < lastAddr) {
		byteStack.push(bytes->data()[*head]);
		(*head)++;
	}
	pImage->colorDepth = mergeBytes(byteStack, 4);
}

void setStartPoint(long* head, BMP* pImage, std::vector<char>* bytes) {
	std::vector<sf::Uint8> byteVector;
	int lastAddr = *head + 4;
	while (*head < lastAddr) {
		byteVector.push_back(bytes->data()[*head]);
		(*head)++;
	}
	pImage->startPointPixels = mergeBytesLeftOrder(&byteVector, 4);
}

void extractPixels(std::stack<sf::Uint8>* pixelStack, std::vector<char>* bytes, long* head, int padding, int x) {
	/*
		Ler uma linha de pixels
		a cada 3 pixels adiciona um alfa de 255
	*/
	int lastAddr = (*head) + (x * 3); //endereço inicial + numero de bytes
	int pixelCounter = 0;
	while (*head < lastAddr)
	{	
		
		if (pixelCounter == 0) {
			pixelStack->push(255);
			pixelCounter = 3;
		}
		pixelStack->push(bytes->data()[*head]);
		(*head)++;
		pixelCounter=pixelCounter>0?--pixelCounter : pixelCounter++;
	}
	
	(*head) += padding;
}
void stackToVector(std::vector<sf::Uint8>* vector, std::stack<sf::Uint8>* stack) {
	while (!stack->empty()) {
		vector->push_back(stack->top());
		stack->pop();	
	}
}
void reader(std::vector<char>* bytes, BMP* pImage,std::vector<sf::Uint8>* pixels){
	if (!(bytes->data()[0] == 0x42 && bytes->data()[1] == 0x4D)) {
		std::cout << " um BMP valido\n";
		exit(1);
	}
	long head = 10; //o tamanho em bytes ja temos e os outros valores vou n considerar 
	setStartPoint(&head, pImage, bytes);
	std::cout << "sp:" << pImage->startPointPixels << "\n";
	//como nao preciso do numero de bytes do DIB vou adicionar 4 no head e pular esses bytes
	head += 4;
	setWidth(&head, pImage, bytes);
	std::cout << "w:" << pImage->x << "\n";
	setHeight(&head, pImage, bytes);
	std::cout << "h:" << pImage->y << "\n";
	setPadding(pImage);
	std::cout <<"pd:" << pImage->padding << "\n";
	//pulanod isso de plano
	head += 2;
	setColorDepth(&head, pImage, bytes);
	std::cout << "cd:" << pImage->colorDepth << "\n";
	//pulando bytes "inuteis"
	//head += 24;
	head = pImage->startPointPixels;
	std::stack<sf::Uint8> pixelStack;
	int currentLine = 0;
	while (currentLine<pImage->y) {
		extractPixels(&pixelStack,bytes,&head,pImage->padding,pImage->x);
		currentLine++;
	}
	int expectSize = (pImage->x * 4 * pImage->y);
	std::cout << "Numero de bytes esperados: " << expectSize << "\n";
	std::cout << "Tamanho da stack: " << pixelStack.size() << "\n";
	stackToVector(pixels,&pixelStack);
	std::cout << "Tamanho do vector: " << pixels->size() << "\n";

	if (expectSize == pixels->size()) {
		std::cout << "Correto\n";
		return;
	}
	std::cout << "errado\n";
}

std::vector<char> getDataVector(std::string filename, BMP* pImage) {
	std::ifstream arquivo(filename);
	if (!arquivo.is_open()) {
		std::cerr << "Erro ao abrir o arquivo\n";
		exit(1);
	}

	arquivo.seekg(0, std::ios::end);
	std::streampos tamanho = arquivo.tellg();
	arquivo.seekg(0, std::ios::beg);
	pImage->size = tamanho;

	std::vector<char> bytesVector(tamanho);
	arquivo.read(bytesVector.data(), tamanho);
	arquivo.close();
	

	return bytesVector;
}