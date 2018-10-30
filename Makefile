CXX=g++
CXXFLAGS=-std=c++11 -O2
SOURCES=main.cpp \
	compressor/rangeencoder.cpp compressor/floatcompressornew.cpp \
	compressor/rangedecoder.cpp compressor/rangemodel.cpp \
	bvh_reader.cpp
EXEC=dfcm
$(EXEC): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXEC)
