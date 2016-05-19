#include "Simulator.h"


Simulator::Simulator()
{
	simulationFile.open("simulation.txt", ios::app);
	reset();
}


Simulator::~Simulator()
{
	simulationFile.close();
}

void Simulator::reset()
{
	cycle = 0;
	disassembly = "";
	currentCycleStr = "";
	registers.clear();
	dataVal.clear();
}
void Simulator::setInstructionAndData(vector<code*> instructions, vector<data*> dataArray)
{

	this->instructions = instructions;
	this->dataArray = dataArray;

	int index = 0;
	for (std::vector<data*>::iterator iter = dataArray.begin(); iter != dataArray.end(); iter++)		//初始化dataVal数组和registers数组
	{
		/*dataVal[index] = (*iter)->dataValue;*/
		dataVal.push_back((*iter)->dataValue);
		index++;
	}
	dataBegAdd = codeBegAdd + 4 * instructions.size();
	dataCount = dataArray.size();
	for (int i = 0; i < 32; i++)
		registers.push_back(0);
}
void Simulator::translate()
{
	cycle = 0;
	int rd;
	int rt;
	int rs;
	int sa;
	int instrIndex=codeBegAdd;
	int imme;
	int target;
	int offset;
	int base;
	string funcode;
	string opcode;
	string instrTxt;
	int category;

	int lineInFile = 0;		//文件中的第几行指令，不包括数据
	int dataIndex = 0;		//文件中第几个数据
	while (instructions.size() != 0)
	{
		cycle++;
		lineInFile = (instrIndex - 64) / 4;
		instrIndex = instructions[lineInFile]->line;
		opcode = instructions[lineInFile]->opcode;
		category = instructions[lineInFile]->category;
		instrTxt = instructions[lineInFile]->instr_text;

		if (opcode == "BREAK")
		{
			print(cycle, instrIndex, instrTxt);
			break;
		}

		if (category == 2)
		{
			rs = instructions[lineInFile]->rs;
			rt = instructions[lineInFile]->rt;
			rd = instructions[lineInFile]->rd;
			instrIndex = instructions[lineInFile]->line;
			imme = instructions[lineInFile]->immediate;
			funcode = instructions[lineInFile]->functionCode;

			if (funcode == "ADD")
				registers[rt] = registers[rs] + imme;
			if (funcode=="SUB")
				registers[rt] = registers[rs] - imme;
			if (funcode=="MUL")
				registers[rt] = registers[rs] * imme;
			if (funcode == "AND")
				registers[rt] = registers[rs] & imme;
			if (funcode == "NOR")
				registers[rt] = ~(registers[rs] | imme);
			if (funcode == "SLT")
			{
				if (registers[rs] < registers[rt])
					registers[rd] = 1;
				else
					registers[rd] = 0;
			}

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode  == "ADD")
		{
			rd = instructions[lineInFile]->rd;
			rs = instructions[lineInFile]->rs;
			rt = instructions[lineInFile]->rt;
			registers[rd] = registers[rs] + registers[rt];

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "SUB")
		{
			rd = instructions[lineInFile]->rd;
			rs = instructions[lineInFile]->rs;
			rt = instructions[lineInFile]->rt;
			registers[rd] = registers[rs] - registers[rt];

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "MUL")
		{
			rd = instructions[lineInFile]->rd;
			rs = instructions[lineInFile]->rs;
			rt = instructions[lineInFile]->rt;
			registers[rd] = registers[rs] * registers[rt];

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "J")
		{
			target = instructions[lineInFile]->target_instr_index;

			print(cycle, instrIndex, instrTxt);
			instrIndex = target;
			continue;
		}
		if (opcode == "JR")
		{
			rs = instructions[lineInFile]->rs;

			print(cycle, instrIndex, instrTxt);
			instrIndex = registers[rs];
			continue;
		}
		if (opcode == "BEQ")
		{
			rs = instructions[lineInFile]->rs;
			rt = instructions[lineInFile]->rt;
			offset = instructions[lineInFile]->offset;		//这里的offset之前乘过4了

			print(cycle, instrIndex, instrTxt);
			if (registers[rs] == registers[rt])
				instrIndex = instrIndex + offset + 4;
			else
				instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "BLTZ")
		{
			rs = instructions[lineInFile]->rs;
			offset = instructions[lineInFile]->offset;		//这里的offset之前乘过4了

			print(cycle, instrIndex, instrTxt);
			if (registers[rs] <0)
				instrIndex = instrIndex + offset + 4;
			else
				instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "BGTZ")
		{
			rs = instructions[lineInFile]->rs;
			offset = instructions[lineInFile]->offset;		//这里的offset之前乘过4了

			print(cycle, instrIndex, instrTxt);
			if (registers[rs] >0)
				instrIndex = instrIndex + offset + 4;
			else
				instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "SW")
		{
			rt = instructions[lineInFile]->rt;
			offset = instructions[lineInFile]->offset;		//这里的offset之前乘过4了
			base = instructions[lineInFile]->base;
			dataIndex = (registers[base] + offset - dataBegAdd) / 4;
			dataVal[dataIndex] = registers[rt];

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "LW")
		{
			rt = instructions[lineInFile]->rt;
			offset = instructions[lineInFile]->offset;		//这里的offset之前乘过4了
			base = instructions[lineInFile]->base;
			dataIndex = (registers[base] + offset - dataBegAdd) / 4;
			registers[rt] = dataVal[dataIndex];

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "SLL")
		{
			rt = instructions[lineInFile]->rt;
			rd = instructions[lineInFile]->rd;
			sa = instructions[lineInFile]->sa;
			registers[rd] = registers[rt] << sa;

			print(cycle, instrIndex, instrTxt);		//左移的话逻辑/算术相等
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "SRL")
		{
			rt = instructions[lineInFile]->rt;
			rd = instructions[lineInFile]->rd;
			sa = instructions[lineInFile]->sa;

			bitset<32> tempBitset = registers[rt];
			bitset<32> tempBitsetR = tempBitset >> sa;
			registers[rd] = tempBitsetR.to_ulong();

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "SRA")
		{
			rt = instructions[lineInFile]->rt;
			rd = instructions[lineInFile]->rd;
			sa = instructions[lineInFile]->sa;
			registers[rd] = registers[rt] >> sa;

			print(cycle, instrIndex, instrTxt);
			instrIndex = instrIndex + 4;
			continue;
		}
		if (opcode == "NOP")
			continue;
	}

}

void Simulator::print(int cycleNum, int lineNum, string instrString)
{
	simulationFile << "--------------------" << '\n';
	simulationFile << "Cycle:" << cycleNum << "\t" << lineNum << "\t" << Util::replaceFirstOf(instrString, " ", "\t") << '\n';
	simulationFile << '\n';
	simulationFile << "Registers" << '\n';
	simulationFile << "R00:" ;
	for (int i = 0; i < 16; i++)
	{
		simulationFile << '\t' << registers[i];
	}
	simulationFile << '\n';
	simulationFile << "R16:";
	for (int i = 16; i < 32; i++)
	{
		simulationFile << '\t' << registers[i];
	}
	simulationFile << '\n';
	simulationFile << '\n';

	simulationFile << "Data" << '\n';
	double temp = (double)dataCount / (double)8;
	for (int i = 0; i < (ceil(temp))-1; i++)
	{
		simulationFile << Util::int2string(dataBegAdd+i*32) << ':';
		for (int j = 0+8*i; j < 8+8*i; j++)
		{
			simulationFile << '\t' << dataVal[j];
		}
		simulationFile << '\n';
	}
	//打印最后一行
	int lastLine = ceil(temp);
	simulationFile << Util::int2string(dataBegAdd + (lastLine - 1) * 32) << ':';
	for (int j = 8*(lastLine-1); j < dataCount; j++)
	{
		simulationFile << '\t' << dataVal[j];
	}
	simulationFile << '\n';
	simulationFile << '\n';
	simulationFile.flush();
}

