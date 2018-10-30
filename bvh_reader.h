#ifndef BVH_READER_H
#define BVH_READER_H

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

#define PI 3.14159265

class BVHReader {
private:
int mJoints;     // number of joints
std::vector<std::vector<float> > mData;    // 2-d raw frame data
std::vector<std::vector<float> > mCoordData;    // 2-d coordinate frame data

public:
BVHReader();
~BVHReader();
int Joints();
int Frames();
int Dimensions();
/*
 * 2-d row frame data, containing root position
 */
std::vector<std::vector<float> >& Data();
/*
 * 2-d coordinate frame data
 */
std::vector<std::vector<float> >& CoordData();
/*
 * 1-d frame data
 */
int FlatData(float** data);
int FlatCoordData(float** data);
/*
 * Load speific BVH file
 */
bool Load(const std::string &file_name_path);
/*
 * Load BVH files under specific directory
 */
bool LoadDir(const std::string &dir);
bool LoadDirAndSave2SingleFile(const std::string &dir, const std::string &file_name_path);
bool SaveData2TxtFile(const std::string &file_name_path);
bool SaveDiffData2TxtFile(const std::string &file_name_path);

private:
bool genFrameCoordinates(std::vector<float>& row_data, std::vector<float>& coord_data);
bool calcJointCoordinate(const std::string joint_name, float argRotMtx[][4], std::vector<float>& row_data, std::vector<float>& coord_data);
bool mtxMultiple4(float argResult[][4], float argLeft[][4], float argRight[][4]);
};


#endif
