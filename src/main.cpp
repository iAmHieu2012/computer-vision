#include "imgUtils.hpp"

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " -command <input_image> <output_image> [<arg>]" << std::endl;
        return -1;
    }

    std::string command = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];

    if (command == "-rgb2gray" && argc == 4)
    {
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.rgb2gray();

        proc.showImage("Grayscale Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Converted to grayscale." << std::endl;
    }
    else if (command == "-brightness" && argc == 5)
    {
        int c = std::stoi(argv[4]);
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.changeBrightness(c);

        proc.showImage("Brightness Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Done! Brightness adjusted by " << c << std::endl;
    }
    else if (command == "-contrast" && argc == 5)
    {
        float c = std::stof(argv[4]);
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.changeContrast(c);

        proc.showImage("Contrast Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Done! Contrast adjusted by " << c << std::endl;
    }
    else if (command == "-avg" && argc == 5)
    {
        int k = std::stoi(argv[4]);
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.averageFilter(k);

        proc.showImage("Average Filter Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Average filter applied (k=" << k << ")." << std::endl;
    }
    else if (command == "-med" && argc == 5)
    {
        int k = std::stoi(argv[4]);
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.medianFilter(k);

        proc.showImage("Median Filter Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Median filter applied (k=" << k << ")." << std::endl;
    }
    else if (command == "-gau" && argc == 5)
    {
        int k = std::stoi(argv[4]);
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.gaussianFilter(k);

        proc.showImage("Gaussian Filter Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Gaussian filter applied (k=" << k << ")." << std::endl;
    }
    else if (command == "-sobel" && argc == 4)
    {
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.sobelEdge();

        proc.showImage("Sobel Edge Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Sobel edge detection applied." << std::endl;
    }
    else if (command == "-laplace" && argc == 4)
    {
        imageProcessor proc;
        if (!proc.readImage(input))
            return -1;
        proc.laplaceEdge();

        proc.showImage("Laplace Edge Result");
        if (!proc.saveImage(output))
            return -1;

        std::cout << "Laplace edge detection applied." << std::endl;
    }
    else
    {
        std::cout << "Unknown command or invalid arguments: " << command << std::endl;
    }

    return 0;
}