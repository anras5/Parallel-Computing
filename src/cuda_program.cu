#include "cuda.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <chrono>
#include <iomanip>

struct Row
{
    int *values;
    int numValues;

    __host__ __device__ Row() : values(nullptr), numValues(0) {}

    __host__ __device__ Row(int *vals, int count) : numValues(count)
    {
        values = new int[numValues];
        for (int i = 0; i < numValues; i++)
        {
            values[i] = vals[i];
        }
    }

    __host__ __device__ ~Row()
    {
        delete[] values;
    }
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

__global__ void findAnomaliesKernel(Row *rows, int numRows, int *numAnomaliesArray, int *numCorrectArray)
{
    int threadNum = blockIdx.x * blockDim.x + threadIdx.x;
    int numThreads = gridDim.x * blockDim.x;

    int numRowsPerThread = (numRows + numThreads - 1) / numThreads;
    int startRow = threadNum * numRowsPerThread;
    int endRow = min(startRow + numRowsPerThread, numRows);

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
        if (isConsistent)
        {
            numCorrect++;
        } else {
            numAnomalies++;
        }
    }

    numAnomaliesArray[threadNum] = numAnomalies;
    numCorrectArray[threadNum] = numCorrect;
}

void findAnomalies(Row *rows, int numRows, int &numAnomalies, int &numCorrect)
{
    cudaError_t error;
    int numThreadsPerBlock = 256; // Choose an appropriate number of threads per block
    int numBlocks = (numRows + numThreadsPerBlock - 1) / numThreadsPerBlock;

    int *numAnomaliesArray;
    int *numCorrectArray;

    Row *devRows;
    cudaMalloc((void **)&devRows, numRows * sizeof(Row));
    // Allocate memory for Row.values on the device
    for (int i = 0; i < numRows; i++)
    {
        int *devValues;
        cudaMalloc((void **)&devValues, rows[i].numValues * sizeof(int));
        error = cudaMemcpy(devValues, rows[i].values, rows[i].numValues * sizeof(int), cudaMemcpyHostToDevice);
        if (error != cudaSuccess)
        {
            printf("cudaMemcpy (devValues,rows[i].values) returned error code %d, line(%d)\n", error, __LINE__);
            exit(EXIT_FAILURE);
        }
        rows[i].values = devValues;
    }

    error = cudaMemcpy(devRows, rows, numRows * sizeof(Row), cudaMemcpyHostToDevice);
    if (error != cudaSuccess)
    {
        printf("cudaMemcpy (devRows,rows) returned error code %d, line(%d)\n", error, __LINE__);
        exit(EXIT_FAILURE);
    }
    cudaMalloc((void **)&numAnomaliesArray, numBlocks * numThreadsPerBlock * sizeof(int));
    cudaMalloc((void **)&numCorrectArray, numBlocks * numThreadsPerBlock * sizeof(int));

    findAnomaliesKernel<<<numBlocks, numThreadsPerBlock>>>(devRows, numRows, numAnomaliesArray, numCorrectArray);

    // Combine results from all threads
    numAnomalies = 0;
    numCorrect = 0;
    for (int i = 0; i < numBlocks * numThreadsPerBlock; i++)
    {
        int numAnomaliesThread = 0;
        error = cudaMemcpy(&numAnomaliesThread, numAnomaliesArray + i, sizeof(int), cudaMemcpyDeviceToHost);
        if (error != cudaSuccess)
        {
            printf("cudaMemcpy (numAnomaliesThread,numAnomaliesArray) returned error code %d, line(%d)\n", error, __LINE__);
            exit(EXIT_FAILURE);
        }
        numAnomalies += numAnomaliesThread;

        int numCorrectThread = 0;
        error = cudaMemcpy(&numCorrectThread, numCorrectArray + i, sizeof(int), cudaMemcpyDeviceToHost);
        if (error != cudaSuccess)
        {
            printf("cudaMemcpy (numCorrectThread,numCorrectArray) returned error code %d, line(%d)\n", error, __LINE__);
            exit(EXIT_FAILURE);
        }
        numCorrect += numCorrectThread;
    }

    // Cleanup
    cudaFree(devRows);
    cudaFree(numAnomaliesArray);
    cudaFree(numCorrectArray);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./cuda_program <csv_filename>" << std::endl;
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