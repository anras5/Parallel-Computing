#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

struct Row
{
    int *values;
    int numValues;

    Row() : values(nullptr), numValues(0) {}

    Row(int *vals, int count) : numValues(count)
    {
        values = new int[numValues];
        for (int i = 0; i < numValues; i++)
        {
            values[i] = vals[i];
        }
    }

    ~Row()
    {
        delete[] values;
    }
};

std::string *splitString(const std::string &line, char delimiter, int &numTokens);
Row *parseCSV(const std::string &filename, int &numRows);
void printRows(Row *rows, int numRows);