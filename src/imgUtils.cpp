#include "imgUtils.hpp"

imageProcessor::imageProcessor() {}
imageProcessor::~imageProcessor() {}

cv::Mat imageProcessor::readImage(const std::string &filepath)
{
    cv::Mat img = cv::imread(filepath, cv::IMREAD_COLOR);
    if (img.empty())
        std::cerr << "Error: Cannot read image from: " << filepath << std::endl;
    return img;
}

void imageProcessor::showImage(const std::string &windowName, const cv::Mat &img)
{
    if (img.empty()) return;
    cv::imshow(windowName, img);
    cv::waitKey(0);
}

bool imageProcessor::saveImage(const std::string &filepath, const cv::Mat &img)
{
    if (img.empty()) return false;
    bool p = cv::imwrite(filepath, img);
    if (!p) std::cerr << "Error: Cannot save image to " << filepath << std::endl;
    return p;
}

cv::Mat imageProcessor::cloneImage(const cv::Mat &src)
{
    if (src.empty()) return cv::Mat();

    int h = src.rows, w = src.cols, ch = src.channels();
    cv::Mat dst(h, w, src.type());

    uchar* s_data = src.data;
    uchar* d_data = dst.data;

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            for (int c = 0; c < ch; c++)
                *(d_data + y * dst.step + x * ch + c) = *(s_data + y * src.step + x * ch + c);

    return dst;
}

cv::Mat imageProcessor::createZeroPaddedImage(const cv::Mat &src, int pad)
{
    int height = src.rows, width = src.cols, ch = src.channels();
    cv::Mat padded(height + 2 * pad, width + 2 * pad, src.type(), cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        const uchar *pSrc = src.ptr<uchar>(y);
        uchar *pPad = padded.ptr<uchar>(y + pad);
        for (int x = 0; x < width; x++)
            for (int c = 0; c < ch; c++)
                pPad[(x + pad) * ch + c] = pSrc[x * ch + c];
    }
    return padded;
}

cv::Mat imageProcessor::createReplicatePaddedImage(const cv::Mat &src, int pad)
{
    int height = src.rows, width = src.cols, ch = src.channels();
    int pad_height = height + 2 * pad, pad_width = width + 2 * pad;
    cv::Mat padded(pad_height, pad_width, src.type());

    uchar* src_data = src.data;
    uchar* pad_data = padded.data;

    for (int y = 0; y < pad_height; y++)
    {
        int src_y = std::max(0, std::min(y - pad, height - 1));
        for (int x = 0; x < pad_width; x++)
        {
            int src_x = std::max(0, std::min(x - pad, width - 1));
            for (int c = 0; c < ch; c++)
            {
                *(pad_data + y * padded.step + x * ch + c) = 
                    *(src_data + src_y * src.step + src_x * ch + c);
            }
        }
    }
    return padded;
}

cv::Mat imageProcessor::applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel)
{
    int height = src.rows, width = src.cols, ch = src.channels();
    int k = kernel.size();
    int pad = k / 2;
    
    cv::Mat paddedSrc = createReplicatePaddedImage(src, pad);
    cv::Mat result(height, width, CV_32FC(ch), cv::Scalar(0));

    uchar* pad_data = paddedSrc.data;
    float* result_data = (float*)result.data;
    int result_step = result.step / sizeof(float);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < ch; c++)
            {
                float sum = 0.0f;
                for (int ky = -pad; ky <= pad; ky++)
                {
                    for (int kx = -pad; kx <= pad; kx++)
                    {
                        int py = y + pad + ky;
                        int px = x + pad + kx;
                        uchar pixel = *(pad_data + py * paddedSrc.step + px * ch + c);
                        sum += pixel * kernel[-ky + pad][-kx + pad];
                    }
                }
                *(result_data + y * result_step + x * ch + c) = sum;
            }
        }
    }
    return result;
}

void imageProcessor::takeAbs(cv::Mat &src)
{
    int height = src.rows, width = src.cols, ch = src.channels();
    for (int y = 0; y < height; y++)
    {
        float *p = src.ptr<float>(y);
        for (int x = 0; x < width * ch; x++)
            p[x] = std::abs(p[x]);
    }
}

cv::Mat imageProcessor::clamp8U(const cv::Mat &src)
{
    int height = src.rows, width = src.cols, ch = src.channels();
    cv::Mat dst(height, width, CV_8UC(ch), cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        const float *pSrc = src.ptr<float>(y);
        uchar *pDst = dst.ptr<uchar>(y);
        for (int x = 0; x < width * ch; x++)
        {
            int val = (int)std::round(pSrc[x]);
            pDst[x] = (uchar)std::max(0, std::min(val, 255));
        }
    }
    return dst;
}

void imageProcessor::applyGaussianFloat(const float* src, float* dst, int width, int height, int k)
{
    int pad = k / 2;
    std::vector<std::vector<float>> G(k, std::vector<float>(k));
    float sigma = 0.3f * ((k - 1) * 0.5f - 1.0f) + 0.8f;
    float sum_G = 0.0f;
    const float PI = 3.1415926535f;

    for (int i = -pad; i <= pad; i++) {
        for (int j = -pad; j <= pad; j++) {
            G[i + pad][j + pad] = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            sum_G += G[i + pad][j + pad];
        }
    }
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++) G[i][j] /= sum_G;

    for (int y = pad; y < height - pad; y++) {
        for (int x = pad; x < width - pad; x++) {
            float sum = 0.0f;
            for (int wy = -pad; wy <= pad; wy++) {
                for (int wx = -pad; wx <= pad; wx++) {
                    int n_idx = (y + wy) * width + (x + wx);
                    sum += *(src + n_idx) * G[wy + pad][wx + pad];
                }
            }
            *(dst + y * width + x) = sum;
        }
    }
}

// 1D Separable Gaussian Filter (O(N^2 * 2K) instead of O(N^2 * K^2))
void imageProcessor::applyGaussianSeparableFloat(const float* src, float* dst, int width, int height, int k)
{
    int pad = k / 2;
    std::vector<float> G1D(k);
    float sigma = 0.3f * ((k - 1) * 0.5f - 1.0f) + 0.8f;
    float sum_G = 0.0f;
    const float PI = 3.1415926535f;

    for (int i = -pad; i <= pad; i++) {
        G1D[i + pad] = std::exp(-(i * i) / (2 * sigma * sigma)) / (std::sqrt(2 * PI) * sigma);
        sum_G += G1D[i + pad];
    }
    for (int i = 0; i < k; i++) G1D[i] /= sum_G;

    float* temp = new float[width * height]{0};

    // Horizontal pass
    for (int y = 0; y < height; y++) {
        for (int x = pad; x < width - pad; x++) {
            float sum = 0.0f;
            for (int wx = -pad; wx <= pad; wx++)
                sum += *(src + y * width + (x + wx)) * G1D[wx + pad];
            *(temp + y * width + x) = sum;
        }
    }

    // Vertical pass
    for (int y = pad; y < height - pad; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0.0f;
            for (int wy = -pad; wy <= pad; wy++)
                sum += *(temp + (y + wy) * width + x) * G1D[wy + pad];
            *(dst + y * width + x) = sum;
        }
    }
    delete[] temp;
}

void imageProcessor::rgb2gray(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty()) return;
    if (src.channels() == 1) { dst = cloneImage(src); return; }

    int width = src.cols, height = src.rows;
    dst = cv::Mat(height, width, CV_8UC1);

    uchar* src_data = src.data;
    uchar* dst_data = dst.data;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uchar b = *(src_data + y * src.step + x * src.channels() + 0);
            uchar g = *(src_data + y * src.step + x * src.channels() + 1);
            uchar r = *(src_data + y * src.step + x * src.channels() + 2);
            *(dst_data + y * dst.step + x) = (uchar)(0.114f * b + 0.587f * g + 0.299f * r);
        }
    }
}

void imageProcessor::changeBrightness(const cv::Mat &src, cv::Mat &dst, int c)
{
    if (src.empty()) return;
    dst = cloneImage(src);
    int width = dst.cols, height = dst.rows, nChannels = dst.channels();

    for (int y = 0; y < height; y++)
    {
        uchar *pRow = dst.ptr<uchar>(y);
        for (int x = 0; x < width * nChannels; x++)
            pRow[x] = (uchar)std::max(0, std::min((int)pRow[x] + c, 255));
    }
}

void imageProcessor::changeContrast(const cv::Mat &src, cv::Mat &dst, float c)
{
    if (src.empty()) return;
    dst = cloneImage(src);
    int width = dst.cols, height = dst.rows, nChannels = dst.channels();

    for (int y = 0; y < height; y++)
    {
        uchar *pRow = dst.ptr<uchar>(y);
        for (int x = 0; x < width * nChannels; x++)
            pRow[x] = (uchar)std::max(0.0f, std::min((float)pRow[x] * c, 255.0f));
    }
}

void imageProcessor::averageFilter(const cv::Mat &src, cv::Mat &dst, int k)
{
    if (src.empty() || k <= 1 || k % 2 == 0) return;
    std::vector<std::vector<float>> kernel(k, std::vector<float>(k, 1.0f / (k * k)));
    dst = clamp8U(applyConvolution(src, kernel));
}

void imageProcessor::medianFilter(const cv::Mat &src, cv::Mat &dst, int k)
{
    if (src.empty() || k <= 1 || k % 2 == 0) return;

    int height = src.rows, width = src.cols, ch = src.channels(), pad = k / 2;
    cv::Mat paddedSrc = createReplicatePaddedImage(src, pad);
    dst = cv::Mat(height, width, src.type(), cv::Scalar(0));
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
                        window[count++] = pSrc_pad[(x + pad + kx) * ch + c];
                }
                std::sort(window.begin(), window.end());
                pDst[x * ch + c] = window[(k * k) / 2];
            }
        }
    }
}

void imageProcessor::gaussianFilter(const cv::Mat &src, cv::Mat &dst, int k)
{
    if (src.empty() || k <= 1 || k % 2 == 0) return;

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

    dst = clamp8U(applyConvolution(src, kernel));
}

void imageProcessor::sobelEdge(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty()) return;
    
    cv::Mat gray;
    if (src.channels() != 1) this->rgb2gray(src, gray);
    else gray = src;

    std::vector<std::vector<float>> Kx = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    std::vector<std::vector<float>> Ky = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    cv::Mat gradX = applyConvolution(gray, Kx);
    cv::Mat gradY = applyConvolution(gray, Ky);

    cv::Mat magnitude(gray.rows, gray.cols, CV_32FC1, cv::Scalar(0));
    for (int y = 0; y < gray.rows; y++)
    {
        float *pMag = magnitude.ptr<float>(y);
        float *pX = gradX.ptr<float>(y);
        float *pY = gradY.ptr<float>(y);

        for (int x = 0; x < gray.cols; x++)
            pMag[x] = std::sqrt(pX[x] * pX[x] + pY[x] * pY[x]);
    }

    dst = clamp8U(magnitude);
}

void imageProcessor::laplaceEdge(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty()) return;
    
    cv::Mat gray;
    if (src.channels() != 1) this->rgb2gray(src, gray);
    else gray = src;

    std::vector<std::vector<float>> K = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}};

    cv::Mat rawDst = applyConvolution(gray, K);
    takeAbs(rawDst);
    dst = clamp8U(rawDst);
}

void imageProcessor::harris(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty()) return;

    dst = this->cloneImage(src);
    cv::Mat grayImg;
    this->rgb2gray(src, grayImg);

    // Compute Image Derivatives (Sobel)
    std::vector<std::vector<float>> Kx = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    std::vector<std::vector<float>> Ky = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    cv::Mat gradX = applyConvolution(grayImg, Kx);
    cv::Mat gradY = applyConvolution(grayImg, Ky);

    int width = src.cols, height = src.rows;
    float* gx_data = (float*)gradX.data;
    float* gy_data = (float*)gradY.data;
    int g_step = gradX.step / sizeof(float);

    // Compute components of the Structure Tensor (M)
    float* Ixx = new float[width * height]{0};
    float* Iyy = new float[width * height]{0};
    float* Ixy = new float[width * height]{0};

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float ix = *(gx_data + y * g_step + x);
            float iy = *(gy_data + y * g_step + x);
            
            int idx = y * width + x;
            *(Ixx + idx) = ix * ix;
            *(Iyy + idx) = iy * iy;
            *(Ixy + idx) = ix * iy;
        }
    }

    // Smooth the Structure Tensor components with Gaussian filter
    int offset = 2; 
    float* Sxx = new float[width * height]{0};
    float* Syy = new float[width * height]{0};
    float* Sxy = new float[width * height]{0};
    
    applyGaussianFloat(Ixx, Sxx, width, height, 5);
    applyGaussianFloat(Iyy, Syy, width, height, 5);
    applyGaussianFloat(Ixy, Sxy, width, height, 5);

    // Compute Harris Response (R = det(M) - k * trace(M)^2)
    float* R = new float[width * height]{0};
    float k = 0.04f, max_R = 0.0f;

    for (int y = offset; y < height - offset; y++)
    {
        for (int x = offset; x < width - offset; x++)
        {
            int idx = y * width + x;
            float det = *(Sxx + idx) * *(Syy + idx) - *(Sxy + idx) * *(Sxy + idx);
            float trace = *(Sxx + idx) + *(Syy + idx);
            float r_val = det - k * trace * trace;
            
            *(R + idx) = r_val;
            if (r_val > max_R) max_R = r_val;
        }
    }

    // Apply Non-Maximum Suppression (NMS) to isolate corners
    float threshold = 0.01f * max_R;

    for (int y = offset + 1; y < height - offset - 1; y++)
    {
        for (int x = offset + 1; x < width - offset - 1; x++)
        {
            float current_R = *(R + y * width + x);
            if (current_R > threshold)
            {
                bool isMax = true;
                for (int ny = -1; ny <= 1; ny++)
                {
                    for (int nx = -1; nx <= 1; nx++)
                    {
                        if (ny == 0 && nx == 0) continue;
                        if (*(R + (y + ny) * width + (x + nx)) > current_R)
                        {
                            isMax = false; 
                            break;
                        }
                    }
                    if (!isMax) break;
                }
                
                if (isMax) cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), 1);
            }
        }
    }

    delete[] Ixx; delete[] Iyy; delete[] Ixy;
    delete[] Sxx; delete[] Syy; delete[] Sxy; delete[] R;
}

void imageProcessor::optHarris(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty()) return;
    
    dst = this->cloneImage(src);
    int width = src.cols, height = src.rows, channels = src.channels(), step = src.step;
    uchar* data = src.data;

    uchar* gray = new uchar[width * height];
    float* Ixx = new float[width * height]{0};
    float* Iyy = new float[width * height]{0};
    float* Ixy = new float[width * height]{0};

    // Optimization 1: Integer-based Grayscale conversion
    for (int y = 0; y < height; y++)
    {
        int row_offset = y * step;
        int gray_row_offset = y * width;
        for (int x = 0; x < width; x++)
        {
            uchar b = *(data + row_offset + x * channels + 0);
            uchar g = *(data + row_offset + x * channels + 1);
            uchar r = *(data + row_offset + x * channels + 2);
            *(gray + gray_row_offset + x) = (uchar)((b * 114 + g * 587 + r * 299) / 1000);
        }
    }

    // Optimization 2: Fused loops for gradients and tensor components
    for (int y = 1; y < height - 1; y++)
    {
        int prev_y = (y - 1) * width, curr_y = y * width, next_y = (y + 1) * width;
        for (int x = 1; x < width - 1; x++)
        {
            float gx = -*(gray + prev_y + x - 1) + *(gray + prev_y + x + 1)
                       -2.0f * *(gray + curr_y + x - 1) + 2.0f * *(gray + curr_y + x + 1)
                       -*(gray + next_y + x - 1) + *(gray + next_y + x + 1);
            
            float gy = -*(gray + prev_y + x - 1) - 2.0f * *(gray + prev_y + x) - *(gray + prev_y + x + 1)
                       +*(gray + next_y + x - 1) + 2.0f * *(gray + next_y + x) + *(gray + next_y + x + 1);

            int idx = curr_y + x;
            *(Ixx + idx) = gx * gx;
            *(Iyy + idx) = gy * gy;
            *(Ixy + idx) = gx * gy;
        }
    }

    // Optimization 3: Separable 1D Gaussian for faster blurring
    int offset = 2;
    float* Sxx = new float[width * height]{0};
    float* Syy = new float[width * height]{0};
    float* Sxy = new float[width * height]{0};

    applyGaussianSeparableFloat(Ixx, Sxx, width, height, 5);
    applyGaussianSeparableFloat(Iyy, Syy, width, height, 5);
    applyGaussianSeparableFloat(Ixy, Sxy, width, height, 5);

    // Compute Harris Response
    float* R = new float[width * height]{0};
    float max_R = 0.0f, k = 0.04f;

    for (int y = offset; y < height - offset; y++)
    {
        for (int x = offset; x < width - offset; x++)
        {
            int idx = y * width + x;
            float det = *(Sxx + idx) * *(Syy + idx) - *(Sxy + idx) * *(Sxy + idx);
            float trace = *(Sxx + idx) + *(Syy + idx);
            
            float r_val = det - k * trace * trace;
            *(R + idx) = r_val;
            
            if (r_val > max_R) max_R = r_val;
        }
    }

    // Apply NMS filtering
    float threshold = 0.01f * max_R;
    
    for (int y = offset + 1; y < height - offset - 1; y++)
    {
        int row_offset = y * width;
        for (int x = offset + 1; x < width - offset - 1; x++)
        {
            float current_R = *(R + row_offset + x);
            if (current_R > threshold)
            {
                bool isMax = true;
                for (int ny = -1; ny <= 1; ny++)
                {
                    for (int nx = -1; nx <= 1; nx++)
                    {
                        if (ny == 0 && nx == 0) continue;
                        if (*(R + (y + ny) * width + (x + nx)) > current_R)
                        {
                            isMax = false; 
                            break;
                        }
                    }
                    if (!isMax) break;
                }
                
                if (isMax) cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), 1);
            }
        }
    }

    delete[] gray; delete[] Ixx; delete[] Iyy; delete[] Ixy;
    delete[] Sxx; delete[] Syy; delete[] Sxy; delete[] R;
}