#include "bvh_reader.h"

#include <sys/types.h>
#include <sstream>
#include <string>
#include <dirent.h>

BVHReader::BVHReader()
{
    std::cout << "=========[init BVHReader]=========" << std::endl;
    this->mJoints = 0;
    this->mData.clear();
    this->mCoordData.clear();
}

BVHReader::~BVHReader()
{
    std::cout << "=========[release BVHReader]=========" << std::endl;
    for (int i = 0; i < this->mData.size(); i++)
    {
        this->mData[i].clear();
    }
    this->mData.clear();
    for (int i = 0; i < this->mCoordData.size(); i++)
    {
        this->mCoordData[i].clear();
    }
    this->mCoordData.clear();
    this->mJoints = 0;
}

int BVHReader::Joints()
{
    return this->mJoints;
}

int BVHReader::Frames()
{
    return this->mData.size();
}

int BVHReader::Dimensions()
{
    if (this->mData.size() == 0)
    {
        return 0;
    }
    return this->mData[0].size();
}

std::vector<std::vector<float>> &BVHReader::Data()
{
    return this->mData;
}

std::vector<std::vector<float>> &BVHReader::CoordData()
{
    if (this->mCoordData.size() == 0)
    {
        /* 根据原始数据计算坐标 */
        for (int i = 0; i < this->mData.size(); i++)
        {
            std::vector<float> coord_data;
            this->genFrameCoordinates(this->mData[i], coord_data);
            this->mCoordData.push_back(coord_data);
        }
    }
    return this->mCoordData;
}

bool BVHReader::Load(const std::string &file_name_path)
{
    std::fstream bvh_file;
    bvh_file.open(file_name_path);
    if (bvh_file.is_open())
    {
        std::cout << "file is openned" << std::endl;
        std::string line;
        // read BVH structure
        int joints = 0;
        int dims = 0;
        int frames = 0;
        while (std::getline(bvh_file, line))
        {
            if (line.find("ROOT") != std::string::npos)
            {
                joints++;
                dims += 6;
            }
            if (line.find("JOINT") != std::string::npos)
            {
                joints++;
                dims += 3;
            }
            if (line.find("Frames") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string item;
                std::vector<std::string> tokens;
                while (std::getline(ss, item, ' '))
                {
                    tokens.push_back(item);
                }
                frames = atoi(tokens[1].c_str());
            }
            if (line.find("Frame Time") != std::string::npos)
            {
                break;
            }
        }
        // read BVH data
        std::cout << "joints: " << joints << " dimensions: " << dims << " frames: " << frames << std::endl;
        // check if data struct matches the previous one
        if (this->mData.size() != 0 && (this->mData[0].size() != dims || this->mJoints != joints))
        {
            std::cout << "invalid data" << std::endl;
            bvh_file.close();
        }
        this->mJoints = joints;

        if (frames > 0)
        {
            for (int i = 0; i < frames; i++)
            {
                if (std::getline(bvh_file, line))
                {
                    std::vector<float> tmp;
                    std::stringstream ss(line);
                    std::string item;
                    std::vector<std::string> tokens;
                    while (std::getline(ss, item, ' '))
                    {
                        tokens.push_back(item);
                    }
                    for (int j = 0; j < dims; j++)
                    {
                        tmp.push_back(atof(tokens[j].c_str()));
                    }
                    this->mData.push_back(tmp);
                }
            }
        }
        bvh_file.close();
        return true;
    }
    std::cout << "failed to open file" << std::endl;
    return false;
}

bool BVHReader::LoadDir(const std::string &dir)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        std::cout << "failed" << std::endl;
        return false;
    }
    while ((dirp = readdir(dp)) != NULL)
    {
        std::string tmp(dirp->d_name);
        if (tmp.find(".bvh") != std::string::npos)
        {
            tmp = dir + "/" + tmp;
            std::cout << tmp << std::endl;
            this->Load(tmp);
        }
    }
    closedir(dp);
    return true;
}

bool BVHReader::LoadDirAndSave2SingleFile(const std::string &dir, const std::string &file_name_path)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        std::cout << "failed" << std::endl;
        return false;
    }
    std::ofstream output;
    output.open(file_name_path.c_str());
    while ((dirp = readdir(dp)) != NULL && output.is_open())
    {
        std::string tmp(dirp->d_name);
        if (tmp.find(".bvh") != std::string::npos)
        {
            tmp = dir + "/" + tmp;
            std::cout << tmp << std::endl;
            std::fstream bvh_file;
            bvh_file.open(tmp);
            if (bvh_file.is_open() && output.is_open())
            {
                std::cout << "file is openned" << std::endl;
                std::string line;
                // read BVH structure
                int joints = 0;
                int dims = 0;
                int frames = 0;
                while (std::getline(bvh_file, line))
                {
                    if (line.find("ROOT") != std::string::npos)
                    {
                        joints++;
                        dims += 6;
                    }
                    if (line.find("JOINT") != std::string::npos)
                    {
                        joints++;
                        dims += 3;
                    }
                    if (line.find("Frames") != std::string::npos)
                    {
                        std::stringstream ss(line);
                        std::string item;
                        std::vector<std::string> tokens;
                        while (std::getline(ss, item, ' '))
                        {
                            tokens.push_back(item);
                        }
                        frames = atoi(tokens[1].c_str());
                    }
                    if (line.find("Frame Time") != std::string::npos)
                    {
                        break;
                    }
                }
                // read BVH data
                std::cout << "joints: " << joints << " dimensions: " << dims << " frames: " << frames << std::endl;
                // check if data struct matches the previous one
                if (this->mData.size() != 0 && (this->mData[0].size() != dims || this->mJoints != joints))
                {
                    std::cout << "invalid data" << std::endl;
                    bvh_file.close();
                    return false;
                }
                this->mJoints = joints;

                if (frames > 0)
                {
                    for (int i = 0; i < frames; i++)
                    {
                        if (std::getline(bvh_file, line))
                        {
                            output << line << std::endl;
                        }
                    }
                }
            }
            else {
                std::cout << "failed to open bvh file" << std::endl;
            }
            bvh_file.close();
        }
    }
    closedir(dp);
    output.close();
    return true;
}

bool BVHReader::SaveData2TxtFile(const std::string &file_name_path)
{
    std::ofstream writer(file_name_path);
    if (writer.is_open())
    {
        writer << this->mData.size() << std::endl;
        writer << std::setiosflags(std::ios::fixed);
        for (int i = 0; i < this->mData.size(); i++)
        {
            for (int j = 0; j < this->mData[i].size(); j++)
            {
                writer << std::setprecision(4) << this->mData[i][j] << " ";
            }
            writer << std::endl;
        }
    }
    writer.close();
}

bool BVHReader::SaveDiffData2TxtFile(const std::string &file_name_path)
{
    std::ofstream writer(file_name_path);
    if (writer.is_open())
    {
        writer << this->mData.size() << std::endl;
        writer << std::setiosflags(std::ios::fixed);
        for (int i = 0; i < this->mData.size(); i++)
        {
            for (int j = 0; j < this->mData[i].size(); j++)
            {
                float value = this->mData[i][j];
                if (i > 0)
                {
                    value = this->mData[i][j] - this->mData[i - 1][j];
                }
                writer << std::setprecision(4) << value << " ";
            }
            writer << std::endl;
        }
    }
    writer.close();
}

int BVHReader::FlatData(float **data)
{
    if (this->mData.size() == 0)
    {
        return 0;
    }
    int rows = this->mData.size();
    int cols = this->mData[0].size();
    *data = new float[rows * cols];
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            (*data)[i * cols + j] = this->mData[i][j];
        }
    }
    return rows * cols;
}

int BVHReader::FlatCoordData(float **data)
{
    if (this->mCoordData.size() == 0)
    {
        return 0;
    }
    int rows = this->mCoordData.size();
    int cols = 75;
    *data = new float[rows * cols];
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            (*data)[i * cols + j] = this->mCoordData[i][j];
        }
    }
}

bool BVHReader::genFrameCoordinates(std::vector<float> &row_data, std::vector<float> &coord_data)
{
    if (row_data.size() == 0)
    {
        return false;
    }
    float lRotationMtx[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float lRootRotMtx[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}; //人为设置根节点的旋转矩阵
    float lTrans[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float lDelta[3] = {0, 0, 0}; //原始BVH文件中两帧间根节点的位置差
    // lTrans[3][0] = row_data[0];
    // lTrans[3][1] = row_data[1];
    // lTrans[3][2] = row_data[2];
    lTrans[3][0] = 0;
    lTrans[3][1] = 0;
    lTrans[3][2] = 0;

    /* 计算root旋转矩阵 */
    float lRadius = row_data[3] * PI / 180.0f;
    float lRotationX[4][4] = {{1, 0, 0, 0},
                              {0, std::cos(lRadius), std::sin(lRadius), 0},
                              {0, -std::sin(lRadius), std::cos(lRadius), 0},
                              {0, 0, 0, 1}};
    this->mtxMultiple4(lRootRotMtx, lRotationX, lRootRotMtx);
    lRadius = row_data[4] * PI / 180.0f;
    float lRotationY[4][4] = {{std::cos(lRadius), 0, -std::sin(lRadius), 0},
                              {0, 1, 0, 0},
                              {std::sin(lRadius), 0, std::cos(lRadius), 0},
                              {0, 0, 0, 1}};
    this->mtxMultiple4(lRootRotMtx, lRotationY, lRootRotMtx);
    lRadius = row_data[5] * PI / 180.0f;
    float lRotationZ[4][4] = {{std::cos(lRadius), std::sin(lRadius), 0, 0},
                              {-std::sin(lRadius), std::cos(lRadius), 0, 0},
                              {0, 0, 1, 0},
                              {0, 0, 0, 1}};
    this->mtxMultiple4(lRootRotMtx, lRotationZ, lRootRotMtx);

    /* 计算全局旋转矩阵 */
    this->mtxMultiple4(lRotationMtx, lRotationMtx, lRootRotMtx);
    this->mtxMultiple4(lRotationMtx, lRotationMtx, lTrans);
    /* 保存root坐标 */
    // coord_data.push_back(row_data[0]);
    // coord_data.push_back(row_data[1]);
    // coord_data.push_back(row_data[2]);
    coord_data.push_back(0);
    coord_data.push_back(0);
    coord_data.push_back(0);

    /* 计算关节坐标 */
    this->calcJointCoordinate("lhipjoint", lRotationMtx, row_data, coord_data);
    this->calcJointCoordinate("rhipjoint", lRotationMtx, row_data, coord_data);
    this->calcJointCoordinate("lowerback", lRotationMtx, row_data, coord_data);

    return true;
}

bool BVHReader::calcJointCoordinate(const std::string joint_name, float argRotMtx[][4], std::vector<float> &row_data, std::vector<float> &coord_data)
{
    float lRotation[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float lTrans[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    /* 初始化节点的局部坐标为偏移值 */
    float lPosition[4] = {0, 0, 0, 1};
    float row_rotation_data[3] = {0, 0, 0};
    std::string next_joint = "";
    /* 目前先根据固定的BVH文件格式获取，后面再改成自动从文件读取 */
    if (joint_name == "lhipjoint")
    {
        /* JOINT lhipjoint
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[6];
        row_rotation_data[1] = row_data[7];
        row_rotation_data[2] = row_data[8];
        next_joint = "lfemur";
    }
    else if (joint_name == "lfemur")
    {
        /* JOINT lfemur
                   {
                   OFFSET 1.65674 -1.80282 0.62477 */
        lPosition[0] = 1.65674f;
        lPosition[1] = -1.80282f;
        lPosition[2] = 0.62477;
        row_rotation_data[0] = row_data[9];
        row_rotation_data[1] = row_data[10];
        row_rotation_data[2] = row_data[11];
        next_joint = "ltibia";
    }
    else if (joint_name == "ltibia")
    {
        /* JOINT ltibia
                   {
                   OFFSET 2.59720 -7.13576 0.00000 */
        lPosition[0] = 2.5972f;
        lPosition[1] = -7.13576f;
        row_rotation_data[0] = row_data[12];
        row_rotation_data[1] = row_data[13];
        row_rotation_data[2] = row_data[14];
        next_joint = "lfoot";
    }
    else if (joint_name == "lfoot")
    {
        /* JOINT lfoot
                   {
                   OFFSET 2.49236 -6.84770 0.00000 */
        lPosition[0] = 2.49236f;
        lPosition[1] = -6.84770f;
        row_rotation_data[0] = row_data[15];
        row_rotation_data[1] = row_data[16];
        row_rotation_data[2] = row_data[17];
        next_joint = "ltoes";
    }
    else if (joint_name == "ltoes")
    {
        /* JOINT ltoes
                   {
                   OFFSET 0.19704 -0.54136 2.14581 */
        lPosition[0] = 0.19704f;
        lPosition[1] = -0.54136f;
        lPosition[2] = 2.14581f;
        row_rotation_data[0] = row_data[18];
        row_rotation_data[1] = row_data[19];
        row_rotation_data[2] = row_data[20];
        next_joint = "end";
    }
    else if (joint_name == "rhipjoint")
    {
        /* JOINT rhipjoint
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[21];
        row_rotation_data[1] = row_data[22];
        row_rotation_data[2] = row_data[23];
        next_joint = "rfemur";
    }
    else if (joint_name == "rfemur")
    {
        /* JOINT rfemur
                   {
                   OFFSET -1.61070 -1.80282 0.62476 */
        lPosition[0] = -1.61070f;
        lPosition[1] = -1.80282f;
        lPosition[2] = 0.62476;
        row_rotation_data[0] = row_data[24];
        row_rotation_data[1] = row_data[25];
        row_rotation_data[2] = row_data[26];
        next_joint = "rtibia";
    }
    else if (joint_name == "rtibia")
    {
        /* JOINT rtibia
                   {
                   OFFSET -2.59502 -7.12977 0.00000 */
        lPosition[0] = -2.59502f;
        lPosition[1] = -7.12977f;
        row_rotation_data[0] = row_data[27];
        row_rotation_data[1] = row_data[28];
        row_rotation_data[2] = row_data[29];
        next_joint = "rfoot";
    }
    else if (joint_name == "rfoot")
    {
        /* JOINT rfoot
                   {
                   OFFSET -2.46780 -6.78024 0.00000 */
        lPosition[0] = -2.46780f;
        lPosition[1] = -6.78024f;
        row_rotation_data[0] = row_data[30];
        row_rotation_data[1] = row_data[31];
        row_rotation_data[2] = row_data[32];
        next_joint = "rtoes";
    }
    else if (joint_name == "rtoes")
    {
        /* JOINT rtoes
                   {
                   OFFSET -0.23024 -0.63258 2.13368 */
        lPosition[0] = -0.23024f;
        lPosition[1] = -0.63258f;
        lPosition[2] = 2.13368f;
        row_rotation_data[0] = row_data[33];
        row_rotation_data[1] = row_data[34];
        row_rotation_data[2] = row_data[35];
        next_joint = "end";
    }
    else if (joint_name == "lowerback")
    {
        /* JOINT lowerback
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[36];
        row_rotation_data[1] = row_data[37];
        row_rotation_data[2] = row_data[38];
        next_joint = "upperback";
    }
    else if (joint_name == "upperback")
    {
        /* JOINT upperback
                   {
                   OFFSET 0.01961 2.05450 -0.14112 */
        lPosition[0] = 0.01961f;
        lPosition[1] = 2.05450f;
        lPosition[2] = -0.14112f;
        row_rotation_data[0] = row_data[39];
        row_rotation_data[1] = row_data[40];
        row_rotation_data[2] = row_data[41];
        next_joint = "thorax";
    }
    else if (joint_name == "thorax")
    {
        /* JOINT thorax
                   {
                   OFFSET 0.01021 2.06436 -0.05921 */
        lPosition[0] = 0.01021f;
        lPosition[1] = 2.06436f;
        lPosition[2] = -0.05921f;
        row_rotation_data[0] = row_data[42];
        row_rotation_data[1] = row_data[43];
        row_rotation_data[2] = row_data[44];
    }
    else if (joint_name == "lowerneck")
    {
        /* JOINT lowerneck
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[45];
        row_rotation_data[1] = row_data[46];
        row_rotation_data[2] = row_data[47];
        next_joint = "upperneck";
    }
    else if (joint_name == "upperneck")
    {
        /* JOINT upperneck
                   {
                   OFFSET 0.00713 1.56711 0.14968 */
        lPosition[0] = 0.00713f;
        lPosition[1] = 1.56711f;
        lPosition[2] = 0.14968f;
        row_rotation_data[0] = row_data[48];
        row_rotation_data[1] = row_data[49];
        row_rotation_data[2] = row_data[50];
        next_joint = "head";
    }
    else if (joint_name == "head")
    {
        /* JOINT head
                   {
                   OFFSET 0.03429 1.56041 -0.10006 */
        lPosition[0] = 0.03429f;
        lPosition[1] = 1.56041f;
        lPosition[2] = -0.10006f;
        row_rotation_data[0] = row_data[51];
        row_rotation_data[1] = row_data[52];
        row_rotation_data[2] = row_data[53];
        next_joint = "end";
    }
    else if (joint_name == "lclavicle")
    {
        /* JOINT lclavicle
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[54];
        row_rotation_data[1] = row_data[55];
        row_rotation_data[2] = row_data[56];
        next_joint = "lhumerus";
    }
    else if (joint_name == "lhumerus")
    {
        /* JOINT lhumerus
                   {
                   OFFSET 3.54205 0.90436 -0.17364 */
        lPosition[0] = 3.54205f;
        lPosition[1] = 0.90436f;
        lPosition[2] = -0.17364f;
        row_rotation_data[0] = row_data[57];
        row_rotation_data[1] = row_data[58];
        row_rotation_data[2] = row_data[59];
        next_joint = "lradius";
    }
    else if (joint_name == "lradius")
    {
        /* JOINT lradius
                   {
                   OFFSET 4.86513 -0.00000 -0.00000 */
        lPosition[0] = 4.86513f;
        row_rotation_data[0] = row_data[60];
        row_rotation_data[1] = row_data[61];
        row_rotation_data[2] = row_data[62];
        next_joint = "lwrist";
    }
    else if (joint_name == "lwrist")
    {
        /* JOINT lwrist
                   {
                   OFFSET 3.35554 -0.00000 0.00000 */
        lPosition[0] = 3.35554f;
        row_rotation_data[0] = row_data[63];
        row_rotation_data[1] = row_data[64];
        row_rotation_data[2] = row_data[65];
        next_joint = "end"; // 不计算lhand、lfinger、lthumb
    }
    else if (joint_name == "lhand")
    {
        /* JOINT lhand
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[66];
        row_rotation_data[1] = row_data[67];
        row_rotation_data[2] = row_data[68];
    }
    else if (joint_name == "lfingers")
    {
        /* JOINT lfingers
                   {
                   OFFSET 0.66117 -0.00000 0.00000 */
        lPosition[0] = 0.66117f;
        row_rotation_data[0] = row_data[69];
        row_rotation_data[1] = row_data[70];
        row_rotation_data[2] = row_data[71];
    }
    else if (joint_name == "lthumb")
    {
        /* JOINT lthumb
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[72];
        row_rotation_data[1] = row_data[73];
        row_rotation_data[2] = row_data[74];
    }
    else if (joint_name == "rclavicle")
    {
        /* JOINT rclavicle
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[75];
        row_rotation_data[1] = row_data[76];
        row_rotation_data[2] = row_data[77];
        next_joint = "rhumerus";
    }
    else if (joint_name == "rhumerus")
    {
        /* JOINT rhumerus
                   {
                   OFFSET -3.49802 0.75994 -0.32616 */
        lPosition[0] = -3.49802f;
        lPosition[1] = 0.75994f;
        lPosition[2] = -0.32616f;
        row_rotation_data[0] = row_data[78];
        row_rotation_data[1] = row_data[79];
        row_rotation_data[2] = row_data[80];
        next_joint = "rradius";
    }
    else if (joint_name == "rradius")
    {
        /* JOINT rradius
                   {
                   OFFSET -5.02649 -0.00000 0.00000 */
        lPosition[0] = -5.02649f;
        row_rotation_data[0] = row_data[81];
        row_rotation_data[1] = row_data[82];
        row_rotation_data[2] = row_data[83];
        next_joint = "rwrist";
    }
    else if (joint_name == "rwrist")
    {
        /* JOINT rwrist
                   {
                   OFFSET -3.36431 -0.00000 0.00000 */
        lPosition[0] = -3.36431f;
        row_rotation_data[0] = row_data[84];
        row_rotation_data[1] = row_data[85];
        row_rotation_data[2] = row_data[86];
        next_joint = "end"; // 不计算rhand、rfinger、rthumb
    }
    else if (joint_name == "rhand")
    {
        /* JOINT rhand
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[87];
        row_rotation_data[1] = row_data[88];
        row_rotation_data[2] = row_data[89];
    }
    else if (joint_name == "rfingers")
    {
        /* JOINT rfingers
                   {
                   OFFSET -0.73041 -0.00000 0.00000 */
        lPosition[0] = -0.73041f;
        row_rotation_data[0] = row_data[90];
        row_rotation_data[1] = row_data[91];
        row_rotation_data[2] = row_data[92];
    }
    else if (joint_name == "rthumb")
    {
        /* JOINT rthumb
                   {
                   OFFSET 0 0 0 */
        row_rotation_data[0] = row_data[93];
        row_rotation_data[1] = row_data[94];
        row_rotation_data[2] = row_data[95];
    }
    else
    {
        std::cout << "invalid joint" << std::endl;
        return false;
    }

    //使用OFFSET初始化该节点的平移矩阵
    lTrans[3][0] = lPosition[0];
    lTrans[3][1] = lPosition[1];
    lTrans[3][2] = lPosition[2];
    /* 计算joint的旋转矩阵 */
    float lRadian = row_rotation_data[0] * PI / 180.0f;
    float lRotationX[4][4] = {{1, 0, 0, 0}, {0, std::cos(lRadian), std::sin(lRadian), 0}, {0, -std::sin(lRadian), std::cos(lRadian), 0}, {0, 0, 0, 1}};
    this->mtxMultiple4(lRotation, lRotationX, lRotation);

    lRadian = row_rotation_data[1] * PI / 180.0f;
    float lRotationY[4][4] = {{std::cos(lRadian), 0, -std::sin(lRadian), 0}, {0, 1, 0, 0}, {std::sin(lRadian), 0, std::cos(lRadian), 0}, {0, 0, 0, 1}};
    this->mtxMultiple4(lRotation, lRotationY, lRotation);

    lRadian = row_rotation_data[2] * PI / 180.0f;
    float lRotationZ[4][4] = {{std::cos(lRadian), std::sin(lRadian), 0, 0}, {-std::sin(lRadian), std::cos(lRadian), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    this->mtxMultiple4(lRotation, lRotationZ, lRotation);

    //计算该节点随父节点变换后的坐标
    double lTempPos[3];
    for (int i = 0; i < 3; i++)
    {
        lTempPos[i] = lPosition[0] * argRotMtx[0][i] +
                      lPosition[1] * argRotMtx[1][i] +
                      lPosition[2] * argRotMtx[2][i] +
                      lPosition[3] * argRotMtx[3][i];
    }
    //计算该节点的变换矩阵
    this->mtxMultiple4(lRotation, lRotation, lTrans);
    //计算该节点随父节点的变换矩阵
    this->mtxMultiple4(lRotation, lRotation, argRotMtx);

    //保存该节点的坐标
    coord_data.push_back(lTempPos[0]);
    coord_data.push_back(lTempPos[1]);
    coord_data.push_back(lTempPos[2]);

    //计算其子节点的坐标
    if (joint_name == "thorax")
    {
        this->calcJointCoordinate("lowerneck", lRotation, row_data, coord_data);
        this->calcJointCoordinate("lclavicle", lRotation, row_data, coord_data);
        this->calcJointCoordinate("rclavicle", lRotation, row_data, coord_data);
    }
    else
    {
        if (next_joint != "end")
        {
            this->calcJointCoordinate(next_joint, lRotation, row_data, coord_data);
        }
    }

    return true;
}

bool BVHReader::mtxMultiple4(float argResult[][4], float argLeft[][4], float argRight[][4])
{
    double lTempResult[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            double lSum = 0;
            for (int k = 0; k < 4; k++)
            {
                lSum += argLeft[i][k] * argRight[k][j];
            }
            lTempResult[i][j] = lSum;
        }
    }
    for (int m = 0; m < 4; m++)
    {
        for (int n = 0; n < 4; n++)
        {
            argResult[m][n] = lTempResult[m][n];
        }
    }
}
