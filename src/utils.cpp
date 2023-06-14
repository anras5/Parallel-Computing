#include "utils.hpp"

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

        int *values = new int[numTokens];
        for (int j = 0; j < numTokens; j++)
        {
            values[j] = std::stoi(tokens[j]);
        }

        Row *newRow = new Row(values, numTokens);
        rows[i++] = *newRow;

        delete[] values;
        delete[] tokens;
    }

    file.close();

    return rows;
}

void printRows(Row *rows, int numRows)
{
    for (int i = 0; i < numRows; i++)
    {
        Row &row = rows[i];
        for (int j = 0; j < row.numValues; j++)
        {
            std::cout << row.values[j];
            if (j < row.numValues - 1)
            {
                std::cout << ",";
            }
        }
        std::cout << std::endl;
    }
}