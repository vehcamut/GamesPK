﻿#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string shifr(bool flag, string num = "НЕТ ДАННЫХ")
{
	setlocale(LC_ALL, "Russian");
	ifstream in;
	ofstream out;
	string text = "";
	char chr;

	if (flag)
		in.open("config", ios::binary);
	else
		out.open("config", ios::binary);

	if (!flag)
	{
		text = num;
		while (text.length() % 7 != 0)
			text = text + " ";
		for (int i = 0; i < text.length(); i += 7)
		{
			//массив с 7 символами, которые кодируются
			unsigned char temp[7];
			for (int j = 0; j < 7; j++)
				temp[j] = text[i + j];
			//семибитный код
			unsigned char sevenbit = 0;
			//заполнение семибитного кода, привидение к unsigned char необзодимо для того, что бы лишние биты отбросились
			for (int j = 0; j < 7; j++)
				sevenbit = ((sevenbit << 1) | (unsigned char(temp[j] << 7) >> 7));
			//запись в файл измененных символов:
			//	1. отбрасвается последний бит
			//	2. отбрасываются j первых битов 
			//	3. добавляются первые j - 1 бит из следующего байта
			for (int j = 0; j < 6; j++)
			{
				unsigned char symbol = (unsigned char(temp[j] >> 1) << (j + 1)) | (temp[j + 1] >> (7 - j));
				out.write((char*)&symbol, sizeof(symbol));
			}
			//формирование и запись последнего байта
			sevenbit = sevenbit | (unsigned char(temp[6] >> 1) << 7);
			out.write((char*)&sevenbit, sizeof(sevenbit));
		}
		out.close();
	}
	else
	{
		string result = "";
		if (in.is_open())
		{
			while (in)
			{
				in.get(chr);
				text = text + chr;
			}
			text = text.substr(0, text.length() - 1);
			if (text.length() % 7 != 0)
			{
				return "НЕТ_ДАННЫХ";
			}
			for (int i = 0; i < text.length(); i += 7)
			{
				//массив с 7 символами, которые раскодируются
				unsigned char temp[7];
				for (int j = 0; j < 7; j++)
					temp[j] = text[i + j];
				//семибитный код
				unsigned char sevenbit = unsigned char(temp[6] << 1) >> 1;
				//раскодирование символа:
				//	1. отбрасвается последние j + 1 бит
				//	2. если не первый бит, то добавляются последние 8 - j битов из предыдущего байта 
				//	3. в конец добавляется символ из семибитного кода
				for (int j = 0; j < 7; j++)
				{
					unsigned char symbol = unsigned char(temp[j] >> (j + 1)) << 1 | (j != 0 ? temp[j - 1] << (8 - j) : 0) | (sevenbit >> 6);
					sevenbit = unsigned char(sevenbit << 2) >> 1;
					result += symbol;
				}
			}
		}
		else
			result = "НЕТ_ДАННЫХ";
		in.close();
		return result;
	}
	return "";
}