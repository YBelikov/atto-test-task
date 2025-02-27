#include <iostream>
#include <fstream>
#include <random>

int main(int argc, char** argv) 
{
    const size_t BYTES_IN_GB = 1e9;

    if (argc < 4) 
    {
        std::cerr << "Usage: <size lower bound> <size upper bound> <output file path>";
        return -1;
    }
    double lowerBound = std::stod(argv[1]);
    double upperBound = std::stod(argv[2]);
    std::string targetPath = argv[3];
    if (lowerBound >= upperBound)
    {
        std::cerr << "Your lower bound should be less than upper bound!!!";
        return -2;
    }
    std::random_device device;
    std::mt19937_64 gen(device());
    std::uniform_real_distribution<double> distribution(lowerBound, upperBound);
    std::ofstream output(targetPath);
    if (!output)
    {
        std::cerr << "Unable to save a file at the target path: " << targetPath << '\n';
        return -3;
    }
    // MARK: well there should some constraints...
    std::setprecision(17);
    for (long bytesPut = 0; bytesPut < BYTES_IN_GB; )
    {
        double num = distribution(gen);
        std::string numAsString = std::to_string(num) + "\n";
        output << numAsString;
        bytesPut += numAsString.size();
    }
    return 0;
}