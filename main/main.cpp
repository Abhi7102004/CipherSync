#include "../include/ThreadPool.h"
#include "../include/FileManager.h"
// #include "../include/TestEncryption.h"
#include "../include/EncryptionManager.h"
#include <iostream>
#include <vector>

const int CHUNK_SIZE = 4096;

int main()
{
    ThreadPool pool(8);
    const std::string key = "12345678912345678912345123456789";
    EncryptionManager manager(key);

    std::vector<std::vector<unsigned char>> chunks;
    
    {
        FileManager fileManager("test/input.txt", "test/output.enc");
        if (!fileManager.openFiles())
        {
            std::cerr << "Error opening files." << std::endl;
            return 1;
        }
        
        while (true)
        {
            std::vector<unsigned char> buffer;
            if (!fileManager.readChunk(buffer, CHUNK_SIZE))
                break;
                
            chunks.push_back(buffer);
        }
        
        fileManager.closeFiles();
    }
    
    std::vector<std::vector<unsigned char>> encryptedChunks(chunks.size());
    
    for (size_t i = 0; i < chunks.size(); i++)
    {
        size_t index = i;  
        pool.enqueueTask([&manager, &chunks, &encryptedChunks, index]() {
            try
            {
                // std::string input(chunks[index].begin(), chunks[index].end());
                auto encrypted = manager.encrypt(chunks[index]);
                encryptedChunks[index] = std::vector<unsigned char>(encrypted.begin(), encrypted.end());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Encryption error: " << e.what() << "\n";
            }
        });
    }
    pool.waitForAll();
    
    {
        FileManager fileManager("test/input.txt", "test/output.enc");
        if (!fileManager.openFiles())
        {
            std::cerr << "Error opening files." << std::endl;
            return 1;
        }
        
        for (const auto& chunk : encryptedChunks)
        {
            if (!fileManager.writeChunk(chunk))
            {
                std::cerr << "Error writing encrypted chunk." << std::endl;
            }
        }
        
        fileManager.closeFiles();
    }
    
    std::vector<std::vector<unsigned char>> encryptedFileChunks;
    
    {
        FileManager fileManager("test/output.enc", "test/original.txt");
        if (!fileManager.openFiles())
        {
            std::cerr << "Error opening files." << std::endl;
            return 1;
        }
        
        while (true)
        {
            std::vector<unsigned char> buffer;
            if (!fileManager.readChunk(buffer, CHUNK_SIZE))
                break;
                
            encryptedFileChunks.push_back(buffer);
        }
        
        fileManager.closeFiles();
    }
    
    std::vector<std::vector<unsigned char>> decryptedChunks(encryptedFileChunks.size());
    
    for (size_t i = 0; i < encryptedFileChunks.size(); i++)
    {
        size_t index = i; 
        pool.enqueueTask([&manager, &encryptedFileChunks, &decryptedChunks, index]() {
            try
            {
                // std::string input(encryptedFileChunks[index].begin(), encryptedFileChunks[index].end());
                auto decrypted = manager.decrypt(encryptedFileChunks[index]);

                decryptedChunks[index] = std::vector<unsigned char>(decrypted.begin(), decrypted.end());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Decryption error: " << e.what() << "\n";
            }
        });
    }
    
    pool.waitForAll();
    
    {
        FileManager fileManager("test/output.enc", "test/original.txt");
        if (!fileManager.openFiles())
        {
            std::cerr << "Error opening files." << std::endl;
            return 1;
        }
        
        for (const auto& chunk : decryptedChunks)
        {
            if (!fileManager.writeChunk(chunk))
            {
                std::cerr << "Error writing decrypted chunk." << std::endl;
            }
        }
        
        fileManager.closeFiles();
    }

    return 0;
}