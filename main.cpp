#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
using namespace std;

#define PROPERTY_NUM 4

struct Setdata
{
	double property[PROPERTY_NUM];
	int label;
};
vector<Setdata> setdata;

int ReadTxt(string file)
{
	ifstream infile;
	infile.open(file.data());	// ���ļ����������ļ���������
	assert(infile.is_open());	// ����ʧ�ܣ������������Ϣ������ֹ��������

	string s;
	while (getline(infile, s))
	{
		int i = 0;
		string str_temp ="" ;	// ��ʼ��stringΪ�գ����ü� ="" Ҳ��
		Setdata temp;

		// �ָ�ÿһ�е�����
		for (int j = 0; s[j] != '\0'; j++)
		{
			if (s[j] != '\t')	// ��Tab��Ϊ�ָ��
			{
				if (s[j + 1] == '\0') temp.label = s[j] - 48;	// �������һ��Ԫ���Ǳ�ǩ
				else str_temp += s[j];
			}				
			else
			{
				temp.property[i++] = atof(str_temp.c_str());	// ��string���͵�����ת���ɶ�Ӧ��double����
				str_temp.clear();
			}
		}
		// ���ָ�õ����ݼ���vector����
		setdata.push_back(temp);
		
		/* ======== ����Ϊ test ======== 
		cout <<setw(6)<< temp.property[0] << " "
			<< setw(6) << temp.property[1] << " "
			<< setw(6) << temp.property[2] << " "
			<< setw(6) << temp.property[3] << " "
			<< setw(6) << temp.label << endl;
		 ======== ����Ϊ test ======== */
	}
	return 0;
}

int CalEntropy();

int CalGain();

int BuildDecisionTree_ID3();




int main()
{
	// ��ȡѵ������
	ReadTxt("traindata.txt");

	// ����ѵ�����ݹ���������
		// ������Ϣ����

	// ��ȡ��������

	// ͨ�������������ݷ���

	return 0;
}