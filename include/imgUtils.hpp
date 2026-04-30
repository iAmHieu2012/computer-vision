#pragma once
#include "libs.hpp"

class imageProcessor
{
private:
    cv::Mat cloneImage(const cv::Mat &src);
    cv::Mat createZeroPaddedImage(const cv::Mat &src, int pad);
    cv::Mat createReplicatePaddedImage(const cv::Mat &src, int pad);
    cv::Mat applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel);
    void takeAbs(cv::Mat &src);
    cv::Mat clamp8U(const cv::Mat &src);
    
    void applyGaussianFloat(const float* src, float* dst, int width, int height, int k);
    void applyGaussianSeparableFloat(const float* src, float* dst, int width, int height, int k);

public:
    imageProcessor();
    ~imageProcessor();

    /* Utilities I/O */
    cv::Mat readImage(const std::string &filepath);
    void showImage(const std::string &windowName, const cv::Mat &img);
    bool saveImage(const std::string &filepath, const cv::Mat &img);

    /* Basic Image Processing */
    void rgb2gray(const cv::Mat &src, cv::Mat &dst);
    void changeBrightness(const cv::Mat &src, cv::Mat &dst, int c);
    void changeContrast(const cv::Mat &src, cv::Mat &dst, float c);
    void averageFilter(const cv::Mat &src, cv::Mat &dst, int k);
    void medianFilter(const cv::Mat &src, cv::Mat &dst, int k);
    void gaussianFilter(const cv::Mat &src, cv::Mat &dst, int k);
    
    /* Edge Detection */
    void sobelEdge(const cv::Mat &src, cv::Mat &dst);
    void laplaceEdge(const cv::Mat &src, cv::Mat &dst);

    /* Feature Detection (Harris Corners) */
    void harris(const cv::Mat &src, cv::Mat &dst);
    void optHarris(const cv::Mat &src, cv::Mat &dst);
};