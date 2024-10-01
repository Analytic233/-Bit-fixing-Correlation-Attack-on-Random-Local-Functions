#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <bitset>
#include <random>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <windows.h>
#include "prng.cpp"

using namespace std;

std::mt19937 generator(static_cast<unsigned int>(std::time(0)));

// 将字节转换为比特串
string bytesToBitstring(const vector<uint8_t> &byte_data)
{
    string bitstring;
    for (auto byte : byte_data)
    {
        bitset<8> bits(byte);
        bitstring += bits.to_string();
    }
    return bitstring;
}
int xorMaj(const vector<int> &xor_bits, const vector<int> &maj_bits)
{
    // XOR 部分
    int xor_result = 0;
    for (int bit : xor_bits)
    {
        xor_result ^= bit;
    }

    // MAJ 部分
    int maj_result = (count(maj_bits.begin(), maj_bits.end(), 1) > 31) ? 1 : 0;

    // 返回 XOR 和 MAJ 的结果组合
    return xor_result ^ maj_result;
}
vector<int> generatePermutedIndices(const vector<int> &t_indices, int total_size = 74, int total_range = 256)
{
    // Step 1: 生成从 0 到 total_range - 1 的所有可能索引
    vector<int> all_indices(total_range);
    for (int i = 0; i < total_range; ++i)
    {
        all_indices[i] = i;
    }

    // Step 2: 确保 t_indices 中的元素是唯一的
    vector<int> unique_t_indices = t_indices;
    sort(unique_t_indices.begin(), unique_t_indices.end());
    unique_t_indices.erase(unique(unique_t_indices.begin(), unique_t_indices.end()), unique_t_indices.end());

    // Step 3: 确保 t_indices 中的数少于或等于 64 个
    if (unique_t_indices.size() > 64)
    {
        throw invalid_argument("t_indices should have at most 64 unique elements");
    }

    // Step 4: 随机选择其他的数，排除 t_indices 中的数
    vector<int> remaining_indices;
    for (int i = 0; i < total_range; ++i)
    {
        if (find(unique_t_indices.begin(), unique_t_indices.end(), i) == unique_t_indices.end())
        {
            remaining_indices.push_back(i);
        }
    }

    // auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    // mt19937 generator(seed);
    shuffle(remaining_indices.begin(), remaining_indices.end(), generator);

    // Step 5: 创建 permuted_indices 列表，并将 t_indices 放入最后 64 个位置
    vector<int> permuted_indices(remaining_indices.begin(), remaining_indices.begin() + total_size - unique_t_indices.size());
    permuted_indices.insert(permuted_indices.end(), unique_t_indices.begin(), unique_t_indices.end());

    // Step 6: 随机打乱 permuted_indices
    shuffle(permuted_indices.begin() + 10, permuted_indices.begin() + 74, generator);

    return permuted_indices;
}

// 恢复密钥
string recover_key(const vector<int> &solution, const vector<int> &t_indices, int total_key_size = 256)
{
    // 初始化全零的 key，长度为 total_key_size
    vector<int> key(total_key_size, 0);

    // 创建 all_indices 和 adjusted_t_indices
    vector<int> all_indices(total_key_size);
    for (int i = 0; i < total_key_size; ++i)
    {
        all_indices[i] = i;
    }

    vector<int> adjusted_t_indices = t_indices;

    // 计算 remaining_indices，排除 t_indices 中的元素
    vector<int> remaining_indices;
    for (int idx : all_indices)
    {
        if (find(adjusted_t_indices.begin(), adjusted_t_indices.end(), idx) == adjusted_t_indices.end())
        {
            remaining_indices.push_back(idx);
        }
    }

    // 通过 remaining_indices 将 solution 的位恢复到 key 中
    for (size_t j = 0; j < remaining_indices.size(); ++j)
    {
        int i = remaining_indices[j];
        key[i] = solution[j]; // 将 solution 第 j 位对应到 key 的第 i 位
    }

    // 将 t_indices 中对应的位置全部置为 1
    for (int t : adjusted_t_indices)
    {
        key[t] = 1;
    }

    // 将 key 转换为 bitstring
    string bitstring;
    for (int bit : key)
    {
        bitstring += to_string(bit);
    }

    return bitstring;
}

void printMatrix(const std::vector<std::vector<int>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (const auto &element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}
// 生成矩阵
pair<vector<vector<int>>, vector<int>> generateMatrixFromInstances(
    const vector<pair<int, vector<int>>> &successfulInstances, const vector<int> &tIndices, int totalKeySize = 256)
{
    // 随机选择 245 个实例

    vector<pair<int, vector<int>>> selectedInstances = successfulInstances;
    size_t maxRows = 245;

    if (successfulInstances.size() > maxRows)
    {

        shuffle(selectedInstances.begin(), selectedInstances.end(), generator);
        selectedInstances.resize(maxRows);
    }

    // 设置列数
    vector<int> allIndices(totalKeySize);
    iota(allIndices.begin(), allIndices.end(), 0);
    vector<int> remainingIndices;

    for (int idx : allIndices)
    {
        if (find(tIndices.begin(), tIndices.end(), idx) == tIndices.end())
        {
            remainingIndices.push_back(idx);
        }
    }

    size_t rows = selectedInstances.size();

    size_t cols = remainingIndices.size();

    vector<vector<int>> matrix(rows, vector<int>(cols, 0));
    vector<int> resultBitsFlipped(rows, 0);

    // 填充矩阵
    for (size_t row = 0; row < rows; ++row)
    {
        auto [resultBit, permutedIndices] = selectedInstances[row];
        vector<int> xorIndices(permutedIndices.begin(), permutedIndices.begin() + 10);

        for (int xorIdx : xorIndices)
        {
            auto it = find(remainingIndices.begin(), remainingIndices.end(), xorIdx);
            if (it != remainingIndices.end())
            {
                int col = distance(remainingIndices.begin(), it);
                if (col > 0)
                {
                    matrix[row][col] = 1; // 标记该位置为1
                }
            }
        }

        resultBitsFlipped[row] = 1 - resultBit;
    }

    return make_pair(matrix, resultBitsFlipped);
}

vector<vector<int>> verifySolutions(const vector<vector<int>> &A, const vector<int> &b, const vector<vector<int>> &solutions)
{
    vector<vector<int>> correct_solutions;

    for (const auto &solution : solutions)
    {
        bool is_correct = true;

        for (int i = 0; i < A.size(); ++i)
        {
            int sum = 0;
            for (int j = 0; j < A[i].size(); ++j)
            {
                sum = (sum + A[i][j] * solution[j]) % 2;
            }
            if (sum != b[i])
            {
                is_correct = false;
                break;
            }
        }

        if (is_correct)
        {
            correct_solutions.push_back(solution);
        }
    }

    return correct_solutions;
}
bool checkMajPart(const vector<int> &guess_indices, const vector<int> &permuted_indices)
{
    for (int idx : guess_indices)
    {
        if (find(permuted_indices.end() - 64, permuted_indices.end(), idx) == permuted_indices.end())
        {
            return false;
        }
    }
    return true;
}
//
vector<vector<int>> gaussianEliminationMod2WithFreeVariables(vector<vector<int>> &A, vector<int> &b)
{
    int rows = A.size();
    int cols = A[0].size();
    vector<int> x(cols, 0);        // 结果向量
    vector<int> free_variables;    // 自由变量存储
    vector<int> leading_variables; // 主元存储

    // 前向消元
    for (int i = 0; i < min(rows, cols); ++i)
    {
        if (A[i][i] == 0)
        {
            bool found = false;
            for (int j = i + 1; j < rows; ++j)
            {
                if (A[j][i] == 1)
                {
                    swap(A[i], A[j]); // 交换行
                    swap(b[i], b[j]);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                // 如果没有找到主元，说明这是一个自由变量
                free_variables.push_back(i);
                continue;
            }
        }

        leading_variables.push_back(i); // 记录有主元的变量

        // 消去下面的行
        for (int j = i + 1; j < rows; ++j)
        {
            if (A[j][i] == 1)
            {
                // 模2加法 (XOR)
                for (int k = 0; k < cols; ++k)
                {
                    A[j][k] = (A[j][k] + A[i][k]) % 2;
                }
                b[j] = (b[j] + b[i]) % 2;
            }
        }
    }

    // 回代求解特解
    for (int i = leading_variables.size() - 1; i >= 0; --i)
    {
        int var = leading_variables[i];
        int sum = b[var];
        for (int j = var + 1; j < cols; ++j)
        {
            sum = (sum + A[var][j] * x[j]) % 2;
        }
        x[var] = sum;
    }

    // 如果没有自由变量，说明解是唯一的
    if (free_variables.empty())
    {
        return {x};
    }

    // 生成所有可能的解
    vector<vector<int>> all_solutions;
    int num_free_vars = free_variables.size();

    // 遍历所有自由变量的可能取值组合
    for (int combination = 0; combination < (1 << num_free_vars); ++combination)
    {
        vector<int> current_solution = x;

        // 使用二进制表示生成自由变量的组合
        for (int i = 0; i < num_free_vars; ++i)
        {
            int var = free_variables[i];
            current_solution[var] = (combination >> i) & 1;
        }

        all_solutions.push_back(current_solution);
    }

    return all_solutions;
}
std::vector<int> selectRandomIndices(int t)
{
    // 确保 t 不大于 256
    if (t > 256)
    {
        throw std::invalid_argument("t must be less than or equal to 256");
    }

    // 初始化从 0 到 255 的数字
    std::vector<int> all_indices(256);
    for (int i = 0; i < 256; ++i)
    {
        all_indices[i] = i;
    }

    // 使用随机数生成器打乱 all_indices
    std::shuffle(all_indices.begin(), all_indices.end(), generator);

    // 选择前 t 个元素
    std::vector<int> t_indices(all_indices.begin(), all_indices.begin() + t);

    return t_indices;
}

// 计算一个字节中 1 的数量，使用 __builtin_popcount
inline int hammingWeightByte(uint8_t byte)
{
    return __builtin_popcount(byte);
}

// 计算 32 字节数组的汉明重量
int hammingWeight(const uint8_t *a, const uint8_t *b, size_t size = 32)
{
    int weight = 0;
    for (size_t i = 0; i < size; ++i)
    {
        weight += hammingWeightByte(a[i] ^ b[i]); // 逐字节异或并计算汉明重量
    }
    return weight;
}

// 将一行的结果写入文件
void writeResultToFile(std::ofstream &outFile, uint8_t result_bit, const uint8_t *xor_vector, const uint8_t *maj_vector, size_t size = 32)
{
    outFile << static_cast<int>(result_bit) << " ";

    // 写入 XOR 索引
    for (size_t i = 0; i < size * 8; ++i)
    {
        if (xor_vector[i / 8] & (1 << (i % 8)))
        {
            outFile << i << " ";
        }
    }

    // 写入 MAJ 索引
    for (size_t i = 0; i < size * 8; ++i)
    {
        if (maj_vector[i / 8] & (1 << (i % 8)))
        {
            outFile << i << " ";
        }
    }

    outFile << "\n";
}

// 使用 Windows 内存映射处理单个文件
void processSingleFileWithMemoryMapping(const std::string &inputFilePath, const std::vector<int> &t_indices, std::ofstream &outFile)
{
    // 将 std::string 转换为 std::wstring
    std::wstring wideInputFilePath = std::wstring(inputFilePath.begin(), inputFilePath.end());

    // 打开输入文件（使用宽字符版本的 CreateFileW）
    HANDLE hFile = CreateFileW(wideInputFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "无法打开输入文件！" << std::endl;
        return;
    }

    // 创建文件映射
    HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapFile == NULL)
    {
        std::cerr << "文件映射失败！" << std::endl;
        CloseHandle(hFile);
        return;
    }

    // 映射文件到内存
    uint8_t *fileData = (uint8_t *)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (fileData == NULL)
    {
        std::cerr << "文件映射视图失败！" << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return;
    }

    // 构建 t_indices 对应的 32 字节向量（256 位）
    uint8_t t_indices_vector[32] = {0}; // 初始化为全 0
    for (int idx : t_indices)
    {
        t_indices_vector[idx / 8] |= (1 << (idx % 8)); // 在对应位置上设置 1
    }

    // 读取文件的每一行数据
    size_t offset = 0;
    size_t fileSize = GetFileSize(hFile, NULL);
    while (offset < fileSize)
    {
        // 读取 result_bit
        uint8_t result_bit = fileData[offset];
        offset += 1;

        // 读取 XOR 和 MAJ 向量
        const uint8_t *xor_vector = fileData + offset;
        offset += 32;
        const uint8_t *maj_vector = fileData + offset;
        offset += 32;

        // 计算与 t_indices_vector 的异或结果的汉明重量
        int weight = hammingWeight(maj_vector, t_indices_vector);

        // 如果汉明重量是 53，存储结果
        if (weight == 53)
        {
            writeResultToFile(outFile, result_bit, xor_vector, maj_vector);
        }
    }

    // 清理和关闭文件
    UnmapViewOfFile(fileData);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
}

// 主函数：处理多个文件
void processFilesWithMemoryMapping(const std::string &folderPath, const std::vector<int> &t_indices, const std::string &outputFilePath, int num_files)
{
    // 打开输出文件
    std::ofstream outFile(outputFilePath);
    if (!outFile.is_open())
    {
        std::cerr << "无法打开输出文件！" << std::endl;
        return;
    }

    // 写入第一行 t_indices
    outFile << "t_indices: ";
    for (int idx : t_indices)
    {
        outFile << idx << " ";
    }
    outFile << "\n";

    // 处理指定数量的小文件
    for (int i = 1; i <= num_files; ++i)
    {
        // 构建当前文件的路径
        std::string inputFilePath = folderPath + "/output_" + std::to_string(i) + ".bin";
        std::cout << "正在处理文件: " << inputFilePath << std::endl;

        // 处理当前文件
        processSingleFileWithMemoryMapping(inputFilePath, t_indices, outFile);
    }

    outFile.close();
}
int main()
{
    int a = 10;
    int b = 64;
    vector<uint8_t> key = {
        0x8b, 0x0b, 0xac, 0x2a, 0xcc, 0x71, 0x7b, 0xb4, 0x66, 0xea, 0xab, 0xed, 0x78, 0xe2, 0xfb, 0xdb, 0x3a, 0xd9, 0x89, 0x60, 0xde, 0xbd, 0xc5, 0x53, 0x2b, 0x67, 0xec, 0x5a, 0xa6, 0x65, 0x54, 0x6f};

    PRNG prng(key, 256, 74);
    string key_bitstring = bytesToBitstring(key);
    int num_1 = 0;
    clock_t start_time = clock();

    while (true)
    {
        //vector<int> t_indices = selectRandomIndices(11);
         vector<int> t_indices = {87, 110, 86, 139, 205, 127, 30, 196, 6, 55, 202};

        int success_count = 0;
        vector<pair<int, vector<int>>> correct_instances;
        vector<pair<int, vector<int>>> all_instances;
        int num_instances = 257;
        processFilesWithMemoryMapping("data", t_indices, "filtered_output.txt", 125); // 假设读取 10 个文件

        ifstream file("filtered_output.txt");
        if (!file.is_open())
        {
            cerr << "无法打开文件filtered_output.txt" << endl;
            return 1;
        }

        string line;
        getline(file, line);
        cout << "读取到的 t 位索引: " << line << endl;

        while (getline(file, line))
        {
            istringstream iss(line);
            vector<int> parts((istream_iterator<int>(iss)), istream_iterator<int>());

            if (parts.empty())
                continue;

            int result_bit = parts[0];
            vector<int> permuted_indices(parts.begin() + 1, parts.end());

            // 计算寄存器中的比特
            vector<int> register_bits;
            for (int idx : permuted_indices)
            {
                int byte_idx = idx / 8;
                int bit_idx = idx % 8;
                uint8_t byte = prng.initRegister()[byte_idx];
                int bit = (byte >> (7 - bit_idx)) & 1;
                register_bits.push_back(bit);
            }

            // XOR 部分
            vector<int> xor_bits(register_bits.begin(), register_bits.begin() + a);
            vector<int> maj_bits(register_bits.begin() + a, register_bits.begin() + a + b);

            int xor_result = 0;
            for (int bit : xor_bits)
            {
                xor_result ^= bit;
            }

            int maj_result = (count(maj_bits.begin(), maj_bits.end(), 1) >= maj_bits.size() / 2) ? 1 : 0;
            all_instances.emplace_back(result_bit, permuted_indices);

            if (maj_result == 1)
            {
                success_count++;
                correct_instances.emplace_back(result_bit, permuted_indices);
            }
        }
        file.close();

        int num = 0;
        cout << "Number of instances in correct_instances: " << correct_instances.size() << endl;

        while (num < 1000)
        {
            // 生成矩阵和结果向量
            auto [matrix, results_bits] = generateMatrixFromInstances(all_instances, t_indices, 256);
            

            vector<vector<int>> solutions = gaussianEliminationMod2WithFreeVariables(matrix, results_bits);
            // cout << "Number of solutions: " << solutions.size() << endl;
            // 反转解中的每个元素

            // 首先验证解的正确性，将所有有效解保存下来
            vector<vector<int>> valid_solutions = verifySolutions(matrix, results_bits, solutions);
            for (int i = 0; i < valid_solutions.size(); ++i)
            {
                for (int j = 0; j < solutions[i].size(); ++j)
                {
                    valid_solutions[i][j] = 1 - solutions[i][j]; // 将1变成0，0变成1
                }
            }
           

            // 遍历有效解并进行密钥比较
            for (const auto &valid_sol : valid_solutions)
            {
                string guess_key = recover_key(valid_sol, t_indices);
                // std::cout << "The guess key is: " << guess_key << std::endl;
                // std::cout << "The  key is: " << key_bitstring << std::endl;
                if (guess_key == key_bitstring)
                {
                    cout << "The recovered key matches the original key." << endl;
                    cout << "尝试了 " << num << " 次" << endl;

                    clock_t end_time = clock();
                    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
                    cout << "Elapsed time: " << elapsed_time << " seconds" << endl;
                    return 0;
                }
              
            }

            num++;
        }

        num_1++;
        cout << "尝试了 " << num_1 << " 个group" << endl;
    }

    clock_t end_time = clock();
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    cout << "Elapsed time: " << elapsed_time << " seconds" << endl;
    cout << "失败" << endl;

    return 0;
}
