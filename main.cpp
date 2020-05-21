#include <fstream>
#include<iostream>
#include <string>
#include"HandlerISO.h"

using namespace std;

void outname(string &name, ofstream &stream) {
	unsigned char a;

	for (int i = 0; i < name.length(); i++) {
		a = name[i];
		stream.write((char *)&a, sizeof(char));
	}

	for (int i = 0; i < 12 - name.length(); i++) {
		a = 0;
		stream.write((char *)&a, sizeof(char));
	}
};

int createISO() {

	ofstream fp("fileObraz.iso", ios::out | ios::binary);

	//SuperBlock                         
	unsigned int SuperBlock[4] = { 0x4, 0x8, 48, 0x0 };

	fp.write((char *)SuperBlock, sizeof(unsigned int) * 4);//write SuperBlock


	//FAT 4 unsigned int + Кол-во записей
	const unsigned int N = 8;//Количество записей
	unsigned int FatArr[N] = { 0xFF, 0x3, 0x4, 0x5, 0x6, 0x7, 0xFF, 0xFF };

	fp.write((char *)FatArr, sizeof(unsigned int) * N);




	//Root

	unsigned int nBlock, size;
	//File A.txt
		//name
	string name = "A.txt";
	outname(name, fp);

	//num first Block
	nBlock = 0x1;
	fp.write((char *)&nBlock, sizeof(nBlock));

	//atr
	unsigned int atr = 0x00;
	fp.write((char *)&atr, sizeof(atr));

	//Размер в байтах
	size = 4;//В байтах
	fp.write((char *)&size, sizeof(size));




	//Дирректория B

				//name
	name = "B";

	outname(name, fp);

	//num first Block
	nBlock = 0x2;
	fp.write((char *)&nBlock, sizeof(nBlock));

	//atr
	atr = 0x01;//Dirr
	fp.write((char *)&atr, sizeof(atr));

	//Размер в байтах
	size = 21;//Для директории - это количесто файлов в дириктории* 21
	fp.write((char *)&size, sizeof(size));



	//root end




	//data блоки по 4 байта  0-A 1-B 2-C 

	//1 
		//data A.txt блоки 0(4 байта)
	char date = 'h';

	fp.write((char *)&date, sizeof(char));
	date = 'i';
	fp.write((char *)&date, sizeof(char));
	date = '!';
	fp.write((char *)&date, sizeof(char));
	date = '\0';
	fp.write((char *)&date, sizeof(char));


	//2,3,4,5,6,7
		//фалй С //Т.К. C в дериктории B, то его описание в данных для этой директории - нужно 6 кластеров

	name = "C.txt";

	outname(name, fp);

	//num first Block
	nBlock = 0x8;
	fp.write((char *)&nBlock, sizeof(nBlock));

	//atr
	atr = 0x02;
	fp.write((char *)&atr, sizeof(atr));

	//Размер в байтах
	size = 4;//Для директории - это количесто файлов
	fp.write((char *)&size, sizeof(size));


	//2 
	//Для файла С.txt
	date = 'b';

	fp.write((char *)&date, sizeof(char));
	date = 'y';
	fp.write((char *)&date, sizeof(char));
	date = 'e';
	fp.write((char *)&date, sizeof(char));
	date = '\0';;
	fp.write((char *)&date, sizeof(char));

	fp.close();

	return 0;
}



int main() {

	setlocale(LC_ALL, "Rus");

	//Create ISO
	int choice = 0;

	cout << "Do you want to create default ISO file sistem?" << endl;
	cout << "[1] Yes." << endl << "[0] No." << endl;
	cin >> choice;
	system("cls");

	if (choice)
		createISO();

	//handling file system

	HandlerISO myFileS;
	
	return myFileS.menu();
}