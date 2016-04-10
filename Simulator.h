//汇编代码执行的模拟
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
	string currentCycleStr;
	string disassembly;
	vector<code*> instructions;
	vector<data*> dataArray;
	int registers[regCount];
	int dataVal[dataCount];

	ofstream simulationFile;

	void print(int cycleNum, int lineNum, string instrString);
};

