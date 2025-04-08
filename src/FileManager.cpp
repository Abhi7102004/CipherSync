#include "../include/FileManager.h"
#include <iostream>

FileManager::FileManager(const std::string &inputFilePath, const std::string &outputFilePath)
    : inputFilePath(inputFilePath), outputFilePath(outputFilePath) {}

FileManager::~FileManager()
{
    closeFiles();
}

bool FileManager::openFiles()
{
    inputFile.open(inputFilePath, std::ios::in | std::ios::binary);
    outputFile.open(outputFilePath, std::ios::out | std::ios::binary);

    if (!inputFile.is_open())
    {

        std::cerr << "Error: Failed to open input file: " << inputFilePath << "\n";
        return false;
    }
    if (!outputFile.is_open())
    {
        std::cerr << "Error: Failed to open output file: " << outputFilePath << "\n";
        return false;
    }
    return true;
}

void FileManager::closeFiles()
{
    if (inputFile.is_open())
        inputFile.close();
    if (outputFile.is_open())
        outputFile.close();
}

bool FileManager::readChunk(std::vector<unsigned char> &buffer, size_t chunkSize)
{
    buffer.resize(chunkSize);
    inputFile.read(reinterpret_cast<char *>(buffer.data()), chunkSize);
    size_t bytesRead = inputFile.gcount();

    if (bytesRead == 0)
    {
        return false;
    }

    buffer.resize(bytesRead);

    return true;
}

bool FileManager::writeChunk(const std::vector<unsigned char> &buffer)
{
    // std::lock_guard<std::mutex> lock(writeMutex);

    if (!outputFile.write(reinterpret_cast<const char *>(buffer.data()), buffer.size()))
    {
        std::cerr << "Error: Failed to write to output file\n";
        return false;
    }

    return true;
}
