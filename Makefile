CXX = g++
CXXFLAGS = -std=c++14 -Werror -Wuninitialized -I include

TARGET = SFMR
SOURCES = src/main.cpp src/kd_tree.cpp src/vp_tree.cpp src/CSVLoader.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
