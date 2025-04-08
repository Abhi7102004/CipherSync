#ifndef TESTENCRYPTION_H
#define TESTENCRYPTION_H

#include <string>

class TestEncryption
{
public:
    TestEncryption() = default;
    std::string encrypt(const std::string &data);
    std::string decrypt(const std::string &data);
};

#endif
