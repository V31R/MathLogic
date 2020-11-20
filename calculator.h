#pragma once
// Math_logic_1.cpp : ���� ���� �������� ������� "main". ����� ���������� � ������������� ���������� ���������.
//

#include "pch.h"
#include <iostream>
#include <iomanip>
#include <stack>
#include <map>
#include <string>
#include <queue>
#include <set>
#include <stdexcept>

using namespace std;
typedef unsigned int Uint;//����� ������ ��������

class RPNLogicCalculator {
public:
	RPNLogicCalculator() :
		operations{//������������� "�������"
			{'+', [](bool a, bool b) {return bool(a | b); } },//����������
			{'|', [](bool a, bool b) {return bool(a | b); } },//����������
			{'*', [](bool a, bool b) {return bool(a & b); } },//����������
			{'&', [](bool a, bool b) {return bool(a & b); } },//����������
			{'>', [](bool a, bool b) {return bool(!a | b); } },//����������
			{'=', [](bool a, bool b) {return bool((a & b) | (!a & !b)); } },//���������������
			{'!', [](bool a, bool b) {return bool(!a); } },//���������
			{'~', [](bool a, bool b) {return bool(!a); } },//���������
	}
	{}
	bool calculateRPN(string str) {
		stack<bool> valStack;//������ ���� ��� ��������

		auto pop_stack([&]() {// ������ ������ ���������, ����� ��������� �� ����� �������� ����� �������
			auto result(valStack.top());
			valStack.pop();
			return result;
		});
		for (Uint i{ 0 }; i < str.size(); i++) {//���� �� ������� ������ ������
			if (str[i] == '1' || str[i] == '0') {//���� ��������� ��������
				valStack.push(bool(str[i] - '0'));
			}
			else {//����� ��� ��������
				auto & operation(operations.at(str[i]));//���� ������ ��� �������
				bool result;
				if (valStack.size() == 0) {
					throw invalid_argument("Invalid input.\n");
				}
				auto right{ pop_stack() };//���� ������ �������
				if (str[i] == '!' || str[i] == '~') {//���� ��� ���������, �� ����� ������� �� �����
					result = operation(right, false);
				}
				else {
					if (valStack.size() == 0) {
						throw invalid_argument("Invalid input.\n");
					}
					auto left{ pop_stack() };//������ ����� �������
					result = operation(left, right);
				}
				valStack.push(result);//����� ���������� ��������� ������� � ����
			}
		}

		return valStack.top();//���������� ����������� ��������
	}
private:
	map<char, bool(*)(bool, bool)> operations;//"�������" ��� �������� ������ �������� �� �������� ����������
};


class AnalyzerToRPN {
public:
	set<string> variableName;//��������� ��� ����������
	AnalyzerToRPN() :
		operationsPriority{//������������� map, ����� ���� ������ ���������� ��������� ��������
			{'!', 5 },
			{'~', 5 },
			{'*', 4},
			{'&', 4 },
			{'+', 3},
			{'|', 3 },
			{'>', 2 },
			{'=', 1},
	}
	{}
	string analysingString(string input) {
		auto pop_stack{ [&](string & str) {//������� ��� ����������� �������� �������� � ������
			auto result{stackForOperations.top()};
			str += result;
			stackForOperations.pop();
		} };
		string strToInsert;// ������ ��� ����������� �������� � �������
		for (Uint i{ 0 }; i < input.size(); i++) {// ����������� �� ������� ������
			strToInsert.clear();//������� ������ �� �������� � ���������� ��������
			if (isLetter(input[i])) {//���� ��������� �����, �� ��� ������ ����� ����������
				strToInsert = getVariableName(input, i);// ������ ���
				variableName.insert(strToInsert);// ��������� ��� ���������� � ���������, ���� ����� ��� ����, �� ������ �� ���������
				strToInsert += ' ';//��������� ������, ����� � ���������� �������� ����� ������ ���������� ��� �������
				output.push(strToInsert);//��������� ��� ���������� � ������� ������
				i--;
			}
			else if (isConstant(input[i])) {//���� ����������� ����������� ��������, ������ ��������� ��� � ������� ������
				strToInsert += input[i];
				output.push(strToInsert);
			}
			else if (isOperations(input[i])) {//���� ��������
				while (!stackForOperations.empty() && isOperations(stackForOperations.top()) //���� ���� �� ����, � ��������� �������� � ����� ���� ��� ����� ������� ��������
					&& (operationsPriority.at(input[i]) <= operationsPriority.at(stackForOperations.top()))) {
					strToInsert.clear();
					pop_stack(strToInsert);//���� ��������
					output.push(strToInsert);//��������� � � ������� �� �����
				}
				stackForOperations.push(input[i]);//��������� ������� �������� � ����
			}
			else if (isOpeningBracket(input[i])) {//���� ����������� ������, �� ��������� � � ����
				stackForOperations.push(input[i]);
			}
			else if (isClosingBracket(input[i])) {//���� ����������� ������
				while (!stackForOperations.empty() && !isOpeningBracket(stackForOperations.top())) {//������������� �������� �� �����, ���� �� ������� ����������� ������
					strToInsert.clear();
					pop_stack(strToInsert);
					output.push(strToInsert);
				}
				if (!stackForOperations.empty()) {//���� ���� �� ����, �� ���������� ������
					stackForOperations.pop();
				}
				else {
					throw invalid_argument("Opening bracket is absented.\n");
					//������, �� ���� ����������� ������
				}

			}
			else if (input[i] == ' ') {/*���� ������, �� ������ �� ������*/ }
			else {
				throw invalid_argument("Unknown symbol.\n");
				//������, ��� �������� ������, ������� �� �������� �� ������ ����� ����������, �� ���������� 
			}
		}
		while (!stackForOperations.empty()) {//���� ���� �� ���� ������������� ��� �������� �� ����
			strToInsert.clear();
			pop_stack(strToInsert);
			if (isOpeningBracket(strToInsert[0])) {//���� �������� ������
				throw invalid_argument("Closing bracket is absented.\n");
				//������, ���� ����������(��) ������(�)
			}
			output.push(strToInsert);
		}
		strToInsert.clear();
		while (!output.empty()) {//����������� ������� �� ����� � ������
			strToInsert += output.front();
			output.pop();
		}
		return strToInsert;
	}
	bool isLetter(char symbol) {//��������� �������� �� ������ ����������� ��������
		return symbol >= 'a'&&symbol <= 'z' || symbol >= 'A'&&symbol <= 'Z';
	}

	string getVariableName(string input, unsigned int &i) {
		string name;
		//����������� �� ������, ���� �� �������� ������ ��� ������ ��������
		while (i < input.size() && !isOperations(input[i]) && !isOpeningBracket(input[i]) && !isClosingBracket(input[i]) && input[i] != ' ') {
			name += input[i];
			i++;
		}
		return name;
	}
private:

	bool isConstant(char symbol) {
		return symbol >= '0'&&symbol <= '1';
	}
	bool isOperations(char symbol) {
		try {
			operationsPriority.at(symbol);//��������� ���� �� ����� ��������
			//���� ����� �������� ���, �� ������������� ���������� out_of_range
		}
		catch (out_of_range & exception) {
			return false;
		}
		return true;
	}
	bool isOpeningBracket(char symbol) {
		return symbol == '(';
	}
	bool isClosingBracket(char symbol) {
		return symbol == ')';
	}
	map<char, char> operationsPriority;//������������� ��������� ��� �������� � �� �����������
	queue<string> output;
	stack<char> stackForOperations;
};
class LogicCalculator {

public:
	LogicCalculator() {}
	LogicCalculator(string str) :
		truthTableValues{0}
	{
		try {
			strWithNames = analyzer.analysingString(str);//������ �� ������� ������ ������ � ���
		}
		catch (invalid_argument &ex) {//���� ����� ����������, �� ��������� ����������
			cout << ex.what();
			throw runtime_error("Calculation is stopped.\n");
		}
		initMapVariable();//������������� ���������� � ������� ���������� � ��������� �������
		makeStrToSubstitution();//������ ������ ��� ������� ��������
		try {
			makeTruthTable();//������� � �������������� ����������
		}
		catch (invalid_argument &ex) {
			cout << ex.what();
			throw logic_error{""};
		}
	}
	
	set<string> getVariableNames() { return analyzer.variableName; }
	vector<bool> getTableValues() { return truthTableValues; }
private:
	
	void makeStrToSubstitution() {
		Uint writer{ 0 };
		for (Uint i{ 0 }; i < strWithNames.size(); i++) {
			if (analyzer.isLetter(strWithNames[i])) {
				string name = analyzer.getVariableName(strWithNames, i);
				variable[name].push_back(writer);
				strToSubstitution += ' ';
			}
			else {
				strToSubstitution += strWithNames[i];
			}
			writer++;
		}
	}
	void makeTruthTable() {
		
		Uint max = 1 << variable.size();//������� ��������, �� �������� ����� ���� �������
		truthTableValues.resize(max);
		for (Uint binaryVector{ 0 }; binaryVector < max; binaryVector++) {//����������
			Uint variableNumber = variable.size() - 1;//��� ��� ��������� � map �������������, �� �������� � ����� ����� ����������
			for (auto obj : variable) {
				Uint value = (binaryVector >> variableNumber) & 1;//��������� �������� ��� ������� ����������
				for (auto valuePos : obj.second) {//��������� � ������ ��� ��������
					strToSubstitution[valuePos] = '0' + value;
				}
				variableNumber--;
			}
			//���������� ��������� 
			try {
				truthTableValues[binaryVector] = calculator.calculateRPN(strToSubstitution);
			}
			catch (invalid_argument & ex) {
				cout << ex.what();
				return;
			}
		}
	}
	void initMapVariable() {
		for (auto obj : analyzer.variableName) {//��������� �� ��������� ��� � ���������, ��� ��� �� ���� �������
			variable.insert(std::pair<string, vector<Uint>>(obj, vector<Uint>()));
		}
	}

	AnalyzerToRPN analyzer;//������ ������ ��� �������� ���
	RPNLogicCalculator calculator;//������ ������ ��� �������� �������� ���������
	string strToSubstitution;//������, � ������� ����� ���������� ��������
	string strWithNames;//������ ��� ����������� ���
	map <string, vector <Uint>> variable;//��������� ��� ��� ���������� � ��������� �������, ���� ����� ������������ ������� ��������
	vector<bool> truthTableValues;
#undef WIDTH
};


void insturction() {
	printf("Instruction\n ! (~) - negation;\n & (*) - conjunction;\n + (|) - disjunction;\n > - material implication;\n = - equivalence;\n");
}


