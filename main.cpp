#include <iostream>
#include "compressor/floatcompressornew.h"

void compress()
{
    FloatCompressorNew compressor;
    compressor.SetupCompressor(NULL);
    compressor.CompressNone(100.0);
    compressor.FinishCompressor();
    compressor.SaveChars("index.txt", "content.dat");
}

int main(int argc, char **argv)
{
    std::cout << "Have Fun" << std::endl;
    compress();
    return 0;
}
