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

void printRows(Row *rows, int numRows)
{
    for (int i = 0; i < numRows; i++)
    {
        Row &row = rows[i];
        std::cout << row.A << "," << row.B << "," << row.C << "," << row.D << "," << row.X << std::endl;
    }
}