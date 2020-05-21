#include "HandlerISO.h"

HandlerISO::HandlerISO()
{
	ifstream fp("fileObraz.iso", ios::in | ios::binary);

	//SuperBlock   16 byte                      

	fp.read((char *)&_superBlock, sizeof(unsigned int) * 4);//read SuperBlock


	int i = 0;
	//FAT 4 unsigned int * ���-�� �������
	_fatTable._size = _superBlock._sizeFatTable;
	_fatTable._pFat = new unsigned int[_fatTable._size];

	fp.read((char *)_fatTable._pFat, sizeof(unsigned int) * _fatTable._size); //4* _fatTable._size byte

	//� ������ ������ �� ����� ����������� ������, �.�. ����������� ��� �� �� ����, ��� ��� ���� _shift ������ ��� �����������, ��� ��� ����
	//� �� ��������, ����� ��������� ��������� �����, ��'nj ����� ������ ���� ���� �����, � ��� ��� �� ������))

		//Root	
	_countFiles = _superBlock._sizeRoot / 21;

	_listOfFiles.resize(_countFiles);


	for (int i = 0; i < _countFiles; i++) {

		inName(_listOfFiles[i]._name, fp);//���� ����� ����� 12 1 byte


		//num first Block in FAT 1 byte
		fp.read((char *)&_listOfFiles[i]._numFirstBlock, sizeof(unsigned int));

		//atr 4 byte
		fp.read((char *)&_listOfFiles[i]._atr, sizeof(unsigned int));


		//������ � ������  4 byte
		fp.read((char *)&_listOfFiles[i]._sizeOfFile, sizeof(unsigned int));

		_listOfFiles[i]._dir = "root";

	}

	//data ����� �� 4 �����  � ����� �������
	char a;
	for (int i = 0; i < _fatTable._size* _superBlock._sizeOneBlock; i++) {
		fp.read((char *)&a, sizeof(a));
		_data.push_back(a);
	}

	fp.close();


	//������ ���������� ������ �� ������ �����, ������� ��������� � �����������k 
	unsigned int mask = 0x0001;
	for( int k = 0; k < _listOfFiles.size(); k ++)
	{
		if ((_listOfFiles[k]._atr & mask) == 1) {//������ ��� ����������(�������� �� ���� �����������)
			//��������� ������� ������ � ����� �� data
			//�������� ����� ������� ��������
			_listOfFiles.resize(_listOfFiles.size() + 1);//��������� ����� ���� � ������

			unsigned int num = _listOfFiles[k]._numFirstBlock - 1;
			const unsigned s = _superBlock._sizeOneBlock;
			unsigned int j = 0;
			//�������� ���
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

		cout << "[0] �����." << endl;
		cout << "[1] ������� ���������� � �������� ������� �� ����������." << endl;
		cout << "[2] ������� ���������� ��������� ��������."<< endl;
		cout << "[3] ������� ���������� ������� Fat." << endl;
		cout << "[4] ������� ���������� � ���������� ������ ��� ���������." << endl;
		cout << "[5] ������� ������ ���� �������� �������." << endl;
		cout << "[6] ������� ��������� ����� �� ������ �� �����." << endl;
		cout << "[7] ������� ���� �� �������� �������." << endl;

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
	//��������� 
	cout << "���������:" << endl;
	cout << "������ 1 �����: " << _superBlock._sizeOneBlock << endl;
	cout << "������ Fat: " << _superBlock._sizeFatTable << endl;
	cout << "������ Root: " << _superBlock._sizeRoot << endl;
	cout << "�����: " << _superBlock._shift << endl;
	
}

void HandlerISO::printRoot()
{
	cout << "Root:" << endl;
	for (auto elem : _listOfFiles) {
		if (elem._dir == "root") {
			cout << "��� �����: " << elem._name << endl;
			cout << "� 1 ����� � Fat:" << elem._numFirstBlock << endl;
			cout << "��������: ";

			if ((elem._atr & 0x001) == 1) {
				cout << "�������, ";
			}
			else {
				cout << "�� �������, ";
			}

			if ((elem._atr & 0x002) == 2) {
				cout << "������ + ������. " << endl;
			}
			else {
				cout << "������ ������. " << endl;
			}
			cout << "������:" << elem._sizeOfFile << endl << endl;
		}
	}

}

void HandlerISO::printFat()
{
	cout << "Fat Table"<<endl;
	cout << setw(10) << "�" << setw(10) << "������" << endl;
	for (int i = 0; i < _fatTable._size; i++) {
		cout << setw(10) << i + 1 << setw(10) << _fatTable._pFat[i] << endl;
	}
}

void HandlerISO::printFC()
{
	string name;

	cout << "������� ��� �������� ��������:";
	cin >> name;

	for (auto elem : _listOfFiles) {
		if (name == elem._name) {
			if ((elem._atr & 0x0001) == 1) {//������ ���������� ������� ������ ��� ���������� ����� ������ � ����������
				cout << "��� �����: " << elem._name << endl;
				cout << "� 1 ����� � Fat:" << elem._numFirstBlock << endl;
				cout << "��������: ";

				if ((elem._atr & 0x001) == 1) {
					cout << "�������, ";
				}
				else {
					cout << "�� �������, ";
				}

				if ((elem._atr & 0x002) == 2) {
					cout << "������ + ������. " << endl;
				}
				else {
					cout << "������ ������. " << endl;
				}

				cout << "������:" << elem._sizeOfFile << endl << endl;
				cout << "����� � ������ �����������: ";

				for (auto el : _listOfFiles) {
					if (el._dir == elem._name) {
						cout << el._name << endl;		
					}
				}
			}
			else
			{
				cout << "��� �����: " << elem._name << endl;
				cout << "� 1 ����� � Fat:" << elem._numFirstBlock << endl;
				cout << "��������: ";

				if ((elem._atr & 0x001) == 1) {
					cout << "�������, ";
				}
				else {
					cout << "�� �������, ";
				}

				if ((elem._atr & 0x002) == 2) {
					cout << "������ + ������. " << endl;
				}
				else {
					cout << "������ ������. " << endl;
				}

				cout << "������:" << elem._sizeOfFile << endl << endl;
			}

		}
	}
}

void HandlerISO::printTree()
{

	for (auto elem : _listOfFiles) {
		cout << "��� �����: " << elem._name << endl;
		cout << "� 1 ����� � Fat:" << elem._numFirstBlock << endl;
		cout << "��������: ";

		if ((elem._atr & 0x001) == 1) {
			cout << "�������, ";
		}
		else {
			cout << "�� �������, ";
		}

		if ((elem._atr & 0x002) == 2) {
			cout << "������ + ������. " << endl;
		}
		else {
			cout << "������ ������. " << endl;
		}

		cout << "������:" << elem._sizeOfFile << endl;
		cout << "��� ��������, � ������� ����������: " << elem._dir<<endl<<endl;
	}
}

void HandlerISO::printTextFiles()
{

	string str;

	for (auto elem : _listOfFiles) {

		if ((elem._atr * 0x01) != 1) {

			cout << "��� �����: " << elem._name << endl;

			cout << "��������: ";

			if ((elem._atr & 0x002) == 2) {
				cout << "������ + ������. " << endl;
			}
			else {
				cout << "������ ������. " << endl;
			}

			cout << "������:" << elem._sizeOfFile << endl;

			cout << "����� � �����: ";

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
	cout << "������� ��� �����, ������ ������ �������: ";
	cin >> nameFile;


	for (auto elem : _listOfFiles) {

		if(elem._name == nameFile){
			// ���� ����������
			if ((elem._atr & 0x01) == 1) {

				for (auto el2 : _listOfFiles) {

					if (elem._name == el2._dir) {						cout << "� ������ �������� ������� �� �������� �����, ���������� �� �������." << endl;
						return 1;
					}

				}
			}

			if (elem._dir == "root") {
				_superBlock._sizeRoot -= 24;
			}

			//��� ����� � ������� ��� ��������������� ����� = 0(�� ������)
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

	cout << "����� ���� ��� ������� �� ������." << endl;
	return 1;
}




