#pragma once
#include <fstream>
#include <vector>
#include "AllStruct.hpp"
#include <iostream>
#include <iomanip>

using namespace std;
class HandlerISO {

	SuperBlock _superBlock;
	FatTable _fatTable;
	vector <myFile> _listOfFiles;//Список всех файлов
	int _countFiles;

	vector <char> _data;


public:
	HandlerISO();// Конструктор считывает образ файловой системы с образа
	~HandlerISO();//Для сохранения данных в образ

	int delFile();

	int menu();

	void printSuperBlock();
	void printRoot();
	void printFat();
	void printFC();
	void printTree();
	void printTextFiles();

	friend void inName(string &name, ifstream &stream);
};



inline void inName(string &name, ifstream &stream) {

	char a;

	for (int i = 0; i < 12; i++) {
		stream.read((char *)&a, sizeof(char));
		if (a != 0) {
			name += a;
		}
	}
}

inline int convertCharToInt(unsigned char *chArr) {

	unsigned int tmp = 0;
	unsigned int val = 0;

	for (int i = 0; i < 4; i++)
	{
		tmp = chArr[i];
		tmp <<= i*8;
		val |= tmp;
	}

	return val;
};




