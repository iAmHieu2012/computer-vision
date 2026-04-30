#include "imgUtils.hpp"

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " <command> <input_image> <output_image> [<arg>]" << std::endl;
        std::cout << "Commands list:" << std::endl;
        std::cout << "  -rgb2gray, -sobel, -laplace, -harris, -optHarris" << std::endl;
        std::cout << "  -brightness <val>, -contrast <val>" << std::endl;
        std::cout << "  -avg <k>, -med <k>, -gau <k>" << std::endl;
        return -1;
    }

    std::string command = argv[1];
    std::string inputFilePath = argv[2];
    std::string outputFilePath = argv[3];

    imageProcessor proc;
    cv::Mat raw = proc.readImage(inputFilePath);
    if (raw.empty()) return -1;
    
    cv::Mat dest; 
    std::string windowTitle = "Result Window";
    
    auto start = std::chrono::high_resolution_clock::now();

    if (command == "-rgb2gray" && argc == 4) {
        proc.rgb2gray(raw, dest);
        windowTitle = "Grayscale Result";
    }
    else if (command == "-brightness" && argc == 5) {
        proc.changeBrightness(raw, dest, std::stoi(argv[4]));
        windowTitle = "Brightness Adjusted";
    }
    else if (command == "-contrast" && argc == 5) {
        proc.changeContrast(raw, dest, std::stof(argv[4]));
        windowTitle = "Contrast Adjusted";
    }
    else if (command == "-avg" && argc == 5) {
        proc.averageFilter(raw, dest, std::stoi(argv[4]));
        windowTitle = "Average Filter";
    }
    else if (command == "-med" && argc == 5) {
        proc.medianFilter(raw, dest, std::stoi(argv[4]));
        windowTitle = "Median Filter";
    }
    else if (command == "-gau" && argc == 5) {
        proc.gaussianFilter(raw, dest, std::stoi(argv[4]));
        windowTitle = "Gaussian Filter";
    }
    else if (command == "-sobel" && argc == 4) {
        proc.sobelEdge(raw, dest);
        windowTitle = "Sobel Edge Detection";
    }
    else if (command == "-laplace" && argc == 4) {
        proc.laplaceEdge(raw, dest);
        windowTitle = "Laplace Edge Detection";
    }
    else if (command == "-harris" && argc == 4) {
        proc.harris(raw, dest);
        windowTitle = "Basic Harris Corner Detection";
    }
    else if (command == "-optHarris" && argc == 4) {
        proc.optHarris(raw, dest);
        windowTitle = "Optimized Harris Corner Detection";
    }
    else {
        std::cout << "Error: Unknown command or invalid arguments for " << command << std::endl;
        return -1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time_taken = end - start;
    std::cout << "[" << windowTitle << "] Execution time: " << time_taken.count() << " ms" << std::endl;

    proc.showImage(windowTitle, dest);
    proc.saveImage(outputFilePath, dest);

    return 0;
}