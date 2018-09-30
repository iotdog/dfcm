#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <limits>
#include "compressor/floatcompressornew.h"

/**
 * 加载数据
 */
void load(const std::string filename, std::vector<double> *data)
{
    std::fstream file;
    file.open(filename);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, ' '))
            {
                if (item.length() == 0)
                { // 过滤空字符串
                    continue;
                }
                tokens.push_back(item);
            }
            for (int i = 0; i < tokens.size(); i++)
            {
                data->push_back(atof(tokens[i].c_str()));
            }
        }
    }
    file.close();
}

void save(const std::string filename, std::vector<double> *data)
{
    std::ofstream file;
    file.open(filename);
    if (file.is_open())
    {
        for (int i = 0; i < data->size(); i++)
        {
            file << data->at(i) << " ";
        }
    }
    file.close();
}

int hash_func(double delta1, double delta2, double delta3)
{
    long long d1, d2, d3;
    if(sizeof(long long) != sizeof(double) || sizeof(double) != 8)
    {
        std::cerr << "wrong data size" << std::endl;
        exit(1);
    }
    memcpy(&d1, &delta1, 8);
    memcpy(&d2, &delta2, 8);
    memcpy(&d3, &delta3, 8);
    // 取前14个bit
    d1 = d1 >> 50;
    d2 = d2 >> 50;
    d3 = d3 >> 50;
    // 异或之后取最后的20个bit
    return 0xfffff & (d1 ^ (d2 << 5) ^ (d3 << 10));
}

double compute_dpred(double dpred1, double dpred2)
{
    if (dpred2 == std::numeric_limits<double>::max())
    {
        return dpred1;
    }
    // 比较前14字节是否相同
    long long dp1 = (long long)dpred1;
    long long dp2 = (long long)dpred2;
    if ((dp1 >> 50) == (dp2 >> 50))
    {
        return dpred1;
    }
    else
    {
        return dpred1 * 2 - dpred2;
    }
}

double predict(double prev, double delta1, double delta2, double delta3, double dpred, std::unordered_map<int, double *> *hashTable)
{
    int index = hash_func(delta1, delta2, delta3);
    auto iter = hashTable->find(index);
    double pred = 0;
    // dpreds的值是如何得来的？？？
    if (iter == hashTable->end()) // 索引（Hash值）不存在，新建入口
    {
        double *dpreds = new (double[3]);
        dpreds[0] = 1; // 标识下一次是更新dpred'还是dpred"
        dpreds[1] = dpred;
        dpreds[2] = std::numeric_limits<double>::max(); // 未填充的字段暂时使用MAX标识
        pred = prev + compute_dpred(dpreds[1], dpreds[2]);
        hashTable->insert({index, dpreds});
    }
    else // 索引（Hash值）已存在，更新预测值
    {
        double *dpreds = iter->second;
        int tmpIdx;
        if (dpreds[0] == 0)
        {
            tmpIdx = 1;
            dpreds[0] = 1;
        }
        else
        {
            tmpIdx = 2;
            dpreds[0] = 0;
        }
        dpreds[tmpIdx] = dpred;
        pred = prev + compute_dpred(dpreds[1], dpreds[2]);
    }
    return pred;
}

/**
 * 使用DFCM预测器压缩
 */
void compress_dfcm(std::vector<double> *data, std::unordered_map<int, double *> *hashTable)
{
    FloatCompressorNew compressor;
    compressor.SetupCompressor(NULL);
    compressor.CompressNone(data->size());
    for (int i = 0; i < data->size(); i++)
    {
        if (i - 4 < 0) // 前四个数采用简单压缩
        {
            compressor.CompressNone(data->at(i));
        }
        else
        {
            double pred = predict(data->at(i - 1),
                                  data->at(i - 1) - data->at(i - 2),
                                  data->at(i - 2) - data->at(i - 3),
                                  data->at(i - 3) - data->at(i - 4), data->at(i - 1) - data->at(i - 2), hashTable);
            std::cout << "real: " << data->at(i) << ", pred: " << pred << std::endl;
            compressor.CompressAcross(pred, data->at(i));
        }
    }
    compressor.FinishCompressor();
    compressor.SaveChars("index_dfcm.txt", "content_dfcm.dat");
}

void decompress_dfcm(const std::string idx_file, const std::string content_file, std::vector<double> *data)
{
    std::unordered_map<int, double *> hashTable;
    FloatCompressorNew decompressor;
    decompressor.NewSetupDC(idx_file, content_file);
    int data_size = decompressor.DecompressNone();
    for (int i = 0; i < data_size; i++)
    {
        if (i - 4 < 0) 
        {
            data->push_back(decompressor.DecompressNone());
        }
        else
        {
            double pred = predict(data->at(i - 1),
                                  data->at(i - 1) - data->at(i - 2),
                                  data->at(i - 2) - data->at(i - 3),
                                  data->at(i - 3) - data->at(i - 4), data->at(i - 1) - data->at(i - 2), &hashTable);
            data->push_back(decompressor.DecompressAcross(pred));
        }
    }
    decompressor.FinishDecompressor();
}

/**
 * 简单压缩
 */
void compress_simple(std::vector<double> *data)
{
    FloatCompressorNew compressor;
    compressor.SetupCompressor(NULL);
    compressor.CompressNone(data->size());
    for (int i = 0; i < data->size(); i++)
    {
        compressor.CompressNone(data->at(i));
    }
    compressor.FinishCompressor();
    compressor.SaveChars("index_simple.txt", "content_simple.dat");
}

/**
 * 简单解压
 */
void decompress_simple(const std::string idx_file, const std::string content_file, std::vector<double> *data)
{
    FloatCompressorNew decompressor;
    decompressor.NewSetupDC(idx_file, content_file);
    int data_size = decompressor.DecompressNone();
    for (int i = 0; i < data_size; i++)
    {
        data->push_back(decompressor.DecompressNone());
    }
    decompressor.FinishDecompressor();
}

int main(int argc, char **argv)
{
    // 初始化Hash表
    std::unordered_map<int, double *> hashTable;

    // 加载数据
    std::vector<double> data = {};
    load("0_float.txt", &data);
    std::cout << "data size: " << data.size() << std::endl;

    // 压缩
    compress_dfcm(&data, &hashTable);
    std::cout << "hash table size: " << hashTable.size() << std::endl;
    // compress_simple(&data);

    // 输出Hash表并释放内存
    for (auto iter = hashTable.begin(); iter != hashTable.end(); ++iter)
    {
        // std::cout << iter->first << ": [" << iter->second[1] << ", " << iter->second[2] << "]" << std::endl;
        free(iter->second);
    }
    hashTable.clear();

    // 解压缩
    // data.clear();
    // decompress_dfcm("index_hfcm.txt", "content_hfcm.dat", &data);
    // std::cout << "decompressed data size: " << data.size() << std::endl;
    // save("1.txt", &data);
    // data.clear();
    // decompress_simple("index_simple.txt", "content_simple.dat", &data);
    // std::cout << "decompressed data size: " << data.size() << std::endl;
    // save("0_float.txt", &data);

    return 0;
}
