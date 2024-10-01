#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <bitset>
#include <iterator>

using namespace std;

class PRNG {
private:
    vector<uint8_t> key;
    int N;
    int n;
    vector<uint8_t> register_data;
    std::mt19937 generator;

public:
    PRNG(vector<uint8_t> key, int N, int n) 
        : key(key), N(N), n(n), generator(static_cast<unsigned int>(std::time(0))) {
        register_data = initRegister();
    }


    vector<uint8_t> initRegister() {
        // 初始化寄存器为密钥的内容
        return key;
    }

      std::vector<int> generate_subset()
    {
        // 生成大小为 n 的伪随机子集索引
        std::vector<int> indices;
        for (int i = 0; i < N; ++i)
            indices.push_back(i);
        std::shuffle(indices.begin(), indices.end(), generator);
        indices.resize(n);
        return indices;
    }
 

    int xorMaj(const vector<int>& xorBits, const vector<int>& majBits) {
        // XOR-MAJ 过滤函数
        // XOR 部分
        int xorResult = 0;
        for (int bit : xorBits) {
            xorResult ^= bit;
        }

        // MAJ 部分：对比特进行多数判决
        int majResult = (count(majBits.begin(), majBits.end(), 1) >= (majBits.size() / 2)) ? 1 : 0;

        // 返回 XOR 和 MAJ 的结果结合
        return xorResult ^ majResult;
    }

    pair<int, vector<int>> generateKeystream(int a, int b) {
        while (true) {
            // 生成一个子集索引
            vector<int> permutedIndices = generate_subset();

            // 根据 permutedIndices 从寄存器中提取对应的比特
            vector<int> registerBits;
            for (int idx : permutedIndices) {
                int byteIdx = idx / 8;
                int bitIdx = idx % 8;
                uint8_t byte = register_data[byteIdx];
                int bit = (byte >> (7 - bitIdx)) & 1; // 提取相关比特
                registerBits.push_back(bit);
            }

            // XOR 部分：前 a 个比特
            vector<int> xorBits(registerBits.begin(), registerBits.begin() + a);

            // MAJ 部分：接下来的 b 个比特
            vector<int> majBits(registerBits.begin() + a, registerBits.begin() + a + b);

            // 应用 XOR-MAJ 过滤函数
            int filteredOutput = xorMaj(xorBits, majBits);

            return {filteredOutput, permutedIndices};
        }
    }
};

// int main() {
//     // 示例使用 PRNG 类
//     vector<uint8_t> key = {
//         0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
//         0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
//         0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
//         0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0
//     };
//     int N = 256;
//     int n = 74;

//     PRNG prng(key, N, n);
//     int a = 10;
//     int b = 64;

//     // 生成一些密钥流比特
//     for (int i = 0; i < 10; ++i) {
//         auto [bit, permutedIndices] = prng.generateKeystream(a, b);
//         cout << "生成的密钥位: " << bit << "\n索引: ";
//         for (int idx : permutedIndices) {
//             cout << idx << " ";
//         }
//         cout << endl;
//     }

//     return 0;
// }
