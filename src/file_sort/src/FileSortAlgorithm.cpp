#include <fstream>
#include <iostream>
#include <queue>
#include <stdio.h>
#include "FileSortAlgorithm.hpp"

void FileSortAlgorithm::sortContents(const std::string& inputFilePath, const std::string& outputFilePath) 
{
    std::vector<double> buffer;
    std::vector<std::string> bufferFiles;
    int bufferCapacity = SORTED_BUFFER_SIZE / sizeof(double);
    buffer.reserve(bufferCapacity);
    std::ifstream in(inputFilePath);
    if (!in) 
    {
        std::cerr << "Error opening file: " << inputFilePath << std::endl;
        return;
    }
    std::string value;
    int fileIndex = 1;
    for(; getline(in, value);)
    {
        buffer.push_back(std::stod(value));
        if (buffer.size() >= bufferCapacity)
        {
            processAndStoreChunk(buffer, bufferFiles, fileIndex);
            buffer.clear();
            ++fileIndex;
        }
    }
    in.close();
    if (!buffer.empty())
    {
        processAndStoreChunk(buffer, bufferFiles, fileIndex);
        buffer.clear();
    }
    kWayMerge(bufferFiles, outputFilePath);
}

void FileSortAlgorithm::processAndStoreChunk(std::vector<double>& buffer, std::vector<std::string>& bufferFiles, int& fileIndex)
{
    std::sort(buffer.begin(), buffer.end());
    bufferFiles.push_back(createFileAndSpitData(buffer, fileIndex));
}

std::string FileSortAlgorithm::createFileAndSpitData(const std::vector<double>& buffer, int fileIndex)
{
    auto bufferOutputFilePath = buildSortedChunkPath(fileIndex);
    std::ofstream out(bufferOutputFilePath);
    out << std::scientific << std::setprecision(5);
    for (const double& value : buffer) 
    {
        out << value << '\n';
    }
    return bufferOutputFilePath;
}

std::string FileSortAlgorithm::buildSortedChunkPath(int chunkIndex) 
{
    return SORTED_CHUNKS_STORAGE_DIR + "/" + "sort_tmp_" + std::to_string(chunkIndex) + ".txt";
}

void FileSortAlgorithm::kWayMerge(const std::vector<std::string>& bufferFiles, const std::string& outputFile)
{
    struct HeapElement 
    {
        double data;
        int streamIndex;
        HeapElement(const HeapElement& other) = default;
        HeapElement(double data, int streamIndex)
        {
            this->data = data;
            this->streamIndex = streamIndex;
        };

        bool operator > (const HeapElement& other) const
        {
            // Since I work with fixed precision of 5 digits after a dot
            // I don't think there should be problems with comparison
            return data > other.data;
        }
    };
    std::vector<std::ifstream> sortedChunkStreams;
    for (const std::string& streamFile : bufferFiles) 
    {
        sortedChunkStreams.emplace_back(streamFile);
    }
    std::priority_queue<HeapElement, std::vector<HeapElement>, std::greater<HeapElement>> minHeap;
    for (int i = 0; i < sortedChunkStreams.size(); ++i)
    {
        std::string line;
        if (getline(sortedChunkStreams[i], line))
        {
            double value = std::stod(line);
            minHeap.push(HeapElement(value, i));
        }   
    }
    std::ofstream finalOut(outputFile);
    finalOut << std::scientific << std::setprecision(5);
    while(!minHeap.empty())
    {
        HeapElement minElement = minHeap.top();
        minHeap.pop();
        finalOut << minElement.data << '\n';
        double value;
        if (sortedChunkStreams[minElement.streamIndex] >> value) 
        {
            minHeap.push(HeapElement(value, minElement.streamIndex));
        }
    }
    for (int idx = 0; idx < sortedChunkStreams.size(); ++idx)
    {
        sortedChunkStreams[idx].close();
        remove(bufferFiles[idx].c_str());
    }
}