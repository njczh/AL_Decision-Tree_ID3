#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

#define PROPERTY_NUM 4
#define LABEL_NUM 3

struct BiTNode
{
	BiTNode *lchild, *rchild;
	double judge;
	int label;
}*DT = NULL;

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
				/// OneNote!!
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

double CalEntropy(vector<Setdata> tuple)
{
	// info(D) = -∑Pi*log2(Pi)

	double entropy = 0;							// 熵
	double label_qty[LABEL_NUM] = {0};			// 每种标签的个数
	const double label_total = tuple.size();	// 标签总数
	
	if (label_total == 0) return 0;

	for (int i = 0; i < tuple.size(); i++) 
		label_qty[tuple[i].label - 1]++;		// 计算每一种标签的数量
	
	for (int i = 0; i < LABEL_NUM; i++)
	{
		double pi = label_qty[i] / label_total;// label_total 非零值
		if (pi) entropy += -(pi * log(pi) / log(2));	// log的参数是非零值
	}

	/* ======== 以下为 test ========
	cout << "entropy=" << entropy;
	======== 以上为 test ======== */
	return entropy;
}

double CalGain(vector<Setdata> tuple,vector<Setdata> cond ) // 元组 和 相对
{
	return CalEntropy(tuple) - CalEntropy(cond);
}

double CalGain_2(vector<Setdata> tuple, double entropy)
{
	return 2 * CalEntropy(tuple) - entropy;
}

bool AscendSortByPro0(const Setdata & st1, const Setdata & st2)
{
	if (st1.property[0] < st2.property[0]) return 1;
	else if (st1.property[0] == st2.property[0]){
			if (st1.label < st2.label) return 1;
			else return 0;
		}
	else return 0;

}
bool AscendSortByPro1(const Setdata & st1, const Setdata & st2)
{
	if (st1.property[1] < st2.property[1]) return 1;
	else if (st1.property[1] == st2.property[1]) {
		if (st1.label < st2.label) return 1;
		else return 0;
	}
	else return 0;
}
bool AscendSortByPro2(const Setdata & st1, const Setdata & st2)
{
	if (st1.property[2] < st2.property[2]) return 1;
	else if (st1.property[2] == st2.property[2]) {
		if (st1.label < st2.label) return 1;
		else return 0;
	}
	else return 0;
}
bool AscendSortByPro3(const Setdata & st1, const Setdata & st2)
{
	if (st1.property[3] < st2.property[3]) return 1;
	else if (st1.property[3] == st2.property[3]) {
		if (st1.label < st2.label) return 1;
		else return 0;
	}
	else return 0;
}

bool IsPure(vector<Setdata> &tuple)
{
	if (tuple.size() == 1) return true;
	for (vector<Setdata>::iterator iter = tuple.begin(); iter < tuple.end() - 1; iter++)
		if (iter->label != (iter + 1)->label) return false;
	return true;
}

int BuildDecisionTree_ID3();

int BuildDecisionTree_C4_5(vector<Setdata> tuple, BiTNode* &DT_node)
{
	// 第一步：判断所有实例是否都属于同一类，如果是，则到达叶子节点
	if (IsPure(tuple)) {
		DT_node = new BiTNode;
		DT_node->judge = -1;	// 表示叶子节点
		DT_node->label = tuple.begin()->label;
		DT_node->lchild = DT_node->rchild = NULL;
		return 0;
	}

	// 第二步：如果不是叶子节点，则计算所有可能分裂点的信息熵
	vector<Setdata> setdata_new[PROPERTY_NUM];
	vector<Setdata> split_1, split_2;

	double entropy_min = 10, entropy_temp;
	double split_value = 0;
	int min_index[2] ; // [0]代表属性 [1]代表划分下标

	// 在各个属性之间寻找熵最小的划分
	for (int i = 0; i < PROPERTY_NUM; i++)
	{
		setdata_new[i].assign(tuple.begin(), tuple.end());
		switch (i)
		{
		case 0:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro0); break;
		case 1:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro1); break;
		case 2:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro2); break;
		case 3:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro3); break;
		default:
			break;
		}		

		for (vector<Setdata>::iterator iter = setdata_new[i].begin(); iter < setdata_new[i].end() - 1; iter++)
		{
			if (iter->label == (iter + 1)->label) continue;
			else {
				split_1.assign(setdata_new[i].begin(), iter+1);
				// test // cout << "split_1=" << split_1.size()<<endl;
				split_2.assign(iter + 1, setdata_new[i].end());
				// test // cout << "split_2=" << split_2.size() << " ";
				entropy_temp = CalEntropy(split_1) + CalEntropy(split_2);

				//CalGain_2()

				// test // cout << i << "\t" << int(iter - setdata_new.begin()) << " " << entropy_temp << endl;
				if (entropy_min > entropy_temp) {
					entropy_min = entropy_temp;
					split_value = (iter->property[i] + (iter + 1)->property[i]) / 2;
					min_index[0] = i;
					min_index[1] = int(iter - setdata_new[i].begin());
				}
			}
		}
	}

	/* ======================== 以下为 test ========================

	cout << "此次最佳分裂点在属性" << min_index[0] << "的下标" 
		<< setw(2) << min_index[1] << "与" << setw(2) << min_index[1] + 1 
		<< "之间，分裂中值为 " << setw(6) << split_value;

	======================== 以上为 test ======================== */
	
	split_1.assign(setdata_new[min_index[0]].begin(), setdata_new[min_index[0]].begin() + min_index[1] + 1);//setdata_new[min_index[0]][min_index[1]]
	split_2.assign(setdata_new[min_index[0]].begin() + min_index[1] + 1, setdata_new[min_index[0]].end());

	DT_node = new BiTNode;
	DT_node->judge = split_value;
	DT_node->label = min_index[0];
	// test //cout << DT_node->lchild << DT_node->rchild;
	// test //cout << " split_1=" << setw(2) << split_1.size();
	BuildDecisionTree_C4_5(split_1, DT_node->lchild);
	// test //cout << " split_2=" << setw(2) << split_2.size() << endl;
	BuildDecisionTree_C4_5(split_2, DT_node->rchild);
	
	return 0;
}

double JudgeLable(vector<Setdata> testdata)
{
	int counter = 0;
	for (vector<Setdata>::iterator iter = testdata.begin(); iter < testdata.end(); iter++)
	{
		BiTNode *temp = DT;
		while (temp)
		{
			if (temp->judge != -1)
			{
				if (iter->property[temp->label] < temp->judge)
					temp = temp->lchild;
				else
					temp = temp->rchild;
			}
			else {
				if (temp->label != iter->label) counter++;
				break;
			}
		}	
	}
	return ((double)(testdata.size() - counter) / (double)testdata.size());
}

int printDT(BiTNode *node)
{
	if (node)
	{
		cout << node->label <<'\t'<<node->judge << endl;
		printDT(node->lchild);
		printDT(node->rchild);
	}
	return 0;
}

int main()
{
	// 读取训练数据
	ReadTxt("traindata.txt");

	// 利用训练数据构建决策树
	BuildDecisionTree_C4_5(setdata, DT);
	
	// 读取测试数据
	setdata.clear();
	ReadTxt("testdata.txt");

	// 通过决策树给数据分类
	cout << JudgeLable(setdata);

	return 0;
}