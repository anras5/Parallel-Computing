#include "utils.hpp"

Row *findAnomalies(Row *rows, int numRows, int &numAnomalies, int &numCorrect)
{
    Row *anomalies = new Row[numRows];
    numAnomalies = 0;
    numCorrect = 0;

    for (int i = 0; i < numRows; i++)
    {
        bool isConsistent = true;
        for (int j = 0; j < numRows; j++)
        {
            if (i != j)
            {
                bool isMatch = true;
                for (int k = 0; k < rows[i].numValues - 1; k++)
                {
                    if (rows[i].values[k] != rows[j].values[k])
                    {
                        isMatch = false;
                        break;
                    }
                }
                if (isMatch && rows[i].values[rows[i].numValues - 1] != rows[j].values[rows[j].numValues - 1])
                {
                    isConsistent = false;
                    break;
                }
            }
        }
        if (!isConsistent)
        {
            anomalies[numAnomalies++] = rows[i];
        }
        else
        {
            numCorrect++;
        }
    }

    return anomalies;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./sequential_all <csv_filename>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    int numRows;
    int numAnomalies = 0;
    int numCorrect = 0;
    Row *rows = parseCSV(filename, numRows);

    if (rows != nullptr)
    {
        Row *foundRows = findAnomalies(rows, numRows, numAnomalies, numCorrect);
        std::cout << "Number of rows in the file: " << numRows << std::endl;
        std::cout << "Correct rows: " << numCorrect << std::endl;
        if (numAnomalies == 0)
        {
            std::cout << "Last column is dependent on the rest" << std::endl;
            
        }
        else
        {
            std::cout << "Found anomalies: " << numAnomalies << std::endl;
            // printRows(foundRows, numAnomalies);
        }

        delete[] rows;
    }

    return 0;
}
