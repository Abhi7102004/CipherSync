CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -pthread
LIBS = -lssl -lcrypto

SRC = main/main.cpp src/EncryptionManager.cpp src/FileManager.cpp src/ThreadPool.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = encryptor

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f $(OBJ) $(TARGET)
