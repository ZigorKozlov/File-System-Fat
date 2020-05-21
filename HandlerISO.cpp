#include "HandlerISO.h"

HandlerISO::HandlerISO()
{
	ifstream fp("fileObraz.iso", ios::in | ios::binary);

	//SuperBlock   16 byte                      

	fp.read((char *)&_superBlock, sizeof(unsigned int) * 4);//read SuperBlock


	int i = 0;
	//FAT 4 unsigned int * Кол-во записей
	_fatTable._size = _superBlock._sizeFatTable;
	_fatTable._pFat = new unsigned int[_fatTable._size];

	fp.read((char *)_fatTable._pFat, sizeof(unsigned int) * _fatTable._size); //4* _fatTable._size byte

	//В данной работе не будут использовны сдвиги, т.к. шифроваться нам не от кого, так что поле _shift Только для наглядности, что оно есть
	//А по хорошему, нужно выполнить побитовый сдвиг, но'nj нужно читать весь обра разом, а мне так не удобно))

		//Root	
	_countFiles = _superBlock._sizeRoot / 21;

	_listOfFiles.resize(_countFiles);


	for (int i = 0; i < _countFiles; i++) {

		inName(_listOfFiles[i]._name, fp);//Ввод имени файла 12 1 byte


		//num first Block in FAT 1 byte
		fp.read((char *)&_listOfFiles[i]._numFirstBlock, sizeof(unsigned int));

		//atr 4 byte
		fp.read((char *)&_listOfFiles[i]._atr, sizeof(unsigned int));


		//Размер в байтах  4 byte
		fp.read((char *)&_listOfFiles[i]._sizeOfFile, sizeof(unsigned int));

		_listOfFiles[i]._dir = "root";

	}

	//data блоки по 4 байта  в нашем примере
	char a;
	for (int i = 0; i < _fatTable._size* _superBlock._sizeOneBlock; i++) {
		fp.read((char *)&a, sizeof(a));
		_data.push_back(a);
	}

	fp.close();


	//Теперь необходимо извлеч из данных файлы, которые находятся в дирикторияхk 
	unsigned int mask = 0x0001;
	for( int k = 0; k < _listOfFiles.size(); k ++)
	{
		if ((_listOfFiles[k]._atr & mask) == 1) {//Значит это директория(Проверка на флаг дирректории)
			//Неоходимо извлечь данные о файле из data
			//Получаем номер первого кластера
			_listOfFiles.resize(_listOfFiles.size() + 1);//Довабляем новый файл в вектор

			unsigned int num = _listOfFiles[k]._numFirstBlock - 1;
			const unsigned s = _superBlock._sizeOneBlock;
			unsigned int j = 0;
			//Получаем имя
			unsigned char a;

			for (int i = 0; i < 12; i++) {

				if (j >= s) {
					num = _fatTable._pFat[num] -1 ;
					j = 0;
				}

				a = _data[(num * s) + j ];
				if (a != 0) {
						_listOfFiles[_listOfFiles.size()-1]._name += a;
				}
				j++;
			
			}
			
			//Get fitst block
			unsigned char b[4];


			for (int i = 0; i < 4; i++) {

				if (j >= s) {
					num = _fatTable._pFat[num] - 1;
					j = 0;
				}

				b[i] = _data[(num*s) + j];
				j++;
			}
			
			_listOfFiles[_listOfFiles.size()-1]._numFirstBlock = convertCharToInt(b);

			//Get atrr

			for (int i = 0; i < 4; i++) {

				if (j >= s) {
					num = _fatTable._pFat[num] - 1;
					j = 0;
				}

				b[i] = _data[(num*s) + j];
				j++;
			}

			_listOfFiles[_listOfFiles.size() - 1]._atr = convertCharToInt(b);

			//Get size

			for (int i = 0; i < 4; i++) {

				if (j >= s) {
					num = _fatTable._pFat[num] - 1;
					j = 0;
				}

				b[i] = _data[(num*s) + j];
				j++;
			}

			_listOfFiles[ _listOfFiles.size() - 1 ]._sizeOfFile = convertCharToInt(b);
			_listOfFiles[ _listOfFiles.size() - 1 ]._dir = _listOfFiles[k]._name;
		} 
	}

}

HandlerISO::~HandlerISO()
{
	delete[] _fatTable._pFat;
}

int HandlerISO::menu()
{
	system("cls");

	char choice = '0';

	do {

		cout << setw(20) << "Menu" << endl;

		cout << "[0] Выход." << endl;
		cout << "[1] Вывести информацию о файловой системе из суперблока." << endl;
		cout << "[2] Вывести содержимое корневого каталога."<< endl;
		cout << "[3] Вывести содержимое таблицы Fat." << endl;
		cout << "[4] Вывести информацию о конкретных файлах или каталогах." << endl;
		cout << "[5] Вывести дерево всей файловой системы." << endl;
		cout << "[6] Вывести текстовые файлы из образа на экран." << endl;
		cout << "[7] Удалить файл из файловой системы." << endl;

		cin >> choice;
		system("cls");

		switch (choice) {
		case '1':
			printSuperBlock();
			system("pause");
			system("cls");
			break;

		case '2':
			printRoot();
			system("pause");
			system("cls");
			break;

		case '3':
			printFat();
			system("pause");
			system("cls");
			break;

		case '4':
			printFC();
			system("pause");
			system("cls");
			break;

		case '5':
			printTree();
			system("pause");
			system("cls");
			break;

		case '6':
			printTextFiles();
			system("pause");
			system("cls");
			break;

		case '7':
			delFile();
			system("pause");
			system("cls");
			break;


		default : break;
		}

	} while (choice != '0');

	return 0;
}

void HandlerISO::printSuperBlock()
{
	//СуперБлок 
	cout << "СУПЕРБЛОК:" << endl;
	cout << "Размер 1 блока: " << _superBlock._sizeOneBlock << endl;
	cout << "Размер Fat: " << _superBlock._sizeFatTable << endl;
	cout << "Размер Root: " << _superBlock._sizeRoot << endl;
	cout << "сдвиг: " << _superBlock._shift << endl;
	
}

void HandlerISO::printRoot()
{
	cout << "Root:" << endl;
	for (auto elem : _listOfFiles) {
		if (elem._dir == "root") {
			cout << "Имя файла: " << elem._name << endl;
			cout << "№ 1 блока в Fat:" << elem._numFirstBlock << endl;
			cout << "Атрибуты: ";

			if ((elem._atr & 0x001) == 1) {
				cout << "Каталог, ";
			}
			else {
				cout << "Не каталог, ";
			}

			if ((elem._atr & 0x002) == 2) {
				cout << "чтение + запись. " << endl;
			}
			else {
				cout << "только чтение. " << endl;
			}
			cout << "Размер:" << elem._sizeOfFile << endl << endl;
		}
	}

}

void HandlerISO::printFat()
{
	cout << "Fat Table"<<endl;
	cout << setw(10) << "№" << setw(10) << "Данные" << endl;
	for (int i = 0; i < _fatTable._size; i++) {
		cout << setw(10) << i + 1 << setw(10) << _fatTable._pFat[i] << endl;
	}
}

void HandlerISO::printFC()
{
	string name;

	cout << "Введите имя файлаили каталога:";
	cin >> name;

	for (auto elem : _listOfFiles) {
		if (name == elem._name) {
			if ((elem._atr & 0x0001) == 1) {//Значит директория выводим помимо его информацию имена файлов в дириктории
				cout << "Имя файла: " << elem._name << endl;
				cout << "№ 1 блока в Fat:" << elem._numFirstBlock << endl;
				cout << "Атрибуты: ";

				if ((elem._atr & 0x001) == 1) {
					cout << "Каталог, ";
				}
				else {
					cout << "Не каталог, ";
				}

				if ((elem._atr & 0x002) == 2) {
					cout << "чтение + запись. " << endl;
				}
				else {
					cout << "только чтение. " << endl;
				}

				cout << "Размер:" << elem._sizeOfFile << endl << endl;
				cout << "Файлы в данной дирриктории: ";

				for (auto el : _listOfFiles) {
					if (el._dir == elem._name) {
						cout << el._name << endl;		
					}
				}
			}
			else
			{
				cout << "Имя файла: " << elem._name << endl;
				cout << "№ 1 блока в Fat:" << elem._numFirstBlock << endl;
				cout << "Атрибуты: ";

				if ((elem._atr & 0x001) == 1) {
					cout << "Каталог, ";
				}
				else {
					cout << "Не каталог, ";
				}

				if ((elem._atr & 0x002) == 2) {
					cout << "чтение + запись. " << endl;
				}
				else {
					cout << "только чтение. " << endl;
				}

				cout << "Размер:" << elem._sizeOfFile << endl << endl;
			}

		}
	}
}

void HandlerISO::printTree()
{

	for (auto elem : _listOfFiles) {
		cout << "Имя файла: " << elem._name << endl;
		cout << "№ 1 блока в Fat:" << elem._numFirstBlock << endl;
		cout << "Атрибуты: ";

		if ((elem._atr & 0x001) == 1) {
			cout << "Каталог, ";
		}
		else {
			cout << "Не каталог, ";
		}

		if ((elem._atr & 0x002) == 2) {
			cout << "чтение + запись. " << endl;
		}
		else {
			cout << "только чтение. " << endl;
		}

		cout << "Размер:" << elem._sizeOfFile << endl;
		cout << "Имя каталога, в котором расположен: " << elem._dir<<endl<<endl;
	}
}

void HandlerISO::printTextFiles()
{

	string str;

	for (auto elem : _listOfFiles) {

		if ((elem._atr * 0x01) != 1) {

			cout << "Имя файла: " << elem._name << endl;

			cout << "Атрибуты: ";

			if ((elem._atr & 0x002) == 2) {
				cout << "чтение + запись. " << endl;
			}
			else {
				cout << "только чтение. " << endl;
			}

			cout << "Размер:" << elem._sizeOfFile << endl;

			cout << "Текст в файле: ";

			str = "";
			//
			unsigned int num = elem._numFirstBlock - 1;
			unsigned int j = 0, s = _superBlock._sizeOneBlock, count = 0;

			while (elem._sizeOfFile!= count) {

				if (j >= s) {
					num = _fatTable._pFat[num] - 1;
					j = 0;
				}

				str+= _data[(num * s) + j];

				j++;
				
				count++;
			}
			cout << str << endl<<endl;
		}
	}
}


int HandlerISO::delFile()
{
	string nameFile;
	vector <myFile>::iterator numFile = _listOfFiles.begin();
	cout << "Введите имя файла, котоый хотите удалить: ";
	cin >> nameFile;


	for (auto elem : _listOfFiles) {

		if(elem._name == nameFile){
			// Если директория
			if ((elem._atr & 0x01) == 1) {

				for (auto el2 : _listOfFiles) {

					if (elem._name == el2._dir) {						cout << "В данном каталоге имеются не удалённые файлы, необходимо их удалить." << endl;
						return 1;
					}

				}
			}

			if (elem._dir == "root") {
				_superBlock._sizeRoot -= 24;
			}

			//Все блоки в таблице фат соответствующие файлу = 0(не заняты)
			int num = elem._numFirstBlock - 1;
			int num2;

			while ((num + 1) != 0xFF) {
				num2 = num;
				num = _fatTable._pFat[num] - 1;
				_fatTable._pFat[num2] = 0;

			}

			_listOfFiles.erase(numFile);
			return 0;
		}


		numFile++;
	}

	cout << "Даный файл или каталог не найден." << endl;
	return 1;
}




