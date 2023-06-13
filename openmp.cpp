#include <omp.h>
#include <cstring>
#include "utils.hpp"

Row *findAnomalies(Row *rows, int numRows, int &numAnomalies, int &numCorrect)
{
    int numThreads = omp_get_max_threads();
    Row **anomaliesArray = new Row *[numThreads];
    int *numAnomaliesArray = new int[numThreads];
    int *numCorrectArray = new int[numThreads];

#pragma omp parallel num_threads(numThreads)
    {
        int threadNum = omp_get_thread_num();

        int numRowsPerThread = (numRows + numThreads - 1) / numThreads;
        int startRow = threadNum * numRowsPerThread;
        int endRow = std::min(startRow + numRowsPerThread, numRows);

        Row *anomalies = new Row[endRow - startRow];
        int numAnomalies = 0;
        int numCorrect = 0;

        for (int i = startRow; i < endRow; i++)
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

        anomaliesArray[threadNum] = anomalies;
        numAnomaliesArray[threadNum] = numAnomalies;
        numCorrectArray[threadNum] = numCorrect;

    }

    // Combine results from all threads
    numAnomalies = 0;
    numCorrect = 0;
    for (int i = 0; i < numThreads; i++)
    {
        numAnomalies += numAnomaliesArray[i];
        numCorrect += numCorrectArray[i];
    }

    // Create a single anomalies array and copy data from all threads
    Row *combinedAnomalies = new Row[numAnomalies];
    int currentIndex = 0;
    for (int i = 0; i < numThreads; i++)
    {
        int numAnomaliesThread = numAnomaliesArray[i];
        memcpy(combinedAnomalies + currentIndex, anomaliesArray[i], numAnomaliesThread * sizeof(Row));
        currentIndex += numAnomaliesThread;
    }

    // Cleanup
    delete[] anomaliesArray;
    delete[] numAnomaliesArray;
    delete[] numCorrectArray;

    return combinedAnomalies;
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