#ifndef VERIFIER_UTILS_H
#define VERIFIER_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <random>
#include <utility>
#include <cstdint>
#include <fstream>
#include <algorithm>

#define MAX_SIZE 256

// 将字节转换为比特串
std::string bytesToBitstring(const std::vector<uint8_t> &byte_data);

// XOR-MAJ 过滤函数
int xorMaj(const std::vector<int> &xor_bits, const std::vector<int> &maj_bits);

// 生成一个随机排列的索引集合
std::vector<int> generatePermutedIndices(const std::vector<int> &t_indices, int total_size = 74, int total_range = 256);

// 恢复密钥
std::string recover_key(const std::vector<int> &solution, const std::vector<int> &t_indices, int total_key_size = 256);

// 打印矩阵
void printMatrix(const std::vector<std::vector<int>> &matrix);

// 从成功的实例中生成矩阵
std::pair<std::vector<std::bitset<MAX_SIZE>>, std::vector<int>> generateMatrixFromInstances(
    const std::vector<std::pair<int, std::vector<int>>> &successfulInstances, const std::vector<int> &tIndices, int totalKeySize = 256);

// 验证解的正确性
std::vector<std::vector<int>> verifySolutions(const std::vector<std::bitset<MAX_SIZE>> &A, const std::vector<int> &b, const std::vector<std::vector<int>> &solutions);

// 检查 MAJ 部分
bool checkMajPart(const std::vector<int> &guess_indices, const std::vector<int> &permuted_indices);

// 高斯消元法求解（带自由变量）
std::vector<std::vector<int>> gaussianEliminationMod2WithFreeVariables(std::vector<std::bitset<MAX_SIZE>> &A, std::vector<int> &b, int rows, int cols);

// 选择随机索引
std::vector<int> selectRandomIndices(int t);

// 计算一个字节中 1 的数量
inline int hammingWeightByte(uint8_t byte);

// 计算 32 字节数组的汉明重量
int hammingWeight(const uint8_t *a, const uint8_t *b, size_t size = 32);

// 将一行的结果写入文件
void writeResultToFile(std::ofstream &outFile, uint8_t result_bit, const uint8_t *xor_vector, const uint8_t *maj_vector, size_t size = 32);

// 使用 Windows 内存映射处理单个文件
void processSingleFileWithMemoryMapping(const std::string &inputFilePath, const std::vector<int> &t_indices, std::ofstream &outFile);

// 处理多个文件
void processFilesWithMemoryMapping(const std::string &folderPath, const std::vector<int> &t_indices, const std::string &outputFilePath, int num_files);

#endif // VERIFIER_UTILS_H
