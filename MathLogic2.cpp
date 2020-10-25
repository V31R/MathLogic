// MathLogic2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "calculator.h"
//ограничения по вводимой формуле нет, так как используются классы из первой лабораторной работе, которые считают значения любого выражения
class PremisesFromConsequences {//класс для нахождения всех формул, для которых одна формула является следствием
public:
	PremisesFromConsequences() {
		PCNF.resize(0);
		missingClausses.resize(0);
	}
	void clear() {
		PCNF.clear();
		missingClausses.clear();
		truthTableValues.clear();
		variableNames.clear();
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
	void makePNFs() {//из значений таблицы истинности получаем скнф и сразу же недостающие дизъюнкты
		for (Uint binaryVector{ 0 }; binaryVector < truthTableValues.size(); binaryVector++) {
			if (truthTableValues[binaryVector]) {
				missingClausses.push_back(binaryVector);
			}
			else {
				PCNF.push_back(binaryVector);
			}
		}
	}
	void makePremises() {//для создания всех формул, дял которых исходная формула явлется следствие используется алгоритм порождения подмножеств
		vector<bool> premisesCombination;
		for (int i{ 0 }; i < missingClausses.size(); i++) {
			premisesCombination.push_back(false);
		}
		cout << "All premises from consequence:" << endl;
		makePremisesRec(premisesCombination, 0);
	}
private:
	void makePremisesRec(vector<bool> premisesCombination, int i) {
		for (int x{ 0 }; x <= 1; x++) {
			premisesCombination[i] = x;
			if (i == premisesCombination.size() - 1) {
				printPCNF();
				printCombination(premisesCombination);
				printf("\n");
			}
			else {
				makePremisesRec(premisesCombination, i + 1);
			}
		}
	}
	void printCombination(vector<bool> premisesCombination) {//вывод всех дизъюнтков в подмножестве
		for (int i{ 0 }; i < premisesCombination.size(); i++) {
			if (premisesCombination[i]) {
				printf("&");
				printClause(missingClausses[i]);
			}
		}
	}
	void printClause(Uint clause) {//вывод одного дизъюнкта
		printf("(");
		int i = variableNames.size() - 1;
		for (auto & name : variableNames) {
			if ((clause >> i) & 1) {
				printf("!");
			}
			cout << name;
			printf("+");
			i--;

		}
		printf("\b)");
	}
	void printPCNF() {//вывод СКНФ
		for (int i{ 0 }; i < PCNF.size(); i++, i < PCNF.size() ? printf("&") : 0) {
			printClause(PCNF[i]);
		}
	}
	vector<Uint> PCNF;//массив для хранения СКНФ
	vector<Uint> missingClausses;//массив для хранения недостающих дизъюнктов
	vector<bool> truthTableValues;//массив со значениями таблицы истинности
	set<string> variableNames;//множество имён переменных
};

int main()
{
	insturction();
	cout << "Enter logic expression:\n";
	string input;
	getline(cin, input);
	PremisesFromConsequences premises;
	try {
		premises.getTruthTableFromExpression(input);
		premises.makePNFs();
		premises.makePremises();
	}
	catch (runtime_error & error) {
		cout << error.what();
		getchar();
		return 1;
	}
	getchar();
	return 0;
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
