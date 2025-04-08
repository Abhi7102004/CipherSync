#include "../include/TestEncryption.h"

std::string TestEncryption::encrypt(const std::string &data) {
    std::string output;
    output.reserve(data.size());

    for (char ch : data) {
        output.push_back(ch + 1); // Shift by +1
    }

    return output;
}

std::string TestEncryption::decrypt(const std::string &data) {
    std::string output;
    output.reserve(data.size());

    for (char ch : data) {
        output.push_back(ch - 1); // Shift by -1
    }

    return output;
}
