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
	infile.open(file.data());	// 将文件流对象与文件连接起来
	assert(infile.is_open());	// 若打开失败，则输出错误消息，并终止程序运行

	string s;
	while (getline(infile, s))
	{
		int i = 0;
		string str_temp ="" ;	// 初始化string为空，不用加 ="" 也行
		Setdata temp;

		// 分割每一行的数据
		for (int j = 0; s[j] != '\0'; j++)
		{
			if (s[j] != '\t')	// 以Tab作为分割符
			{
				if (s[j + 1] == '\0') temp.label = s[j] - 48;	// 数据最后一个元素是标签
				else str_temp += s[j];
			}				
			else
			{
				temp.property[i++] = atof(str_temp.c_str());	// 将string类型的数据转换成对应的double类型
				str_temp.clear();
			}
		}
		// 将分割好的数据加入vector容器
		setdata.push_back(temp);
		
		/* ======== 以下为 test ======== 
		cout <<setw(6)<< temp.property[0] << " "
			<< setw(6) << temp.property[1] << " "
			<< setw(6) << temp.property[2] << " "
			<< setw(6) << temp.property[3] << " "
			<< setw(6) << temp.label << endl;
		 ======== 以上为 test ======== */
	}
	return 0;
}

int CalEntropy();

int CalGain();

int BuildDecisionTree_ID3();




int main()
{
	// 读取训练数据
	ReadTxt("traindata.txt");

	// 利用训练数据构建决策树
		// 计算信息增益

	// 读取测试数据

	// 通过决策树给数据分类

	return 0;
}