#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <limits>
#include <ctime>
#include "compressor/floatcompressornew.h"
#include "bvh_reader.h"

/**
 * 加载数据
 */
void load(const std::string filename, std::vector<float> *data)
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

void save(const std::string filename, std::vector<float> *data)
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

int hash_func(float delta1, float delta2, float delta3)
{
    int d1, d2, d3;
    memcpy(&d1, &delta1, 4);
    memcpy(&d2, &delta2, 4);
    memcpy(&d3, &delta3, 4);
    // 浮点数数据结构：https://www.cnblogs.com/sunfan1988/p/3932714.html
    // 取前11个bit：1符号位，8阶数位，2小数位
    d1 = d1 >> 21;
    d2 = d2 >> 21;
    d3 = d3 >> 21;
    // 异或之后取最后的20个bit
    return 0xfffff & (d1 ^ (d2 << 5) ^ (d3 << 10));
}

float compute_dpred(float dpred1, float dpred2)
{
    if (dpred2 == std::numeric_limits<float>::max())
    {
        return dpred1;
    }
    // 比较前11字节是否相同
    int dp1, dp2;
    memcpy(&dp1, &dpred1, 4);
    memcpy(&dp2, &dpred2, 4);
    if ((dp1 >> 21) == (dp2 >> 21))
    {
        return dpred1;
    }
    else
    {
        return dpred1 * 2 - dpred2;
    }
}

float predict(float prev, float delta1, float delta2, float delta3, float dpred_new, std::unordered_map<int, float *> *hashTable)
{
    int index = hash_func(delta1, delta2, delta3);
    auto iter = hashTable->find(index);
    float pred = 0;
    if (iter == hashTable->end()) // 索引（Hash值）不存在，新建入口
    {
        float *dpreds = new (float[3]);
        dpreds[0] = 2; // 标识下一次是更新dpred'还是dpred"
        dpreds[1] = dpred_new;
        dpreds[2] = std::numeric_limits<float>::max(); // 未填充的字段暂时使用MAX标识
        hashTable->insert({index, dpreds});
        // std::cout << "new hash table entry: " << index << ": [" << dpreds[0] << ", " << dpreds[1] << ", " << dpreds[2] << "]" << std::endl;
        // 此时没有预测值
        return std::numeric_limits<float>::max();
    }
    else // 索引（Hash值）已存在，更新预测值
    {
        float *dpreds = iter->second;
        pred = prev + compute_dpred(dpreds[1], dpreds[2]);
        // 更新预测值
        if (dpreds[0] == 2)
        {
            dpreds[2] = dpred_new;
            dpreds[0] = 1;
        }
        else
        {
            dpreds[1] = dpred_new;
            dpreds[0] = 2;
        }
        // std::cout << "update hash table entry: " << index << ": [" << dpreds[0] << ", " << dpreds[1] << ", " << dpreds[2] << "]" << std::endl;
    }
    return pred;
}

/**
 * 使用DFCM预测器压缩
 */
void compress_dfcm(std::vector<float> *data, std::unordered_map<int, float *> *hashTable)
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
            float pred = predict(data->at(i - 1),
                                 data->at(i - 1) - data->at(i - 2),
                                 data->at(i - 2) - data->at(i - 3),
                                 data->at(i - 3) - data->at(i - 4), data->at(i) - data->at(i - 1), hashTable);
            if (std::numeric_limits<float>::max() == pred) // 没有预测值，简单压缩
            {
                compressor.CompressNone(data->at(i));
            }
            else
            {
                // std::cout << "real: " << data->at(i) << ", pred: " << pred << std::endl;
                compressor.CompressAcross(pred, data->at(i));
            }
        }
    }
    compressor.FinishCompressor();
    compressor.SaveChars("index_dfcm.txt", "content_dfcm.dat");
}

void decompress_dfcm(const std::string idx_file, const std::string content_file, std::vector<float> *data)
{
    std::unordered_map<int, float *> hashTable;
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
            float delta1, delta2, delta3;
            delta1 = data->at(i - 1) - data->at(i - 2);
            delta2 = data->at(i - 2) - data->at(i - 3);
            delta3 = data->at(i - 3) - data->at(i - 4);
            int index = hash_func(delta1, delta2, delta3);
            auto iter = hashTable.find(index);
            if (iter == hashTable.end()) // 索引（Hash值）不存在，新建入口
            {
                // 此时没有预测值
                data->push_back(decompressor.DecompressNone());
                float *dpreds = new (float[3]);
                dpreds[0] = 2; // 标识下一次是更新dpred'还是dpred"
                dpreds[1] = data->at(i) - data->at(i - 1);
                dpreds[2] = std::numeric_limits<float>::max(); // 未填充的字段暂时使用MAX标识
                hashTable.insert({index, dpreds});
            }
            else
            {
                float *dpreds = iter->second;
                float pred = data->at(i - 1) + compute_dpred(dpreds[1], dpreds[2]);
                data->push_back(decompressor.DecompressAcross(pred));
                float dpredNew = data->at(i) - data->at(i - 1);
                // 更新预测值
                if (dpreds[0] == 2)
                {
                    dpreds[2] = dpredNew;
                    dpreds[0] = 1;
                }
                else
                {
                    dpreds[1] = dpredNew;
                    dpreds[0] = 2;
                }
            }
        }
    }
    decompressor.FinishDecompressor();
}

/**
 * 简单压缩
 */
void compress_simple(std::vector<float> *data)
{
    clock_t start = clock();
    FloatCompressorNew compressor;
    compressor.SetupCompressor(NULL);
    compressor.CompressNone(data->size());
    for (int i = 0; i < data->size(); i++)
    {
        compressor.CompressNone(data->at(i));
    }
    compressor.FinishCompressor();
    compressor.SaveChars("index_simple.txt", "content_simple.dat");
    float interval = float(clock() - start) / CLOCKS_PER_SEC;
    // std::cout << "compress time: " << interval << std::endl;
}

/**
 * 简单解压
 */
void decompress_simple(const std::string idx_file, const std::string content_file, std::vector<float> *data)
{
    clock_t start = clock();
    FloatCompressorNew decompressor;
    decompressor.NewSetupDC(idx_file, content_file);
    int data_size = decompressor.DecompressNone();
    for (int i = 0; i < data_size; i++)
    {
        data->push_back(decompressor.DecompressNone());
    }
    decompressor.FinishDecompressor();
    float interval = float(clock() - start) / CLOCKS_PER_SEC;
    // std::cout << "decompress time: " << interval << std::endl;
}

void bvh_compress()
{
    // 初始化Hash表
    std::unordered_map<int, float *> hashTable;
    // 读取BVH文件
    BVHReader reader;
    if (reader.LoadDir("./086"))
    {
        FloatCompressorNew compressor;
        compressor.SetupCompressor(NULL);
        compressor.CompressNone(reader.Frames() * reader.Dimensions());
        float *bvhData;
        reader.FlatData(&bvhData);

        for (int d = 0; d < reader.Dimensions(); d++) // 针对每一列进行预测压缩
        {
            std::vector<float> data = {};
            for (int f = 0; f < reader.Frames(); f++)
            {
                data.push_back(bvhData[f * reader.Dimensions() + d]);
            }

            for (int i = 0; i < data.size(); i++)
            {
                if (i - 4 < 0) // 前四个数采用简单压缩
                {
                    compressor.CompressNone(data[i]);
                }
                else
                {
                    float pred = predict(data[i - 1],
                                         data[i - 1] - data[i - 2],
                                         data[i - 2] - data[i - 3],
                                         data[i - 3] - data[i - 4], data[i] - data[i - 1], &hashTable);
                    if (std::numeric_limits<float>::max() == pred) // 没有预测值，简单压缩
                    {
                        compressor.CompressNone(data[i]);
                    }
                    else
                    {
                        // std::cout << "real: " << data[i] << ", pred: " << pred << std::endl;
                        compressor.CompressAcross(pred, data[i]);
                    }
                }
            }
        }
        compressor.FinishCompressor();
        compressor.SaveChars("index_bvh.txt", "content_bvh.dat");

        std::cout << "data size: " << reader.Frames() * reader.Dimensions() << std::endl;
        std::cout << "hash table size: " << hashTable.size() << std::endl;

        delete[] bvhData;
    }
}

float preprocess(std::vector<float> *data)
{
    // 找到最小值
    float min = 0;
    for (int i = 0; i < data->size(); i++)
    {
        if (data->at(i) < min)
        {
            min = data->at(i);
        }
    }
    // 所有值都减去最小值，从而保证没有负数
    for (int i = 0; i < data->size(); i++)
    {
        (*data)[i] = data->at(i) - min;
    }
    return min;
}

int main(int argc, char **argv)
{
    // 运动压缩
    // bvh_compress();
    // return 0;

    std::string filePathName;
    int method = -1;
    if (argc != 3)
    {
        std::cout << "wrong parameter" << std::endl;
        std::cout << "usage:dfcm file_path_name algorithm" << std::endl;
        std::cout << "file_path_name - 文件名，包含路径" << std::endl;
        std::cout << "algorithm - 压缩算法，0表示简单压缩，1表示DFCM预测压缩" << std::endl;
        return 1;
    }
    else
    { // 读取命令行参数
        std::string tmp(argv[1]);
        filePathName = tmp;
        method = atoi(argv[2]);
        if (0 != method && 1 != method)
        {
            std::cout << "wrong method, must be 0 or 1." << std::endl;
            return 1;
        }
    }
    if (sizeof(float) != sizeof(int)) // 数据类型检查
    {
        std::cerr << "incompatible size" << std::endl;
        std::cout << "size of float: " << sizeof(float) << std::endl;
        std::cout << "size of int: " << sizeof(int) << std::endl;
        return 1;
    }

    // 加载数据
    std::vector<float> data = {};
    load(filePathName, &data);
    std::cout << "data size: " << data.size() << std::endl;

    // 预处理
    std::cout << "minimum of the data: " << preprocess(&data) << std::endl;

    // 压缩
    if (0 == method) // 简单压缩
    {
        std::cout << "简单压缩" << std::endl;
        compress_simple(&data);

        // 解压缩
        data.clear();
        decompress_simple("index_simple.txt", "content_simple.dat", &data);
        std::cout << "decompressed data size: " << data.size() << std::endl;
        save("decompress_simple.txt", &data);
    }
    else if (1 == method) // DFCM预测压缩
    {
        std::cout << "DFCM预测压缩" << std::endl;
        // 初始化Hash表
        std::unordered_map<int, float *> hashTable;
        compress_dfcm(&data, &hashTable);
        std::cout << "hash table size: " << hashTable.size() << std::endl;
        // 输出Hash表并释放内存
        for (auto iter = hashTable.begin(); iter != hashTable.end(); ++iter)
        {
            // std::cout << iter->first << ": [" << iter->second[1] << ", " << iter->second[2] << "]" << std::endl;
            free(iter->second);
        }
        hashTable.clear();
        // 解压缩
        data.clear();
        decompress_dfcm("index_dfcm.txt", "content_dfcm.dat", &data);
        std::cout << "decompressed data size: " << data.size() << std::endl;
        save("decompress_dfcm.txt", &data);
    }

    return 0;
}
