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
	double judge;					// judgeΪ-1shi
	int label;
}*DT = NULL;

struct Setdata
{
	double property[PROPERTY_NUM];	// ���ݵļ�������ֵ
	int label;						// ���ݱ�ǩ
};

vector<Setdata> setdata;

void ReadTxt(string file)
{
	ifstream infile;
	infile.open(file.data());					// ���ļ����������ļ���������
	assert(infile.is_open());					// ����ʧ�ܣ������������Ϣ������ֹ��������
	setdata.clear();							// ������������ʼ��

	string s;
	while (getline(infile, s))
	{
		int i = 0;
		string str_temp = "";					// ��ʼ��stringΪ�գ����ü� ="" Ҳ��
		Setdata temp;

		for (int j = 0; s[j] != '\0'; j++) {	// �ָ�ÿһ�е�����
			if (s[j] != '\t') {					// ��Tab��Ϊ�ָ��
				if (s[j + 1] == '\0')
					temp.label = s[j] - 48;		// �������һ��Ԫ���Ǳ�ǩ
				else str_temp += s[j];
			}
			else {
				temp.property[i++] = atof(str_temp.c_str());	// ��string���͵�����ת���ɶ�Ӧ��double����
				str_temp.clear();
			}
		}
		setdata.push_back(temp);				// ���ָ�õ����ݼ���vector����
	}
}

/*******************************
*  Entropy(D) = -��Pi*log2(Pi)
*  Pi = label_qty / label_total;
********************************/
double CalEntropy(vector<Setdata> &tuple)
{
	double entropy = 0;							// ��
	double label_qty[LABEL_NUM] = { 0 };		// ÿ�ֱ�ǩ�ĸ���
	const double label_total = tuple.size();	// ��ǩ����
	if (label_total == 0) return 0;				// label_total ����ֵ

	for (int i = 0; i < tuple.size(); i++)
		label_qty[tuple[i].label - 1]++;		// ����ÿһ�ֱ�ǩ������

	for (int i = 0; i < LABEL_NUM; i++)			// �� i=0 n=LABEL_NUM
	{
		double pi = label_qty[i] / label_total;			// label_total ����ֵ
		if (pi) entropy += -(pi * log(pi) / log(2));	// log�Ĳ����Ƿ���ֵ
	}

	return entropy;
}

/***********************************
* gain = Entropy(D) -  ��Entropy(Ai)
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
	if (tuple.size() == 1) return true;		// ���������ֻ��һ��������Ϊ�����������
	for (vector<Setdata>::iterator iter = tuple.begin(); iter < tuple.end() - 1; iter++)
		if (iter->label != (iter + 1)->label) return false;	// ��������в�һ����Ԫ�أ�����Ϊ�ü��ϲ�����
	return true;
}

int BuildDecisionTree(vector<Setdata> &tuple, BiTNode* &DT_node)
{
	// ��һ�����ж�����ʵ���Ƿ�����ͬһ�࣬����ǣ��򵽴�Ҷ�ӽڵ�
	if (IsPure(tuple)) {
		DT_node = new BiTNode;
		DT_node->judge = -1;	// ��ʾҶ�ӽڵ�
		DT_node->label = tuple.begin()->label;
		DT_node->lchild = DT_node->rchild = NULL;
		return 0;
	}

	// �ڶ������������Ҷ�ӽڵ㣬��������п��ܷ��ѵ����Ϣ��
	vector<Setdata> setdata_new[PROPERTY_NUM];	// Ϊ�˱��������ĸ���
	vector<Setdata> split_1, split_2;

	double entropy_max = -1;					// �����Ϣ����
	double entropy_temp;
	double split_value = 0;						// ������ֹ
	int min_index[2];							// [0]�������� [1]�������±�

	for (int i = 0; i < PROPERTY_NUM; i++)		// �ڸ�������֮��Ѱ������С�Ļ���
	{
		setdata_new[i].assign(tuple.begin(), tuple.end());	// ����һ�������ĸ���������֮�󻮷�
		switch (i)	// ���������԰�����������
		{
		case 0:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro0); break;
		case 1:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro1); break;
		case 2:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro2); break;
		case 3:sort(setdata_new[i].begin(), setdata_new[i].end(), AscendSortByPro3); break;
		default:
			break;
		}

		// ��n-1�ַ��ѿ���
		for (vector<Setdata>::iterator iter = setdata_new[i].begin(); iter < setdata_new[i].end() - 1; iter++)
		{
			// ��Ԫ�ر�ǩ��ͬ������Ϣ���棬���������е�����Ԫ��֮�䣬������ټ�����
			if (iter->label == (iter + 1)->label) continue;
			else {
				// �ڿ��ܵ����ŷ��ѵ㴦�����Ϸֿ���Ϊ�˼�����Ϣ����
				split_1.assign(setdata_new[i].begin(), iter + 1);
				split_2.assign(iter + 1, setdata_new[i].end());

				entropy_temp = CalGain(tuple, split_1, split_2);		// ������Ϣ����
				// C4.5 //entropy_temp = CalGainRatio(tuple, split_1, split_2, entropy_temp);

				// ѡ�������Ϣ����
				if (entropy_max < entropy_temp) {
					entropy_max = entropy_temp;
					split_value = (iter->property[i] + (iter + 1)->property[i]) / 2;
					min_index[0] = i;
					min_index[1] = int(iter - setdata_new[i].begin());
				}
			}
		}
	}

	// ͨ�����������Ե����п��ܷ��ѵ��еõ���ѷ��ѵ㣬���ñ���ķ��ѵ����꣬�����ϻ��ֳ���������
	split_1.assign(setdata_new[min_index[0]].begin(), setdata_new[min_index[0]].begin() + min_index[1] + 1);
	split_2.assign(setdata_new[min_index[0]].begin() + min_index[1] + 1, setdata_new[min_index[0]].end());

	// ͨ�����ֵõ����ѵ��þ������ݴӶ�����������
	DT_node = new BiTNode;
	DT_node->judge = split_value;
	DT_node->label = min_index[0];

	// ���ѵõ��������Ӽ���������������
	BuildDecisionTree(split_1, DT_node->lchild);
	BuildDecisionTree(split_2, DT_node->rchild);

	return 0;
}

double JudgeLable(vector<Setdata> testdata)
{
	int counter = 0;// ͳ�Ʒ�����������
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
	// ��ȡѵ������
	ReadTxt("traindata.txt");

	// ����ѵ�����ݹ���������
	BuildDecisionTree(setdata, DT);

	// ��ȡ��������
	ReadTxt("testdata.txt");

	// ͨ�������������ݷ���
	cout << "������ɣ�׼ȷ��Ϊ��"
		<< JudgeLable(setdata) * 100 << "%" << endl;
	system("pause");

	return 0;
}