#include <string>

class FileSortAlgorithm
{
private:
    void kWayMerge(const std::vector<std::string>& bufferFiles, const std::string& outputFile);
    std::string buildSortedChunkPath(int chunkIndex);
    std::string createFileAndSpitData(const std::vector<double>& buffer, int fileIndex);
    const std::string SORTED_CHUNKS_STORAGE_DIR = "/tmp";
    int mSortingBufferSize;

public:
    void sortContents(const std::string& inputFilePath, const std::string& outputFilePath);
    FileSortAlgorithm(int sortingBufferSize);
    FileSortAlgorithm() = delete;
    FileSortAlgorithm(const FileSortAlgorithm&) = delete;
    FileSortAlgorithm(FileSortAlgorithm&&) = delete;
    ~FileSortAlgorithm() noexcept = default;
};