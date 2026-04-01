#pragma once
#include "libs.hpp"

class imageProcessor
{
private:
    cv::Mat img;
    cv::Mat createZeroPaddedImage(const cv::Mat &src, int pad);
    cv::Mat createReplicatePaddedImage(const cv::Mat &src, int pad);
    cv::Mat applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel);
    void takeAbs(cv::Mat &src);
    cv::Mat clamp8U(const cv::Mat &src);

public:
    imageProcessor();
    ~imageProcessor();

    bool readImage(const std::string &filepath);
    void showImage(const std::string &windowName);
    bool saveImage(const std::string &filepath);

    void rgb2gray();
    void changeBrightness(int c);
    void changeContrast(float c);

    void averageFilter(int k);
    void medianFilter(int k);
    void gaussianFilter(int k);

    void sobelEdge();
    void laplaceEdge();
};