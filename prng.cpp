#include "PRNG.h"  // 引入头文件
#include <algorithm>
#include <numeric>
#include <random>
#include <ctime>

// 构造函数
PRNG::PRNG(std::vector<uint8_t> key, int N, int n)
    : key(key), N(N), n(n), generator(static_cast<unsigned int>(std::time(0))) {
    register_data = initRegister();
}

// 初始化寄存器为密钥的内容
std::vector<uint8_t> PRNG::initRegister() {
    return key;
}

// 生成大小为 n 的伪随机子集索引
std::vector<int> PRNG::generate_subset() {
    std::vector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), generator);
    indices.resize(n);
    return indices;
}

// XOR-MAJ 过滤函数
int PRNG::xorMaj(const std::vector<int>& xorBits, const std::vector<int>& majBits) {
    // XOR 部分
    int xorResult = 0;
    for (int bit : xorBits) {
        xorResult ^= bit;
    }

    // MAJ 部分：对比特进行多数判决
    int majResult = (std::count(majBits.begin(), majBits.end(), 1) >= (majBits.size() / 2)) ? 1 : 0;

    // 返回 XOR 和 MAJ 的结果结合
    return xorResult ^ majResult;
}

// 生成密钥流
std::pair<int, std::vector<int>> PRNG::generateKeystream(int a, int b) {
    while (true) {
        // 生成一个子集索引
        std::vector<int> permutedIndices = generate_subset();

        // 根据 permutedIndices 从寄存器中提取对应的比特
        std::vector<int> registerBits;
        for (int idx : permutedIndices) {
            int byteIdx = idx / 8;
            int bitIdx = idx % 8;
            uint8_t byte = register_data[byteIdx];
            int bit = (byte >> (7 - bitIdx)) & 1; // 提取相关比特
            registerBits.push_back(bit);
        }

        // XOR 部分：前 a 个比特
        std::vector<int> xorBits(registerBits.begin(), registerBits.begin() + a);

        // MAJ 部分：接下来的 b 个比特
        std::vector<int> majBits(registerBits.begin() + a, registerBits.begin() + a + b);

        // 应用 XOR-MAJ 过滤函数
        int filteredOutput = xorMaj(xorBits, majBits);

        return {filteredOutput, permutedIndices};
    }
}
