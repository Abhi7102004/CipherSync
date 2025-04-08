#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include <string>
#include <vector>
#include <mutex>
class FileManager
{

public:
    FileManager(const std::string &inputFilePath, const std::string &outputFilePath);
    ~FileManager();
    bool openFiles();
    bool readChunk(std::vector<unsigned char> &buffer, size_t chunkSize);
    bool writeChunk(const std::vector<unsigned char> &buffer);
    void closeFiles();
   
private:
    std::ifstream inputFile;
    std::ofstream outputFile;
    std::string inputFilePath;
    std::string outputFilePath;
    std::mutex writeMutex;
};

#endif