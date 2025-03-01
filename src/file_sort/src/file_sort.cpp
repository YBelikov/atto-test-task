#include <iostream>
#include "FileSortAlgorithm.hpp"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Wrong number of options. Usage: file_sort <input file path> <output file path>\n";
        return -1;
    }
    const std::string inputFile = argv[1];
    const std::string outputFile = argv[2];
    FileSortAlgorithm algo;
    algo.sortContents(inputFile, outputFile);
    return 0;
}