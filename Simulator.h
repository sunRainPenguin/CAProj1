//模拟汇编代码执行过程
#pragma once
#include "code.h"
#include "data.h"
#include "Util.h"

class Simulator
{
public:
	Simulator();
	~Simulator();

	void reset();
	void setInstructionAndData(vector<code*> instructions, vector<data*> dataArray);
	void translate();

private:
	int cycle;
	int dataBegAdd;
	int dataCount;
	string currentCycleStr;
	string disassembly;
	vector<code*> instructions;
	vector<data*> dataArray;
	vector<int> registers;
	vector<int> dataVal;

	ofstream simulationFile;

	void print(int cycleNum, int lineNum, string instrString);
};

