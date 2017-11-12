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
				/// OneNote!!
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

double CalEntropy(vector<Setdata> tuple)
{
	// info(D) = -��Pi*log2(Pi)

	double entropy = 0;							// ��
	double label_qty[LABEL_NUM] = {0};			// ÿ�ֱ�ǩ�ĸ���
	const double label_total = tuple.size();	// ��ǩ����
	
	if (label_total == 0) return 0;

	for (int i = 0; i < tuple.size(); i++) 
		label_qty[tuple[i].label - 1]++;		// ����ÿһ�ֱ�ǩ������
	
	for (int i = 0; i < LABEL_NUM; i++)
	{
		double pi = label_qty[i] / label_total;// label_total ����ֵ
		if (pi) entropy += -(pi * log(pi) / log(2));	// log�Ĳ����Ƿ���ֵ
	}

	/* ======== ����Ϊ test ========
	cout << "entropy=" << entropy;
	======== ����Ϊ test ======== */
	return entropy;
}

double CalGain(vector<Setdata> tuple,vector<Setdata> cond ) // Ԫ�� �� ���
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
	// ��һ�����ж�����ʵ���Ƿ�����ͬһ�࣬����ǣ��򵽴�Ҷ�ӽڵ�
	if (IsPure(tuple)) {
		DT_node = new BiTNode;
		DT_node->judge = -1;	// ��ʾҶ�ӽڵ�
		DT_node->label = tuple.begin()->label;
		DT_node->lchild = DT_node->rchild = NULL;
		return 0;
	}

	// �ڶ������������Ҷ�ӽڵ㣬��������п��ܷ��ѵ����Ϣ��
	vector<Setdata> setdata_new[PROPERTY_NUM];
	vector<Setdata> split_1, split_2;

	double entropy_min = 10, entropy_temp;
	double split_value = 0;
	int min_index[2] ; // [0]�������� [1]�������±�

	// �ڸ�������֮��Ѱ������С�Ļ���
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

	/* ======================== ����Ϊ test ========================

	cout << "�˴���ѷ��ѵ�������" << min_index[0] << "���±�" 
		<< setw(2) << min_index[1] << "��" << setw(2) << min_index[1] + 1 
		<< "֮�䣬������ֵΪ " << setw(6) << split_value;

	======================== ����Ϊ test ======================== */
	
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
	// ��ȡѵ������
	ReadTxt("traindata.txt");

	// ����ѵ�����ݹ���������
	BuildDecisionTree_C4_5(setdata, DT);
	
	// ��ȡ��������
	setdata.clear();
	ReadTxt("testdata.txt");

	// ͨ�������������ݷ���
	cout << JudgeLable(setdata);

	return 0;
}