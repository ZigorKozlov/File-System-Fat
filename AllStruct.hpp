#pragma once
#include <string>

using namespace std;

typedef struct SuperBlock {
	unsigned int _sizeOneBlock;
	unsigned int _sizeFatTable;
	unsigned int _sizeRoot;
	unsigned int _shift;

}SuperBlock;

typedef struct FatTable {
	unsigned int _size;
	unsigned int *_pFat;
}FatTable;

typedef struct myFile {
	string _name;
	unsigned int _numFirstBlock;
	unsigned int _atr;	
	unsigned int _sizeOfFile;
	string _dir;

}myFile;
