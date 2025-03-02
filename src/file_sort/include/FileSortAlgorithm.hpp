#ifndef FILESORTALGORITHM
#define FILESORTALGORITHM

#include <string>

class FileSortAlgorithm
{
public:
    void sortContents(const std::string& inputFilePath, const std::string& outputFilePath);
    FileSortAlgorithm() = default;
    FileSortAlgorithm(const FileSortAlgorithm&) = delete;
    FileSortAlgorithm(FileSortAlgorithm&&) = delete;
    ~FileSortAlgorithm() noexcept = default;

private:
    void kWayMerge(const std::vector<std::string>& bufferFiles, const std::string& outputFile);
    std::string buildSortedChunkPath(int chunkIndex);
    void processAndStoreChunk(std::vector<double>& buffer, std::vector<std::string>& bufferFiles, int& fileIndex);
    std::string createFileAndSpitData(const std::vector<double>& buffer, int fileIndex);

    const std::string SORTED_CHUNKS_STORAGE_DIR = "/tmp"; // Works for Unix-based system (macOS, Linux) I tested it on
    
    const int SORTED_BUFFER_SIZE = 80 * 1e6; // bytes in 80 MB
};

#endif