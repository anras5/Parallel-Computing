#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <omp.h>

struct Row
{
    int A, B, C, D, X;

    Row() : A(0), B(0), C(0), D(0), X(0) {}

    Row(int a, int b, int c, int d, int x) : A(a), B(b), C(c), D(d), X(x) {}
};

std::string *splitString(const std::string &line, char delimiter, int &numTokens)
{
    std::stringstream ss(line);
    std::string token;
    numTokens = 0;

    while (std::getline(ss, token, delimiter))
    {
        numTokens++;
    }

    std::string *tokens = new std::string[numTokens];
    ss.clear();
    ss.seekg(0);
    int i = 0;

    while (std::getline(ss, token, delimiter))
    {
        tokens[i++] = token;
    }

    return tokens;
}

Row *parseCSV(const std::string &filename, int &numRows)
{
    std::ifstream file(filename);
    std::string line;
    numRows = 0;

    if (!file.is_open())
    {
        std::cout << "Failed to open the file: " << filename << std::endl;
        return nullptr;
    }

    // Count the number of rows in the file
    while (std::getline(file, line))
    {
        numRows++;
    }

    // Allocate memory for rows
    Row *rows = new Row[numRows];
    file.clear();
    file.seekg(0);

    int i = 0;
    while (std::getline(file, line))
    {
        int numTokens;
        std::string *tokens = splitString(line, ',', numTokens);

        if (numTokens == 5)
        {
            int a = std::stoi(tokens[0]);
            int b = std::stoi(tokens[1]);
            int c = std::stoi(tokens[2]);
            int d = std::stoi(tokens[3]);
            int x = std::stoi(tokens[4]);

            rows[i++] = Row(a, b, c, d, x);
        }

        delete[] tokens;
    }

    file.close();
    return rows;
}

bool rowComparator(const Row &row1, const Row &row2)
{
    if (row1.A != row2.A)
        return row1.A < row2.A;
    if (row1.B != row2.B)
        return row1.B < row2.B;
    if (row1.C != row2.C)
        return row1.C < row2.C;
    return row1.D < row2.D;
}

Row *findMatchingRows(const Row *rows, int numRows, int &numMatches)
{
    Row *matchingRows = new Row[numRows];
    numMatches = 0;

    // Find matching rows
    for (int i = 0; i < numRows; i++)
    {
        bool printRow = true;
        int j = i + 1;

        while (j < numRows && rows[i].A == rows[j].A && rows[i].B == rows[j].B && rows[i].C == rows[j].C && rows[i].D == rows[j].D)
        {
            if (rows[i].X != rows[j].X)
            {
                printRow = false;
                break;
            }
            j++;
        }

        if (printRow)
        {
            matchingRows[numMatches++] = rows[i];
        }

        i = j - 1; // Skip the already checked rows
    }

    return matchingRows;
}

Row *findAndPrintMatchingRowsParallel(Row *rows, int numRows, int numThreads, int &totalMatches)
{
    Row **threadResults = new Row *[numThreads];
    int *threadNumMatches = new int[numThreads];

#pragma omp parallel num_threads(numThreads)
    {
        int tid = omp_get_thread_num();
        int threadStart = (tid * numRows) / numThreads;
        int threadEnd = ((tid + 1) * numRows) / numThreads;

        threadResults[tid] = findMatchingRows(rows + threadStart, threadEnd - threadStart, threadNumMatches[tid]);
    }

    totalMatches = 0;

    for (int i = 0; i < numThreads; i++)
    {
        totalMatches += threadNumMatches[i];
    }

    Row *matchingRows = new Row[totalMatches];
    int currentIndex = 0;

    for (int i = 0; i < numThreads; i++)
    {
        std::copy(threadResults[i], threadResults[i] + threadNumMatches[i], matchingRows + currentIndex);
        currentIndex += threadNumMatches[i];
        delete[] threadResults[i];
    }

    delete[] threadResults;
    delete[] threadNumMatches;

    return matchingRows;
}

int main()
{
    int numRows;
    Row *rows = parseCSV("data.csv", numRows);

    if (rows != nullptr)
    {
        int numThreads = 4;
        int totalMatches = 0;
        Row *matchingRows = findAndPrintMatchingRowsParallel(rows, numRows, numThreads, totalMatches);

        // Print the matching rows
        for (int i = 0; i < totalMatches; i++)
        {
            std::cout << matchingRows[i].A << "," << matchingRows[i].B << "," << matchingRows[i].C << ","
                      << matchingRows[i].D << "," << matchingRows[i].X << std::endl;
        }

        delete[] rows;
        delete[] matchingRows;
    }

    return 0;
}
