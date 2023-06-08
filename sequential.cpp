#include "utils.hpp"

Row *findMatchingRows(Row *rows, int numRows, int &numAnomalies, int& numCorrect)
{
    Row *anomalies = new Row[numRows];
    numAnomalies = 0;
    numCorrect = 0;

    for (int i = 0; i < numRows; i++)
    {
        bool isConsistent = true;
        for (int j = 0; j < numRows; j++)
        {
            if (rows[i].A == rows[j].A && rows[i].B == rows[j].B && rows[i].C == rows[j].C && rows[i].D == rows[j].D)
            {
                if (rows[i].X != rows[j].X)
                {
                    isConsistent = false;
                    break;
                }
            }
        }
        if (!isConsistent)
        {
            anomalies[numAnomalies++] = rows[i];
        } else {
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
        Row *foundRows = findMatchingRows(rows, numRows, numAnomalies, numCorrect);
        std::cout << "Liczba wierszy w pliku: " << numRows << std::endl;
        std::cout << "Zgodnych wynikow: " << numCorrect << std::endl;
        if (numAnomalies == 0)
        {
            std::cout << "X wynika bezpośrednio z ABCD" << std::endl;
        }
        else
        {
            std::cout << "Wystepuja anomalie: " << numAnomalies << std::endl;
            printRows(foundRows, numAnomalies);
        }

        delete[] rows;
    }

    return 0;
}
