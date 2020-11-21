// Math_logic_3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "calculator.h"
typedef signed char Letter;
typedef vector<Letter> Disjunct;
class PСNF {
public:
	set<string> variableNames;
	vector<Uint> disjuncts;//массив для хранения СKНФ
	vector<bool> truthTableValues;//массив со значениями таблицы истинности
	PСNF() {
		disjuncts.resize(0);
	}
	void getTruthTableFromExpression(string & expr) {//считаем значения таблицы истинности для полученного выражения
		try {
			LogicCalculator calculator(expr);
			truthTableValues = calculator.getTableValues();
			variableNames = calculator.getVariableNames();
		}
		catch (...) {
			throw runtime_error("Calculation is stopped.\n");
		}
	}
	void make() {//из значений таблицы истинности получаем сднф
		for (Uint binaryVector{ 0 }; binaryVector < truthTableValues.size(); binaryVector++) {
			if (!truthTableValues[binaryVector]) {
				disjuncts.push_back(~binaryVector);
			}
		}
	}
};
class Resolution {//класс для нахождения всех формул, для которых одна формула является следствием
public:
	Resolution() {
		PCNFs.resize(0);
	}
	void clear() {
		PCNFs.clear();
		variableNames.clear();
		disjunctSet.clear();
	}
	
	void inputExpression() {
		getPremises();
		getConsequences();
	}
	void makeStartDisjunctSet() {
		makeNameSet();
		for (int i{ 0 }; i < PCNFs.size(); i++) {//итерируемся по всем скнф
			for (int j{ 0 }; j < PCNFs[i].disjuncts.size(); j++) {//итерируемся по дизъюнктам внутри скнф
				Disjunct temp;
				temp.resize(variableNames.size());
				int pos{ 0 };
				for (auto name : variableNames) {
					if (PCNFs[i].variableNames.find(name) != PCNFs[i].variableNames.end()) {//если такая переменная есть 
						Uint curPos{ PCNFs[i].variableNames.size()};
						for (auto vName : PCNFs[i].variableNames) {//ищем значение переменной
							curPos--;
							
							if (vName == name) {
								temp[pos] = ((PCNFs[i].disjuncts[j] >> curPos) & 1) > 0?1:-1;
								break;
							}
						}
					}
					else {
						temp[pos] = 0;
					}
					pos++;
				}
				insert(temp);
			}
		}
	}
	void startResolution() {
		cout << "Set of disjuncts: " << endl;
		printDisjuncts();
		for (int i{ 0 }; i < disjunctSet.size(); i++) {
			for (int j{ 0 }; j < disjunctSet.size(); j++) {
				if (i != j) {
					bool flag{ false };
					for (int k{ 0 }; k < variableNames.size(); k++) {
						if (disjunctSet[i][k] * disjunctSet[j][k] == -1) {//если они разных знаков, то можно склеить
							flag = true;
							break;
						}
					}
					if (flag) {//если есть, что склеивать
						Disjunct temp;
						temp.resize(variableNames.size());
						bool isNull{ true };
						for (int k{ 0 }; k < variableNames.size(); k++) {//склеиваем дизъюнкты
							temp[k] = disjunctSet[i][k] + disjunctSet[j][k] >= 1 ? 1 : 0;
							temp[k] = disjunctSet[i][k] + disjunctSet[j][k] < 0 ? -1 : temp[k];
							if (temp[k]) {
								isNull = false;
							}
						}
						if (insert(temp)) {//если элемент был добавлен в множество то выводим из чего был получен
							cout << "Disjunct ";
							printDisjunct(temp);
							cout << " was received as a result of gluing the disjuncts ";
							printDisjunct(disjunctSet[i]);
							cout << " and ";
							printDisjunct(disjunctSet[j]);
							cout << endl;
						}
						if (isNull) {//если пустой дизъюнкт, то теорема доказана
							cout << "Was received an empty disjunct.The theorem is proved." << endl;
							return;
						}
					}
				}
			}
		}
		cout << "The theorem is disproved." << endl;
	}
private:
	void makeNameSet() {
		for (int i{ 0 }; i < PCNFs.size(); i++) {
			for (auto name : PCNFs[i].variableNames) {
				variableNames.insert(name);
			}
		}
	}
	void getConsequences() {//ввод заключения
		cout << "Enter consequnces" << endl;
		string input;
		getline(cin, input);
		input = "!(" + input + ")";
		PСNF temp{};
		try {
			temp.getTruthTableFromExpression(input);//сразу же делаем скнф и обрабатываем ошибки ввода
		}
		catch (runtime_error & ex) {
			throw ex;
		}
		temp.make();
		temp.truthTableValues.clear();
		PCNFs.push_back(temp);
	}
	void getPremises() {//ввод всех посылок
		cout << "Enter premisions, to stop input enter \\" << endl;
		string input;
		do {
			getline(cin, input);
			if (input != string{ "\\" }) {
				PСNF temp{};
				try {
					temp.getTruthTableFromExpression(input);//сразу же делаем скнф и обрабатываем ошибки ввода
				}
				catch (runtime_error & ex) {
					throw ex;
				}
				temp.make();
				temp.truthTableValues.clear();
				PCNFs.push_back(temp);
			}
		} while (input != string{ "\\" });
	}

	bool inSet(Disjunct & d) {//проверка, есть ли дизъюнкт в множестве
		for (auto el:disjunctSet) {
			bool flag{ false };
			for (int i{ 0 };i< el.size(); i++) {
				if (el[i] != d[i]) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				return true;
			}
		}
		return false;
	}
	bool insert(Disjunct &disjunct) {//добавление дизъюнкта в множество дизъюнктов, если его там нет
		if (!inSet(disjunct)) {
			disjunctSet.push_back(disjunct);
			return true;
		}
		return false;
	}
	
	void printDisjuncts() {//вывод множемства дизъюнктов
		cout << "{ ";
		for (int i{ 0 }; i < disjunctSet.size(); i++) {
			printDisjunct(disjunctSet[i]);
			if (i < disjunctSet.size() - 1) {
				cout << ", ";
			}
		}
		cout << "}" << endl;
	}
	void printDisjunct(Disjunct & d) {//вывод дизъюнкта
		int pos{ 0 };
		bool flag = false;
		cout << "(";
		for (auto name : variableNames) {
			if (d[pos]) {
				if (pos>=1&&flag) {
					cout << "+";
				}
				if (d[pos] == -1) {
					cout << "!";
				}
				cout << name;
				flag = true;
			}
			pos++;
		}
		cout << ")";
	}

	set<string> variableNames;//множество имён переменных
	vector<PСNF> PCNFs;//массив всех совершенных конъюктивных нормальных форм
	vector <Disjunct> disjunctSet;//множество дизъюнктов
};
int main()
{	
	insturction();
	Resolution resolution;
	try {
		resolution.inputExpression();
	}
	catch (runtime_error & ex) {
		cout << ex.what() << endl;
		return 1;
	}
	resolution.makeStartDisjunctSet();
	resolution.startResolution();
	return 0;
}
/*
!A+B
\
A+!B
*/
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
