#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <string>
#include <vector>

class EncryptionManager
{
public:
    EncryptionManager(const std::string &key);
    ~EncryptionManager();
    
    std::vector<unsigned char> encrypt(const std::vector<unsigned char> &data);
    std::vector<unsigned char> decrypt(const std::vector<unsigned char> &data);
    
    std::vector<unsigned char> generateIV();

private:
    unsigned char key[32];
    void handleErrors();
};

#endif
