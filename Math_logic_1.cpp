// Math_logic_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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

class RPNLogicCalculator {
public:
	RPNLogicCalculator() :
		operations{
			{'+', [](bool a, bool b) {return bool(a | b); } },
			{'|', [](bool a, bool b) {return bool(a | b); } },
			{'*', [](bool a, bool b) {return bool(a & b); } },
			{'&', [](bool a, bool b) {return bool(a & b); } },
			{'>', [](bool a, bool b) {return bool(!a | b); } },
			{'=', [](bool a, bool b) {return bool((a & b) | (!a & !b)); } },
			{'!', [](bool a, bool b) {return bool(!a); } },
			{'~', [](bool a, bool b) {return bool(!a); } },
	}
	{}
	bool calculateRPN(string str) {
		stack<bool> valStack;

		auto pop_stack([&]() {
			auto result(valStack.top());
			valStack.pop();
			return result;
		});
		for (int i{ 0 }; i < str.size(); i++) {
			if (str[i] == '1' || str[i] == '0') {
				valStack.push(bool(str[i] - '0'));
			}
			else {
				auto & operation(operations.at(str[i]));
				bool result;
				auto right{ pop_stack() };
				if (str[i] == '!' || str[i] == '~') {
					result = operation(right, false);
				}
				else {
					auto left{ pop_stack() };
					result = operation(left, right);
				}
				valStack.push(result);
			}
		}
		return valStack.top();
	}
private:
	map<char, bool(*)(bool, bool)> operations;
};


class AnalyzerToRPN {	 
public:
	set<string> variableName;

	AnalyzerToRPN() :
		operationsPriority{
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
		auto pop_stack{ [&](string & str) {
			auto result{stackForOperations.top()};
			str += result;
			stackForOperations.pop();
		} };
		string temp;
		for (unsigned int i{ 0 }; i< input.size(); i++) {
			//cout << "string " << input[i] <<" "<< isOperations(input[i]) << endl;
			temp.clear();
			if (isLetter(input[i])) {
				temp = getVariableName(input, i);
			//	std::cout << temp << std::endl;
				variableName.insert(temp);
				temp += ' ';
				output.push(temp);
				i--;
			}
			else if (isConstant(input[i])) {
				temp += input[i];
				//cout << "I'm work?" << endl;
				output.push(temp);
			}
			else if(isOperations(input[i])){
					while (!stackForOperations.empty() && isOperations(stackForOperations.top()) && (operationsPriority.at(input[i]) <= operationsPriority.at(stackForOperations.top()))) {
						temp.clear();
						pop_stack(temp);
						output.push(temp);
					}
					stackForOperations.push(input[i]);
					//temp.clear();
					//temp += input[i];
					//cout << "   " << temp << endl;
					//output.push(temp);
			}
			else if(isOpeningBracket(input[i])){
				stackForOperations.push(input[i]);
			}
			else if (isClosingBracket(input[i])) {
				while (!stackForOperations.empty() &&!isOpeningBracket(stackForOperations.top()) ) {
					temp.clear();
					pop_stack(temp);
					output.push(temp);
				}
				if (!stackForOperations.empty()) {
					stackForOperations.pop();
				}
				else {
					//ошибка, не было открывающей скобки
				}

			}
			else if (input[i]==' ') {
				
				//ничего не делаем
			}
			else {
				//ошибка, был встречен символ, который не является ни именем переменной, ни оператором 
			}
		}
		while (!stackForOperations.empty()) {
			temp.clear();
			pop_stack(temp);
			if (isOpeningBracket(temp[0])) {
				//ошибка, есть незакрытые скобки
			}
			output.push(temp);
		}
		temp.clear();
		while (!output.empty()) {

			temp += output.front();
			//std::cout << temp << endl;
			output.pop();
		}
		return temp;
	}
	bool isLetter(char symbol) {
		return symbol >= 'a'&&symbol <= 'z' || symbol >= 'A'&&symbol <= 'Z';
	}

	string getVariableName(string input, unsigned int &i) {
		string name;
		while (i < input.size() && !isOperations(input[i]) && !isOpeningBracket(input[i]) && !isClosingBracket(input[i]) && input[i] != ' ') {
			name += input[i];
			i++;
		}
		return name;
	}
private:

	bool isConstant(char symbol) {
		return symbol>='0'&&symbol<='1';
	}
	bool isOperations(char symbol) {
		try {
			operationsPriority.at(symbol);
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
	map<char, char> operationsPriority;
	queue<string> output;
	stack<char> stackForOperations;
};
class LogicCalculator {
public:
	LogicCalculator(){}
	LogicCalculator(string str) {
		strWithNames=analyzer.analysingString(str);
		initMapVariable();
		makeStrToSubstitution();
		makeTruthTable();
	}

private:
	void makeStrToSubstitution() {
		unsigned int writer{ 0 };
		for (unsigned int i{ 0 }; i < strWithNames.size(); i++) {
			if (analyzer.isLetter(strWithNames[i])) {
				//unsigned int j{ i };
				string name = analyzer.getVariableName(strWithNames, i);
				variable[name].push_back(writer);
				strToSubstitution += ' ';
			}
			else {
				strToSubstitution += strWithNames[i];
			}
			writer++;
		}
		/*cout << strToSubstitution << endl;
		for (auto obj : variable) {
			cout << obj.first << endl;
			for (auto x : obj.second) {
				cout <<"   "<< x << endl;
			}
		}*/
	}
	void makeTruthTable() {
		printHorizontalLine();
		printNames();
		cout << setw(10) << "Result" << " |" << endl;
		printHorizontalLine();
		unsigned int max = 1 << variable.size();
		for (unsigned int binaryVector{ 0 }; binaryVector < max; binaryVector++) {
			unsigned int mask = variable.size()-1;
			for (auto obj : variable) {
				unsigned int value = (binaryVector >>mask ) & 1;
				cout << setw(10) << value<<" |";
				for (auto valuePos : obj.second) {
					strToSubstitution[valuePos] = '0' + value;
				}
				mask--;
			}
			cout << setw(10) << calculator.calculateRPN(strToSubstitution)<<" |" << endl;
		}
		printHorizontalLine();
	}
	void initMapVariable() {
		//printHorizontalLine();
		for (auto obj : analyzer.variableName) {
			//std::pair<string, vector<int>>
			variable.insert(std::pair<string, vector<int>>(obj, vector<int>()));
			//cout << setw(10) << obj<<" |";
		}
		//cout << setw(10) <<"Result"<<" |"<<endl;
		//printHorizontalLine();
	}
	void printNames() {
		for (auto obj :variable) {
			cout << setw(10) << obj.first << " |";
		}
	}
	void printHorizontalLine() {
		for (int i{ 0 }; i <= variable.size(); i++) {
			for (int j{ 0 }; j <= 11; j++) {
				cout << "-";
			}
		}
		cout << endl;
	}
	AnalyzerToRPN analyzer;
	RPNLogicCalculator calculator;
	string strToSubstitution;
	string strWithNames;
	map <string, vector <int>> variable;
};

int main()
{
	string input;
	getline(cin, input);
	//cin.get(input);
	//1&X1+X2*X3>X4*X1+(X1&X2)*0
	//AnalyzerToRPN test;
	//string t{ "(A+B)*B" };
	//std::cout << test.analysingString(t) << std::endl;
	LogicCalculator calculator(input);
	//string test{ "1!0+!1&!" };

	//std::cout << calculateRPN(test.analysingString(t));
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
