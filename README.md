Thi# Bit-fixing Correlation Attack on Random Local Functions

This is a simple implementation of the Bit-fixing Correlation Attack on Random Local Functions based on C++.

## Features and Future Improvements
Due to time constraints, the following optimization techniques have not yet been implemented:
- **Parallel Computing**: This will help in speeding up the computation process.
- **Sparse Matrix Gaussian Elimination**: To efficiently solve large, sparse systems.

These features will be added in the future.

## Key Information
The key used in this implementation is derived from hashing the value **"EUROCRYPTO2025"** using the standard **SHA-256** function.

## Usage Instructions
1. **Generate Outputs**:
   Use `output.cpp` to generate a large number of outputs for the attack.

   ```sh
   g++ output.cpp -o output_generator
   ./output_generator
