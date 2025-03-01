#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <cmath>

int main(int argc, char** argv) 
{
    const size_t BYTES_IN_GB = 1e9;
    if (argc < 2) 
    {
        std::cerr << "Usage: file_generator <output file path>";
        return -1;
    }
    std::string targetPath = argv[1];
    std::random_device device;
    std::mt19937_64 gen(device());
    std::uniform_real_distribution<double> mantissaDistribution(-1.0, 1.0);
    std::uniform_int_distribution<int> exponentDistribution(-300, 300);
    std::ofstream output(targetPath);
    if (!output)
    {
        std::cerr << "Unable to save a file at the target path: " << targetPath << '\n';
        return -3;
    }

    // Since there are no explicit constrains
    // I decided to restrict precision to match with example in a test task description (5 digits after a point)
    output << std::scientific << std::setprecision(5);
    for (long bytesPut = 0; bytesPut < BYTES_IN_GB; )
    {
        double mantissa = mantissaDistribution(gen);
        int exponent = exponentDistribution(gen);
        double num = mantissa * pow(10, exponent); //std::scalbn(mantissa, exponent);
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(5) << num << '\n';
        std::string numAsString = oss.str();
        output << numAsString;
        bytesPut += numAsString.size();
    }
    return 0;
}