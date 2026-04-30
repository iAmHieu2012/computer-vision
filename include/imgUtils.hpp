#pragma once
#include "libs.hpp"

/*
 * Lớp: imageProcessor
 * Mục đích: Chứa các hàm hỗ trợ I/O, tiền xử lý và thực thi thuật toán Harris.
 */
class imageProcessor
{
private:
    /*
     * Hàm: cloneImage
     * Mục đích: Hàm copy ảnh.
     * Tham số: src - ảnh gốc cần copy.
     * Trả về: Ảnh bản sao (cv::Mat).
     */
    cv::Mat cloneImage(const cv::Mat &src);

    /*
     * Hàm: createReplicatePaddedImage
     * Mục đích: Tạo viền giá trị lặp lại viền xung quanh ảnh để phục vụ phép nhân chập.
     * Tham số: src - ảnh gốc cần thêm viền, pad - độ dày của viền.
     * Trả về: Ảnh đã được thêm viền (cv::Mat).
     */
    cv::Mat createReplicatePaddedImage(const cv::Mat &src, int pad);

    /*
     * Hàm: applyConvolution
     * Mục đích: Áp dụng phép tích chập (convolution) lên ảnh bằng một kernel.
     * Tham số: src - ảnh đầu vào, kernel - ma trận hạt nhân dạng vector 2D.
     * Trả về: Ảnh kết quả sau tích chập, định dạng float 32-bit (cv::Mat).
     */
    cv::Mat applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel);

    /*
     * Hàm: applyGaussianFloat
     * Mục đích: Áp dụng bộ lọc Gaussian 2D cho mảng float 1D (Dùng riêng cho Harris cơ bản).
     * Tham số: src (mảng đầu vào), dst (mảng kết quả), width, height, k (kích thước kernel).
     */
    void applyGaussianFloat(const float* src, float* dst, int width, int height, int k);

    /*
     * Hàm: applyGaussianSeparableFloat
     * Mục đích: Áp dụng Gaussian 1D phân tách ngang/dọc để tối ưu tốc độ (Dùng cho optHarris).
     * Tham số: src (mảng đầu vào), dst (mảng kết quả), width, height, k.
     */
    void applyGaussianSeparableFloat(const float* src, float* dst, int width, int height, int k);

public:
    /*
     * Constructor
     */
    imageProcessor();
    /*
     * Destructor
     */
    ~imageProcessor();

    /*
     * Hàm: readImage
     * Mục đích: Đọc ảnh từ hệ thống.
     * Tham số: filepath - chuỗi đường dẫn tới file ảnh.
     * Trả về: Ảnh cv::Mat.
     */
    cv::Mat readImage(const std::string &filepath);

    /*
     * Hàm: showImage
     * Mục đích: Hiển thị ảnh.
     * Tham số: windowName - tên cửa sổ, img - ảnh cần hiển thị.
     * Trả về: Không
     */
    void showImage(const std::string &windowName, const cv::Mat &img);

    /*
     * Hàm: saveImage
     * Mục đích: Lưu ảnh.
     * Tham số: filepath - đường dẫn lưu, img - ảnh cần lưu.
     * Trả về: bool báo thành công/thất bại.
     */
    bool saveImage(const std::string &filepath, const cv::Mat &img);

    /*
     * Hàm: rgb2gray
     * Mục đích: Chuyển đổi ảnh màu sang xám.
     * Tham số: src - ảnh gốc đầu vào, dst - ảnh xám đầu ra.
     * Trả về: Không
     */
    void rgb2gray(const cv::Mat &src, cv::Mat &dst);

    /*
     * Hàm: harris
     * Mục đích: Dò tìm Harris corner cơ bản.
     * Tham số: src - ảnh đầu vào, dst - ảnh đầu ra vẽ sẵn điểm.
     * Trả về: Không
     */
    void harris(const cv::Mat &src, cv::Mat &dst);

    /*
     * Hàm: optHarris
     * Mục đích: Dò tìm Harris đã tối ưu.
     * Tham số: src - ảnh đầu vào, dst - ảnh đầu ra vẽ sẵn điểm.
     * Trả về: Không
     */
    void optHarris(const cv::Mat &src, cv::Mat &dst);
};