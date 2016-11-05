#include <iostream>
#include "calc.h"

using namespace std;

void printA();
void printB();
void printC();

int main(void)
{
	cout << "hello, this is Start!" << endl;

	cout << "Add: " << Add(80, 20) << endl;
	
	cout << "Sub: " << Sub(50, 20) << endl;

	printA();
	printB();
	printC();

	return 0;
}
