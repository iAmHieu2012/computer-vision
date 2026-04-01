#include "imgUtils.hpp"

imageProcessor::imageProcessor() {}
imageProcessor::~imageProcessor() {}

bool imageProcessor::readImage(const std::string &filepath)
{
    img = cv::imread(filepath, cv::IMREAD_COLOR);
    if (img.empty())
    {
        std::cerr << "Error: Cannot read image from: " << filepath << std::endl;
        return false;
    }
    return true;
}

void imageProcessor::showImage(const std::string &windowName)
{
    if (img.empty())
    {
        std::cerr << "Error: No image to show!" << std::endl;
        return;
    }
    cv::imshow(windowName, img);
    cv::waitKey(0);
}

bool imageProcessor::saveImage(const std::string &filepath)
{
    if (img.empty())
    {
        std::cerr << "Error: Empty image" << std::endl;
        return false;
    }
    bool p = cv::imwrite(filepath, img);
    if (!p)
        std::cerr << "Error: Cannot save image" << std::endl;
    return p;
}

void imageProcessor::rgb2gray()
{
    if (img.empty() || img.channels() == 1)
        return;

    int width = img.cols, height = img.rows;
    int nChannels = img.channels();
    cv::Mat res(height, width, CV_8UC1);

    for (int y = 0; y < height; y++)
    {
        uchar *pRowSrc = img.ptr<uchar>(y);
        uchar *pRowDst = res.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pRowSrc += nChannels, pRowDst++)
        {
            *pRowDst = (uchar)(0.11 * pRowSrc[0] + 0.59 * pRowSrc[1] + 0.3 * pRowSrc[2]);
        }
    }
    img = res;
}

void imageProcessor::changeBrightness(int c)
{
    if (img.empty())
        return;

    int width = img.cols, height = img.rows;
    int nChannels = img.channels();

    for (int y = 0; y < height; y++)
    {
        uchar *pRow = img.ptr<uchar>(y);
        for (int x = 0; x < width * nChannels; x++)
        {
            int val = pRow[x] + c;
            if (val > 255)
                val = 255;
            else if (val < 0)
                val = 0;
            pRow[x] = (uchar)val;
        }
    }
}

void imageProcessor::changeContrast(float c)
{
    if (img.empty())
        return;

    int width = img.cols, height = img.rows;
    int nChannels = img.channels();

    for (int y = 0; y < height; y++)
    {
        uchar *pRow = img.ptr<uchar>(y);
        for (int x = 0; x < width * nChannels; x++)
        {
            float val = (float)pRow[x] * c;
            if (val > 255.0f)
                val = 255.0f;
            else if (val < 0.0f)
                val = 0.0f;
            pRow[x] = (uchar)val;
        }
    }
}

cv::Mat imageProcessor::createZeroPaddedImage(const cv::Mat &src, int pad)
{
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();

    cv::Mat padded(height + 2 * pad, width + 2 * pad, src.type(), cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        const uchar *pSrc = src.ptr<uchar>(y);
        uchar *pPad = padded.ptr<uchar>(y + pad);
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < ch; c++)
            {
                pPad[(x + pad) * ch + c] = pSrc[x * ch + c];
            }
        }
    }
    return padded;
}

cv::Mat imageProcessor::createReplicatePaddedImage(const cv::Mat &src, int pad)
{
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();

    cv::Mat padded(height + 2 * pad, width + 2 * pad, src.type());

    for (int y = 0; y < padded.rows; y++)
    {
        int src_y = y - pad;
        if (src_y < 0) src_y = 0;
        else if (src_y >= height) src_y = height - 1;

        const uchar *pSrc = src.ptr<uchar>(src_y);
        uchar *pPad = padded.ptr<uchar>(y);

        for (int x = 0; x < padded.cols; x++)
        {
            int src_x = x - pad;
            if (src_x < 0) src_x = 0;
            else if (src_x >= width) src_x = width - 1;

            for (int c = 0; c < ch; c++)
            {
                pPad[x * ch + c] = pSrc[src_x * ch + c];
            }
        }
    }
    return padded;
}

cv::Mat imageProcessor::applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel)
{
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();
    int k = kernel.size();
    int pad = k / 2;

    cv::Mat paddedSrc = createZeroPaddedImage(src, pad);

    cv::Mat dst(height, width, CV_32FC(ch), cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        float *pDst = dst.ptr<float>(y);
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < ch; c++)
            {
                float sum = 0.0f;

                for (int ky = -pad; ky <= pad; ky++)
                {
                    const uchar *pSrc_pad = paddedSrc.ptr<uchar>(y + pad + ky);
                    for (int kx = -pad; kx <= pad; kx++)
                    {
                        sum += pSrc_pad[(x + pad + kx) * ch + c] * kernel[-ky + pad][-kx + pad];
                    }
                }

                pDst[x * ch + c] = sum;
            }
        }
    }
    return dst;
}

void imageProcessor::takeAbs(cv::Mat &src)
{
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();

    for (int y = 0; y < height; y++)
    {
        float *p = src.ptr<float>(y);
        for (int x = 0; x < width * ch; x++)
        {
            p[x] = std::abs(p[x]);
        }
    }
}

cv::Mat imageProcessor::clamp8U(const cv::Mat &src)
{
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();

    cv::Mat dst(height, width, CV_8UC(ch), cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        const float *pSrc = src.ptr<float>(y);
        uchar *pDst = dst.ptr<uchar>(y);
        for (int x = 0; x < width * ch; x++)
        {
            int val = (int)std::round(pSrc[x]);
            if (val > 255)
                val = 255;
            else if (val < 0)
                val = 0;
            pDst[x] = (uchar)val;
        }
    }
    return dst;
}

void imageProcessor::averageFilter(int k)
{
    if (img.empty() || k <= 1 || k % 2 == 0)
    {
        std::cerr << "Error: Kernel size 'k' must be an odd number > 1." << std::endl;
        return;
    }

    std::vector<std::vector<float>> kernel(k, std::vector<float>(k, 1.0f / (k * k)));
    cv::Mat rawDst = applyConvolution(img, kernel);
    img = clamp8U(rawDst);
}

void imageProcessor::medianFilter(int k)
{
    if (img.empty() || k <= 1 || k % 2 == 0)
    {
        std::cerr << "Error: Kernel size must be an odd number > 1." << std::endl;
        return;
    }

    int height = img.rows;
    int width = img.cols;
    int ch = img.channels();
    int pad = k / 2;

    cv::Mat paddedSrc = createZeroPaddedImage(img, pad);

    cv::Mat dst(height, width, img.type(), cv::Scalar(0));
    std::vector<uchar> window(k * k);

    for (int y = 0; y < height; y++)
    {
        uchar *pDst = dst.ptr<uchar>(y);
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < ch; c++)
            {
                int count = 0;

                for (int ky = -pad; ky <= pad; ky++)
                {
                    const uchar *pSrc_pad = paddedSrc.ptr<uchar>(y + pad + ky);
                    for (int kx = -pad; kx <= pad; kx++)
                    {
                        window[count++] = pSrc_pad[(x + pad + kx) * ch + c];
                    }
                }

                std::sort(window.begin(), window.end());
                pDst[x * ch + c] = window[(k * k) / 2];
            }
        }
    }
    img = dst;
}

void imageProcessor::gaussianFilter(int k)
{
    if (img.empty() || k <= 1 || k % 2 == 0)
    {
        std::cerr << "Error: Kernel size must be an odd number > 1." << std::endl;
        return;
    }

    std::vector<std::vector<float>> kernel(k, std::vector<float>(k));
    float sigma = 0.3 * ((k - 1) * 0.5 - 1) + 0.8;
    float sum = 0.0f;
    int pad = k / 2;
    const float PI = 3.1415926535f;

    for (int i = -pad; i <= pad; i++)
    {
        for (int j = -pad; j <= pad; j++)
        {
            kernel[i + pad][j + pad] = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            sum += kernel[i + pad][j + pad];
        }
    }
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++)
            kernel[i][j] /= sum;

    cv::Mat rawDst = applyConvolution(img, kernel);
    img = clamp8U(rawDst);
}

void imageProcessor::sobelEdge()
{
    if (img.empty())
        return;
    if (img.channels() != 1)
        this->rgb2gray();

    std::vector<std::vector<float>> Kx = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    std::vector<std::vector<float>> Ky = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    cv::Mat gradX = applyConvolution(img, Kx);
    cv::Mat gradY = applyConvolution(img, Ky);

    cv::Mat magnitude(img.rows, img.cols, CV_32FC1, cv::Scalar(0));
    for (int y = 0; y < img.rows; y++)
    {
        float *pMag = magnitude.ptr<float>(y);
        float *pX = gradX.ptr<float>(y);
        float *pY = gradY.ptr<float>(y);

        for (int x = 0; x < img.cols; x++)
        {
            pMag[x] = std::sqrt(pX[x] * pX[x] + pY[x] * pY[x]);
        }
    }

    img = clamp8U(magnitude);
}

void imageProcessor::laplaceEdge()
{
    if (img.empty())
        return;
    if (img.channels() != 1)
        this->rgb2gray();

    std::vector<std::vector<float>> K = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}};

    cv::Mat rawDst = applyConvolution(img, K);

    takeAbs(rawDst);

    img = clamp8U(rawDst);
}