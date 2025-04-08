#include "../include/EncryptionManager.h"
#include "../include/FileManager.h"
#include <iostream>
#include <vector>

const int CHUNK_SIZE = 4096;

int main() {
    std::string inputFilePath = "test/input.txt";
    std::string encryptedFilePath = "test/output.enc";
    std::string decryptedFilePath = "test/decrypted.txt";

    std::string key = "12345678901234561234567890123456"; 
    EncryptionManager encryptionManager(key);

    {
        FileManager fileManager(inputFilePath, encryptedFilePath);

        if (!fileManager.openFiles()) {
            std::cerr << "Failed to open input/output files." << std::endl;
            return 1;
        }

        std::vector<unsigned char> buffer;
        while (fileManager.readChunk(buffer, CHUNK_SIZE)) {
            std::vector<unsigned char> encryptedData = encryptionManager.encrypt(buffer);
            if (!fileManager.writeChunk(encryptedData)) {
                std::cerr << "Error writing encrypted data to file." << std::endl;
                return 1;
            }
        }

        std::cout << "File encrypted successfully." << std::endl;
    }
    
    {
        FileManager fileManager(encryptedFilePath, decryptedFilePath);

        if (!fileManager.openFiles()) {
            std::cerr << "Failed to open encrypted/decrypted files." << std::endl;
            return 1;
        }

        std::vector<unsigned char> buffer;
        while (fileManager.readChunk(buffer, CHUNK_SIZE + 16)) {
            try {
                std::vector<unsigned char> decryptedData = encryptionManager.decrypt(buffer);
                if (!fileManager.writeChunk(decryptedData)) {
                    std::cerr << "Error writing decrypted data to file." << std::endl;
                    return 1;
                }
            } catch (const std::exception &ex) {
                std::cerr << "Decryption error: " << ex.what() << std::endl;
                return 1;
            }
        }

        std::cout << "File decrypted successfully." << std::endl;
    }
    return 0;
}
