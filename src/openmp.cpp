#include <omp.h>
#include <cstring>
#include <chrono>
#include <iomanip>
#include "utils.hpp"

void findAnomalies(Row *rows, int numRows, int &numAnomalies, int &numCorrect)
{
    int numThreads = omp_get_max_threads();
    int *numAnomaliesArray = new int[numThreads];
    int *numCorrectArray = new int[numThreads];

#pragma omp parallel num_threads(numThreads)
    {
        int threadNum = omp_get_thread_num();

        int numRowsPerThread = (numRows + numThreads - 1) / numThreads;
        int startRow = threadNum * numRowsPerThread;
        int endRow = std::min(startRow + numRowsPerThread, numRows);

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
                numAnomalies++;
            }
            else
            {
                numCorrect++;
            }
        }
        numAnomaliesArray[threadNum] = numAnomalies;
        numCorrectArray[threadNum] = numCorrect;
    }

    // combine results from all threads
    numAnomalies = 0;
    numCorrect = 0;
    for (int i = 0; i < numThreads; i++)
    {
        numAnomalies += numAnomaliesArray[i];
        numCorrect += numCorrectArray[i];
    }

    // cleanup
    delete[] numAnomaliesArray;
    delete[] numCorrectArray;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./openmp <csv_filename>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    int numRows;
    int numAnomalies = 0;
    int numCorrect = 0;
    Row *rows = parseCSV(filename, numRows);

    if (rows != nullptr)
    {
        auto start = std::chrono::high_resolution_clock::now();
        findAnomalies(rows, numRows, numAnomalies, numCorrect);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        double executionTime = duration.count() * 1000;

        std::cout << numRows << ";" << std::fixed << std::setprecision(2) << executionTime << std::endl;

        // std::cout << "Number of rows in the file: " << numRows << std::endl;
        // std::cout << "Correct rows: " << numCorrect << std::endl;
        // if (numAnomalies == 0)
        // {
        //     std::cout << "Last column is dependent on the rest" << std::endl;
        // }
        // else
        // {
        //     std::cout << "Found anomalies: " << numAnomalies << std::endl;
        // }

        delete[] rows;
    }

    return 0;
}