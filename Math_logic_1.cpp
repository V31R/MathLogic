﻿// Math_logic_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
typedef unsigned int Uint;//чтобы меньше печатать

class RPNLogicCalculator {
public:
	RPNLogicCalculator() :
		operations{//инициализация "словаря"
			{'+', [](bool a, bool b) {return bool(a | b); } },//дизъюнкция
			{'|', [](bool a, bool b) {return bool(a | b); } },//дизъюнкция
			{'*', [](bool a, bool b) {return bool(a & b); } },//конъюнкция
			{'&', [](bool a, bool b) {return bool(a & b); } },//конъюнкция
			{'>', [](bool a, bool b) {return bool(!a | b); } },//импликация
			{'=', [](bool a, bool b) {return bool((a & b) | (!a & !b)); } },//эквивалентность
			{'!', [](bool a, bool b) {return bool(!a); } },//отрицание
			{'~', [](bool a, bool b) {return bool(!a); } },//отрицание
	}
	{}
	bool calculateRPN(string str) {
		stack<bool> valStack;//создаём стек для значений

		auto pop_stack([&]() {// создаём лямбда выражение, чтобы доставать из стека значение одной строкой
			auto result(valStack.top());
			valStack.pop();
			return result;
		});
		for (Uint i{ 0 }; i < str.size(); i++) {//цикл по входной строке строке
			if (str[i] == '1' || str[i] == '0') {//если встретили значение
				valStack.push(bool(str[i] - '0'));
			}
			else {//иначе это оператор
				auto & operation(operations.at(str[i]));//берём нужную нам функцию
				bool result;
				auto right{ pop_stack() };//берём правый операнд
				if (str[i] == '!' || str[i] == '~') {//если это отрицание, то левый операнд не нужен
					result = operation(right, false);
				}
				else {
					auto left{ pop_stack() };//достаём левый операнд
					result = operation(left, right);
				}
				valStack.push(result);//кладём полученный результат обратно в стек
			}
		}
		return valStack.top();//возвращаем вычисленное значение
	}
private:
	map<char, bool(*)(bool, bool)> operations;//"словарь" для удобного вызова операций по символам операторов
};


class AnalyzerToRPN {	 
public:
	set<string> variableName;//множество имён переменных
	AnalyzerToRPN() :
		operationsPriority{//инициализация map, чтобы было удобно определять приоритет операций
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
		auto pop_stack{ [&](string & str) {//функция для записывания верхнего значения в строку
			auto result{stackForOperations.top()};
			str += result;
			stackForOperations.pop();
		} };
		string strToInsert;// строка для записывания значений в очередь
		for (Uint i{ 0 }; i< input.size(); i++) {// итерируемся по входной строке
			strToInsert.clear();//очищаем строку от остатков с предыдущей итерации
			if (isLetter(input[i])) {//если встретили букву, то это начало имени переменной
				strToInsert = getVariableName(input, i);// достаём имя
				variableName.insert(strToInsert);// добавляем имя переменной в множество, если такое уже есть, то ничего не произойдёт
				strToInsert += ' ';//добавляем пробел, чтобы в дальнейшем отличить имена разных переменных для вставки
				output.push(strToInsert);//добавляем имя переменной в очередь выхода
				i--;
			}
			else if (isConstant(input[i])) {//если втсретилось константное значение, просто добавляем его в очередь выхода
				strToInsert += input[i];
				output.push(strToInsert);
			}
			else if(isOperations(input[i])){//если операция
					while (!stackForOperations.empty() && isOperations(stackForOperations.top()) //если стек не пуст, и приоритет операции в стеке выше или равен текущей операции
						&& (operationsPriority.at(input[i]) <= operationsPriority.at(stackForOperations.top()))) {
						strToInsert.clear();
						pop_stack(strToInsert);//берём операцию
						output.push(strToInsert);//добавляем её в очередь на выход
					}
					stackForOperations.push(input[i]);//добавляем текущую операцию в стек
			}
			else if(isOpeningBracket(input[i])){//если открывающая скобка, то добавляем её в стек
				stackForOperations.push(input[i]);
			}
			else if (isClosingBracket(input[i])) {//если закрывающая скобка
				while (!stackForOperations.empty() &&!isOpeningBracket(stackForOperations.top()) ) {//перекладываем операции из стека, пока не нашлась открывающая скобка
					strToInsert.clear();
					pop_stack(strToInsert);
					output.push(strToInsert);
				}
				if (!stackForOperations.empty()) {//если стек не пуст, то выкидываем скобку
					stackForOperations.pop();
				}
				else {
					throw invalid_argument("Opening bracket is absented.\n");
					//ошибка, не было открывающей скобки
				}

			}
			else if (input[i]==' ') {/*если пробел, то ничего не делаем*/}
			else {
				throw invalid_argument("Unknown symbol.\n");
				//ошибка, был встречен символ, который не является ни частью имени переменной, ни оператором 
			}
		}
		while (!stackForOperations.empty()) {//пока стек не пуст перекладываем все операции из него
			strToInsert.clear();
			pop_stack(strToInsert);
			if (isOpeningBracket(strToInsert[0])) {//если открытая скобка
				throw invalid_argument("Closing bracket is absented.\n");
				//ошибка, есть незакрытая(ые) скобка(и)
			}
			output.push(strToInsert);
		}
		strToInsert.clear();
		while (!output.empty()) {//преваращаем очередь на выход в строку
			strToInsert += output.front();
			output.pop();
		}
		return strToInsert;
	}
	bool isLetter(char symbol) {//проверяем является ли буквой английского алфавита
		return symbol >= 'a'&&symbol <= 'z' || symbol >= 'A'&&symbol <= 'Z';
	}

	string getVariableName(string input, unsigned int &i) {
		string name;
		//итерируемся по строке, пока не встречен пробел или символ операции
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
			operationsPriority.at(symbol);//проверяем есть ли такая операция
			//если такой операции нет, то сгенерируется исключение out_of_range
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
	map<char, char> operationsPriority;//ассоциативный контейнер для операций и их приоритетов
	queue<string> output;
	stack<char> stackForOperations;
};
class LogicCalculator {
#define WIDTH 10// компилируемая константа для красивого вывода в виде таблицы
public:
	LogicCalculator(){}
	LogicCalculator(string str) {
		try {
			strWithNames = analyzer.analysingString(str);//делаем из входной строки строку с ОПН
		}
		catch (invalid_argument &ex) {//если ловим исключение, то прерываем исполнение
			cout << ex.what();
			throw runtime_error("Calculation is stopped.\n");
		}
		initMapVariable();//инициализация контйенера с именами переменных и позициями вставки
		makeStrToSubstitution();//создаём строку для вставки значений
		makeTruthTable();//считаем и выводимтаблицу истинности
	}
	void makeTask(string str){
		try {
			strWithNames = analyzer.analysingString(str);//делаем из входной строки строку с ОПН
		}
		catch (invalid_argument &ex) {//если ловим исключение, то прерываем исполнение
			cout << ex.what();
			throw runtime_error("Calculation is stopped.\n");
		}
		initMapVariable();//инициализация контйенера с именами переменных и позициями вставки
		makeStrToSubstitution();//создаём строку для вставки значений
		task();
	}
	//x1 & !x2 + x3 & x4
private:
	void task() {
		printHorizontalLine();//вывод шапки таблицы
		printNames();
		cout << setw(WIDTH) << "Result" << " |" << endl;
		printHorizontalLine();
		char flag = 0;//переменная, которая будет хранить флаги для определения общезначима, выполнима или невыполнима формула
		Uint max = 1 << variable.size();//находим значение, до которого будет идти перебор
		for (Uint binaryVector{ 0 }; binaryVector < max; binaryVector++) {//перебираем

			Uint variableNumber = variable.size() - 1;//так как эелементы в map отсортированы, то начинаем с самой левой переменной
			for (auto obj : variable) {
				Uint value = (binaryVector >> variableNumber) & 1;//извлекаем значение для текущей переменной
				for (auto valuePos : obj.second) {//вставляем в строку для подсчёта
					strToSubstitution[valuePos] = '0' + value;
				}
				variableNumber--;
			}
			bool result = calculator.calculateRPN(strToSubstitution);
			//выводим строку, если значение функции равно истине
			if (result) {
				for (int i = variable.size() - 1; i >= 0;i--) {
					cout << setw(WIDTH) << ((binaryVector >> i) & 1) << " |";
				}
				cout << setw(WIDTH) << result << " |" << endl;
				flag |= 1;//ставим флаг, что встретилось истинное значение
			}
			else {
				flag |= 2;//ставим флаг, что встретилось ложное значение
			}
		}
		printHorizontalLine();//заканчиваем таблицу

		if (flag & 2 ) {//анализируем флаги
			if (flag & 1) {
				cout << "Formula is satisfiable." << endl;
			}
			else {
				cout << "Formula is impracticable." << endl;
			}
		}
		else {
			cout << "Formula is tautology." << endl;
		}
	}
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
		printHorizontalLine();//вывод шапки таблицы
		printNames();
		cout << setw(WIDTH) << "Result" << " |" << endl;
		printHorizontalLine();
		Uint max = 1 << variable.size();//находим значение, до которого будет идти перебор
		for (Uint binaryVector{ 0 }; binaryVector < max; binaryVector++) {//перебираем
			Uint variableNumber = variable.size()-1;//так как эелементы в map отсортированы, то начинаем с самой левой переменной
			for (auto obj : variable) {
				Uint value = (binaryVector >>variableNumber ) & 1;//извлекаем значение для текущей переменной
				cout << setw(WIDTH) << value<<" |";//выводим часть строки с выражениями
				for (auto valuePos : obj.second) {//вставляем в строку для подсчёта
					strToSubstitution[valuePos] = '0' + value;
				}
				variableNumber--;
			}
			//выводим результат 
			cout << setw(WIDTH) << calculator.calculateRPN(strToSubstitution)<<" |" << endl;
		}
		printHorizontalLine();//заканчиваем таблицу
	}
	void initMapVariable() {
		for (auto obj : analyzer.variableName) {//переводим из множества имён в контейнер, где так же есть позиции
			variable.insert(std::pair<string, vector<Uint>>(obj, vector<Uint>()));
		}		
	}
	void printNames() {//вывод всех имён переменных
		for (auto obj :variable) {
			cout << setw(WIDTH) << obj.first << " |";
		}
	}
	void printHorizontalLine() {//вывод горизонтальной линии
		for (Uint i{ 0 }; i <= variable.size(); i++) {
			for (Uint j{ 0 }; j <= WIDTH+1; j++) {
				printf("-");
			}
		}
		cout << endl;
	}
	AnalyzerToRPN analyzer;//создаём объект для создания ОПН
	RPNLogicCalculator calculator;//создаём объект для подсчёта значений выражений
	string strToSubstitution;//строка, в которую будут вставлятся значения
	string strWithNames;//строка для запоминания ОПН
	map <string, vector <Uint>> variable;//контейнер для имёен переменных с массивами позиций, куда будет производится вставка значений
#undef WIDTH
};
void insturction() {
	printf("Instruction\n ! (~) - negation;\n & (*) - conjunction;\n + (|) - disjunction;\n > - material implication;\n = - equivalence;\n");
}
int main()
{
	insturction();
	cout << "Enter logic expression:\n";
	string input;
	getline(cin, input);
	LogicCalculator calculator;
	try {
		calculator.makeTask(input);
	}
	catch (runtime_error & error) {
		cout << error.what();
		return 1;
	}
	return 0;
}

