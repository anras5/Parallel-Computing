#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


struct Row
{
    int A, B, C, D, X;

    Row() : A(0), B(0), C(0), D(0), X(0) {}

    Row(int a, int b, int c, int d, int x) : A(a), B(b), C(c), D(d), X(x) {}
};

std::string *splitString(const std::string &line, char delimiter, int &numTokens);
Row *parseCSV(const std::string &filename, int &numRows);
void printRows(Row *rows, int numRows);