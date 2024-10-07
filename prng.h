#ifndef PRNG_H
#define PRNG_H

#include <vector>
#include <cstdint>
#include <random>
#include <utility>

class PRNG {
private:
    std::vector<uint8_t> key;
    int N;
    int n;
    std::vector<uint8_t> register_data;
    std::mt19937 generator;

public:
    // 构造函数
    PRNG(std::vector<uint8_t> key, int N, int n);

    // 初始化寄存器
    std::vector<uint8_t> initRegister();

    // 生成大小为 n 的伪随机子集索引
    std::vector<int> generate_subset();

    // XOR-MAJ 过滤函数
    int xorMaj(const std::vector<int>& xorBits, const std::vector<int>& majBits);

    // 生成密钥流
    std::pair<int, std::vector<int>> generateKeystream(int a, int b);
};

#endif // PRNG_H

