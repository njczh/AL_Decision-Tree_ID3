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
	double judge;					// judge为-1shi
	int label;
}*DT = NULL;

struct Setdata
{
	double property[PROPERTY_NUM];	// 数据的集中属性值
	int label;						// 数据标签
};

vector<Setdata> setdata;

void ReadTxt(string file)
{
	ifstream infile;
	infile.open(file.data());					// 将文件流对象与文件连接起来
	assert(infile.is_open());					// 若打开失败，则输出错误消息，并终止程序运行
	setdata.clear();							// 将数据容器初始化

	string s;
	while (getline(infile, s))
	{
		int i = 0;
		string str_temp = "";					// 初始化string为空，不用加 ="" 也行
		Setdata temp;

		for (int j = 0; s[j] != '\0'; j++) {	// 分割每一行的数据
			if (s[j] != '\t') {					// 以Tab作为分割符
				if (s[j + 1] == '\0')
					temp.label = s[j] - 48;		// 数据最后一个元素是标签
				else str_temp += s[j];
			}
			else {
				temp.property[i++] = atof(str_temp.c_str());	// 将string类型的数据转换成对应的double类型
				str_temp.clear();
			}
		}
		setdata.push_back(temp);				// 将分割好的数据加入vector容器
	}
}

/*******************************
*  Entropy(D) = -∑Pi*log2(Pi)
*  Pi = label_qty / label_total;
********************************/
double CalEntropy(vector<Setdata> &tuple)
{
	double entropy = 0;							// 熵
	double label_qty[LABEL_NUM] = { 0 };		// 每种标签的个数
	const double label_total = tuple.size();	// 标签总数
	if (label_total == 0) return 0;				// label_total 非零值

	for (int i = 0; i < tuple.size(); i++)
		label_qty[tuple[i].label - 1]++;		// 计算每一种标签的数量

	for (int i = 0; i < LABEL_NUM; i++)			// ∑ i=0 n=LABEL_NUM
	{
		double pi = label_qty[i] / label_total;			// label_total 非零值
		if (pi) entropy += -(pi * log(pi) / log(2));	// log的参数是非零值
	}

	return entropy;
}

/***********************************
* gain = Entropy(D) -  ∑Entropy(Ai)
***********************************/
double CalGain(vector<Setdata> &tuple, vector<Setdata> &split_1, vector<Setdata> &split_2)
{
	return CalEntropy(tuple)
		- ((double)split_1.size() / tuple.size()*CalEntropy(split_1)
			+ (double)split_2.size() / tuple.size()*CalEntropy(split_2));
}

double CalGainRatio(vector<Setdata> &tuple, vector<Setdata> &split_1, vector<Setdata> &split_2, double &gain)
{
	double total, num_split_1, num_split_2;
	total = tuple.size();
	num_split_1 = split_1.size();
	num_split_2 = split_2.size();

	double p1 = (double)num_split_1 / total;
	double p2 = (double)num_split_2 / total;

	double info = -(p1 * log(p1) / log(2) + p2 * log(p2) / log(2));
	return gain / info;
}

bool AscendSortByPro0(const Setdata & st1, const Setdata & st2)
{
	if (st1.property[0] < st2.property[0]) return 1;
	else if (st1.property[0] == st2.property[0]) {
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
	if (tuple.size() == 1) return true;		// 如果集合里只有一个数，认为它就是有序的
	for (vector<Setdata>::iterator iter = tuple.begin(); iter < tuple.end() - 1; iter++)
		if (iter->label != (iter + 1)->label) return false;	// 如果存在有不一样的元素，即认为该集合不纯净
	return true;
}

int BuildDecisionTree(vector<Setdata> &tuple, BiTNode* &DT_node)
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
	vector<Setdata> setdata_new[PROPERTY_NUM];	// 为了保存排序后的副本
	vector<Setdata> split_1, split_2;

	double entropy_max = -1;					// 最大信息增益
	double entropy_temp;
	double split_value = 0;						// 分裂终止
	int min_index[2];							// [0]代表属性 [1]代表划分下标

	for (int i = 0; i < PROPERTY_NUM; i++)		// 在各个属性之间寻找熵最小的划分
	{
		setdata_new[i].assign(tuple.begin(), tuple.end());	// 保存一个排序后的副本，方便之后划分
		switch (i)	// 将各种属性按照升序排序
		{
		case 0:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro0); break;
		case 1:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro1); break;
		case 2:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro2); break;
		case 3:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro3); break;
		default:
			break;
		}

		// 有n-1种分裂可能
		for (vector<Setdata>::iterator iter = setdata_new[i].begin(); iter < setdata_new[i].end() - 1; iter++)
		{
			// 在元素标签不同处求信息增益，而非在所有的两两元素之间，大幅减少计算量
			if (iter->label == (iter + 1)->label) continue;
			else {
				// 在可能的最优分裂点处将集合分开，为了计算信息增益
				split_1.assign(setdata_new[i].begin(), iter + 1);
				split_2.assign(iter + 1, setdata_new[i].end());

				entropy_temp = CalGain(tuple, split_1, split_2);		// 计算信息增益
				// C4.5 //entropy_temp = CalGainRatio(tuple, split_1, split_2, entropy_temp);

				// 选择最大信息增益
				if (entropy_max < entropy_temp) {
					entropy_max = entropy_temp;
					split_value = (iter->property[i] + (iter + 1)->property[i]) / 2;
					min_index[0] = i;
					min_index[1] = int(iter - setdata_new[i].begin());
				}
			}
		}
	}

	// 通过在所有属性的所有可能分裂点中得到最佳分裂点，利用保存的分裂点坐标，将集合划分成两个集合
	split_1.assign(setdata_new[min_index[0]].begin(), setdata_new[min_index[0]].begin() + min_index[1] + 1);
	split_2.assign(setdata_new[min_index[0]].begin() + min_index[1] + 1, setdata_new[min_index[0]].end());

	// 通过划分得到分裂点获得决策依据从而创建决策树
	DT_node = new BiTNode;
	DT_node->judge = split_value;
	DT_node->label = min_index[0];

	// 分裂得到的两个子集迭代构建决策树
	BuildDecisionTree(split_1, DT_node->lchild);
	BuildDecisionTree(split_2, DT_node->rchild);

	return 0;
}

double JudgeLable(vector<Setdata> testdata)
{
	int counter = 0;// 统计分类错误的数量
	for (vector<Setdata>::iterator iter = testdata.begin(); iter < testdata.end(); iter++)
	{
		BiTNode *temp = DT;
		while (temp)
		{
			if (temp->judge != -1){
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
	return (testdata.size() - counter) / (double)testdata.size();
}

int main()
{
	// 读取训练数据
	ReadTxt("traindata.txt");

	// 利用训练数据构建决策树
	BuildDecisionTree(setdata, DT);

	// 读取测试数据
	ReadTxt("testdata.txt");

	// 通过决策树给数据分类
	cout << "分类完成，准确率为："
		<< JudgeLable(setdata) * 100 << "%" << endl;
	system("pause");

	return 0;
}