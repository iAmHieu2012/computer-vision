#include "imgUtils.hpp"

// Constructor
imageProcessor::imageProcessor() {}
// Destructor
imageProcessor::~imageProcessor() {}

/*
 * Hàm: readImage
 * Mục đích: Đọc ảnh từ hệ thống.
 * Tham số: filepath - chuỗi đường dẫn tới file ảnh.
 * Trả về: Ảnh cv::Mat.
 */
cv::Mat imageProcessor::readImage(const std::string &filepath)
{
    // Đọc ảnh từ đường dẫn, luôn load ở chế độ màu
    cv::Mat img = cv::imread(filepath, cv::IMREAD_COLOR);
    // Kiểm tra nếu ảnh trống do sai đường dẫn
    if (img.empty())
        // Báo lỗi ra màn hình console
        std::cerr << "Error: Cannot read image from: " << filepath << std::endl;
    // Trả về ma trận ảnh
    return img;
}

/*
 * Hàm: showImage
 * Mục đích: Hiển thị ảnh.
 * Tham số: windowName - tên cửa sổ, img - ảnh cần hiển thị.
 * Trả về: Không
 */
void imageProcessor::showImage(const std::string &windowName, const cv::Mat &img)
{
    // Nếu ảnh trống thì không làm gì cả
    if (img.empty()) return;
    // Mở cửa sổ GUI hiển thị ảnh
    cv::imshow(windowName, img);
    // Chờ người dùng nhấn phím bất kỳ
    cv::waitKey(0);
}

/*
 * Hàm: saveImage
 * Mục đích: Lưu ảnh.
 * Tham số: filepath - đường dẫn lưu, img - ảnh cần lưu.
 * Trả về: bool báo thành công/thất bại.
*/
bool imageProcessor::saveImage(const std::string &filepath, const cv::Mat &img)
{
    // Không lưu nếu ảnh bị trống
    if (img.empty()) return false;
    // Ghi file ra ổ cứng
    bool p = cv::imwrite(filepath, img);
    // In lỗi nếu ghi thất bại
    if (!p) std::cerr << "Error: Cannot save image to " << filepath << std::endl;
    // Trả về trạng thái lưu
    return p;
}

/*
 * Hàm: cloneImage
 * Mục đích: Hàm copy ảnh.
 * Tham số: src - ảnh gốc cần copy.
 * Trả về: Ảnh bản sao (cv::Mat).
 */
cv::Mat imageProcessor::cloneImage(const cv::Mat &src)
{
    // Trả về ma trận rỗng nếu ảnh đầu vào rỗng
    if (src.empty()) return cv::Mat();

    // Lấy chiều cao ảnh gốc
    int h = src.rows;
    // Lấy chiều rộng ảnh gốc
    int w = src.cols;
    // Lấy số kênh màu ảnh gốc
    int ch = src.channels();
    // Lấy bước nhớ của ảnh gốc
    int src_step = src.step;
    // Khởi tạo ma trận đích với cùng kích thước và kiểu dữ liệu
    cv::Mat dst(h, w, src.type());
    // Lấy bước nhớ của ma trận đích
    int dst_step = dst.step;

    // Lấy con trỏ dữ liệu ảnh gốc
    uchar* s_data = src.data;
    // Lấy con trỏ dữ liệu ảnh đích
    uchar* d_data = dst.data;

    // Duyệt qua từng hàng
    for (int y = 0; y < h; y++)
    {
        // Duyệt qua từng cột
        for (int x = 0; x < w; x++)
        {
            // Duyệt qua từng kênh màu
            for (int c = 0; c < ch; c++)
            {
                // Copy từng byte dữ liệu
                *(d_data + y * dst_step + x * ch + c) = *(s_data + y * src_step + x * ch + c);
            }
        }
    }
    // Trả về ảnh bản sao
    return dst;
}

/*
 * Hàm: rgb2gray
 * Mục đích: Chuyển đổi ảnh màu sang xám.
 * Tham số: src - ảnh gốc đầu vào, dst - ảnh xám đầu ra.
 * Trả về: Không
 */
void imageProcessor::rgb2gray(const cv::Mat &src, cv::Mat &dst)
{
    // Nếu ảnh gốc trống thì bỏ qua
    if (src.empty()) return;
    
    // Nếu ảnh đã là 1 kênh (xám sẵn) thì chỉ copy
    if (src.channels() == 1) {
        // Copy ảnh vào dst
        dst = this->cloneImage(src);
        // Thoát hàm
        return;
    }

    // Lấy thông số chiều rộng
    int width = src.cols;
    // Lấy thông số chiều cao
    int height = src.rows;
    // Lấy số kênh màu
    int channels = src.channels();
    // Lấy bước nhớ (độ dài byte của 1 hàng ảnh gốc)
    int src_step = src.step;
    // Lấy con trỏ dữ liệu ảnh gốc
    uchar* src_data = src.data;

    // Khởi tạo ảnh đích 1 kênh màu (8-bit unsigned)
    dst = cv::Mat(height, width, CV_8UC1);
    // Lấy bước nhớ ảnh đích
    int dst_step = dst.step;
    // Lấy con trỏ dữ liệu ảnh đích
    uchar* dst_data = dst.data;

    // Duyệt qua từng hàng
    for (int y = 0; y < height; y++)
    {
        // Duyệt qua từng cột
        for (int x = 0; x < width; x++)
        {
            // Trích xuất kênh màu Blue
            uchar b = *(src_data + y * src_step + x * channels + 0);
            // Trích xuất kênh màu Green
            uchar g = *(src_data + y * src_step + x * channels + 1);
            // Trích xuất kênh màu Red
            uchar r = *(src_data + y * src_step + x * channels + 2);
            
            // Tính độ xám theo công thức chuẩn và gán vào ảnh đích
            *(dst_data + y * dst_step + x) = (uchar)(0.114f * b + 0.587f * g + 0.299f * r);
        }
    }
}

/*
 * Hàm: createReplicatePaddedImage
 * Mục đích: Tạo viền giá trị lặp lại xung quanh ảnh để phục vụ phép nhân chập.
 * Tham số: src - ảnh gốc cần thêm viền, pad - độ dày của viền.
 * Trả về: Ảnh đã được thêm viền (cv::Mat).
 */
cv::Mat imageProcessor::createReplicatePaddedImage(const cv::Mat &src, int pad)
{
    // Lấy thông số chiều cao ảnh gốc
    int height = src.rows;
    // Lấy thông số chiều rộng ảnh gốc
    int width = src.cols;
    // Lấy số kênh màu ảnh gốc
    int ch = src.channels();
    // Lấy bước nhớ ảnh gốc (số byte trên 1 hàng)
    int src_step = src.step;
    // Lấy con trỏ thô trỏ tới mảng dữ liệu ảnh gốc
    uchar* src_data = src.data;

    // Tính toán chiều cao ảnh mới sau khi đắp viền
    int pad_height = height + 2 * pad;
    // Tính toán chiều rộng ảnh mới sau khi đắp viền
    int pad_width = width + 2 * pad;

    // Khởi tạo ảnh mới để chứa kết quả đắp viền (chưa có dữ liệu)
    cv::Mat padded(pad_height, pad_width, src.type());
    // Lấy bước nhớ của ảnh đã đắp viền
    int pad_step = padded.step;
    // Lấy con trỏ thô trỏ tới mảng dữ liệu ảnh đích
    uchar* pad_data = padded.data;

    // Duyệt qua từng hàng của ảnh mới (ảnh đích)
    for (int y = 0; y < pad_height; y++)
    {
        // Xác định tọa độ y tương ứng trên ảnh gốc
        int src_y = y - pad;
        // Nếu y nằm ở khu vực viền trên, gán tọa độ chiếu bằng lề trên (0)
        if (src_y < 0) src_y = 0;
        // Nếu y nằm ở khu vực viền dưới, gán tọa độ chiếu bằng lề dưới (height - 1)
        else if (src_y >= height) src_y = height - 1;

        // Tính trước offset của hàng trên ảnh gốc để tiết kiệm phép nhân
        int src_row_offset = src_y * src_step;
        // Tính trước offset của hàng trên ảnh đích
        int pad_row_offset = y * pad_step;

        // Duyệt qua từng cột của ảnh mới
        for (int x = 0; x < pad_width; x++)
        {
            // Xác định tọa độ x tương ứng trên ảnh gốc
            int src_x = x - pad;
            // Nếu x nằm ở khu vực viền trái, gán tọa độ chiếu bằng lề trái (0)
            if (src_x < 0) src_x = 0;
            // Nếu x nằm ở khu vực viền phải, gán tọa độ chiếu bằng lề phải (width - 1)
            else if (src_x >= width) src_x = width - 1;

            // Tính offset cột của ảnh gốc
            int src_col_offset = src_x * ch;
            // Tính offset cột của ảnh đích
            int pad_col_offset = x * ch;

            // Duyệt qua từng kênh màu (hỗ trợ cả ảnh xám 1 kênh và màu 3 kênh)
            for (int c = 0; c < ch; c++)
            {
                // Truy xuất và copy pixel từ ảnh gốc sang ảnh mới bằng pointer arithmetic
                *(pad_data + pad_row_offset + pad_col_offset + c) = 
                    *(src_data + src_row_offset + src_col_offset + c);
            }
        }
    }
    
    // Trả về ảnh đã được đắp viền lặp lại
    return padded;
}

/*
 * Hàm: applyConvolution
 * Mục đích: Áp dụng phép tích chập (convolution) lên ảnh bằng một kernel.
 * Tham số: src - ảnh đầu vào, kernel - ma trận hạt nhân dạng vector 2D.
 * Trả về: Ảnh kết quả sau tích chập, định dạng float 32-bit (cv::Mat).
 */
cv::Mat imageProcessor::applyConvolution(const cv::Mat &src, const std::vector<std::vector<float>> &kernel)
{
    // Lấy thông số cấu trúc ảnh
    int height = src.rows;
    int width = src.cols;
    int ch = src.channels();
    // Lấy kích thước kernel
    int k = kernel.size();
    // Tính số pixel viền cần đắp
    int pad = k / 2;
    
    // Gọi hàm đắp viền cho ảnh đầu vào
    cv::Mat paddedSrc = createReplicatePaddedImage(src, pad);
    // Lấy con trỏ ảnh đã đắp viền
    uchar* pad_data = paddedSrc.data;
    // Lấy bước nhớ ảnh đã đắp viền
    int pad_step = paddedSrc.step;

    // Tạo ảnh chứa kết quả dạng Float 32-bit (vì đạo hàm có số âm)
    cv::Mat result(height, width, CV_32FC(ch), cv::Scalar(0));
    // Ép kiểu con trỏ sang float
    float* result_data = (float*)result.data;
    // Tính bước nhớ theo đơn vị float (số byte chia kích thước 1 float)
    int result_step = result.step / sizeof(float);

    // Lặp qua hàng
    for (int y = 0; y < height; y++)
    {
        // Lặp qua cột
        for (int x = 0; x < width; x++)
        {
            // Lặp qua từng kênh màu
            for (int c = 0; c < ch; c++)
            {
                // Biến giữ tổng giá trị tích chập
                float sum = 0.0f;
                // Quét hàng trong ma trận kernel
                for (int ky = -pad; ky <= pad; ky++)
                {
                    // Quét cột trong ma trận kernel
                    for (int kx = -pad; kx <= pad; kx++)
                    {
                        // Tính y của pixel lân cận
                        int py = y + pad + ky;
                        // Tính x của pixel lân cận
                        int px = x + pad + kx;
                        // Trích xuất giá trị pixel
                        uchar pixel = *(pad_data + py * pad_step + px * ch + c);
                        // Nhân với giá trị kernel tương ứng và cộng dồn
                        sum += pixel * kernel[-ky + pad][-kx + pad];
                    }
                }
                // Lưu tổng nhân chập vào vị trí trung tâm ảnh kết quả
                *(result_data + y * result_step + x * ch + c) = sum;
            }
        }
    }
    // Trả về ma trận đã tích chập với kernel
    return result;
}

/*
 * Hàm: applyGaussianFloat
 * Mục đích: Áp dụng bộ lọc Gaussian 2D cho mảng float 1D (Dùng riêng cho Harris cơ bản).
 * Tham số: src (mảng đầu vào), dst (mảng kết quả), width, height, k (kích thước kernel).
 */
void imageProcessor::applyGaussianFloat(const float* src, float* dst, int width, int height, int k)
{
    // Tính bán kính của cửa sổ lân cận
    int pad = k / 2;
    // Khởi tạo vector 2 chiều chứa các trọng số Gaussian
    std::vector<std::vector<float>> G(k, std::vector<float>(k));
    // Tính toán độ lệch chuẩn (sigma)
    float sigma = 0.3f * ((k - 1) * 0.5f - 1.0f) + 0.8f;
    // Khởi tạo biến lưu tổng trọng số
    float sum_G = 0.0f;
    // Định nghĩa hằng số Pi
    const float PI = 3.1415926535f;

    // Sinh Kernel 2D
    for (int i = -pad; i <= pad; i++) {
        for (int j = -pad; j <= pad; j++) {
            // Áp dụng công thức hàm phân phối Gaussian 2 chiều
            G[i + pad][j + pad] = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            // Cộng dồn tổng trọng số
            sum_G += G[i + pad][j + pad];
        }
    }
    // Chuẩn hóa kernel 2 chiều để tổng các phần tử bằng 1
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++) G[i][j] /= sum_G;

    // Áp dụng Kernel lên mảng Float
    // Quét qua các hàng, bỏ qua phần viền trên/dưới
    for (int y = pad; y < height - pad; y++) {
        // Quét qua các cột, bỏ qua phần viền trái/phải
        for (int x = pad; x < width - pad; x++) {
            // Khởi tạo biến tổng cho từng pixel
            float sum = 0.0f;
            // Quét lân cận theo trục Y (hàng)
            for (int wy = -pad; wy <= pad; wy++) {
                // Quét lân cận theo trục X (cột)
                for (int wx = -pad; wx <= pad; wx++) {
                    // Xác định chỉ số mảng 1D của pixel lân cận
                    int n_idx = (y + wy) * width + (x + wx);
                    // Nhân giá trị pixel lân cận với trọng số Gaussian 2D tương ứng và cộng dồn
                    sum += *(src + n_idx) * G[wy + pad][wx + pad];
                }
            }
            // Ghi kết quả sau khi làm mượt vào mảng đích
            *(dst + y * width + x) = sum;
        }
    }
}

/*
 * Hàm: applyGaussianSeparableFloat
 * Mục đích: Áp dụng Gaussian 1D phân tách ngang/dọc để tối ưu tốc độ (Dùng cho optHarris).
 * Tham số: src (mảng đầu vào), dst (mảng kết quả), width, height, k.
 */
void imageProcessor::applyGaussianSeparableFloat(const float* src, float* dst, int width, int height, int k)
{
    // Tính bán kính của cửa sổ lân cận (vd k=5 thì pad=2)
    int pad = k / 2;
    // Khởi tạo vector 1 chiều chứa các trọng số Gaussian
    std::vector<float> G1D(k);
    // Tính toán độ lệch chuẩn (sigma) theo công thức chuẩn của OpenCV
    float sigma = 0.3f * ((k - 1) * 0.5f - 1.0f) + 0.8f;
    // Khởi tạo biến lưu tổng trọng số để chuẩn hóa sau này
    float sum_G = 0.0f;
    // Định nghĩa hằng số Pi
    const float PI = 3.1415926535f;

    // Sinh Kernel 1D
    for (int i = -pad; i <= pad; i++) {
        // Áp dụng công thức hàm phân phối Gaussian 1 chiều
        G1D[i + pad] = std::exp(-(i * i) / (2 * sigma * sigma)) / (std::sqrt(2 * PI) * sigma);
        // Cộng dồn giá trị vào tổng để chuẩn hóa
        sum_G += G1D[i + pad];
    }
    // Chuẩn hóa kernel 1 chiều sao cho tổng các phần tử bằng 1
    for (int i = 0; i < k; i++) G1D[i] /= sum_G;

    // Cấp phát mảng 1 chiều làm buffer trung gian cho lần quét ngang
    float* temp = new float[width * height]{0};

    // Quét ngang (Horizontal pass)
    for (int y = 0; y < height; y++) {
        // Bỏ qua phần viền 2 bên trái phải
        for (int x = pad; x < width - pad; x++) {
            // Khởi tạo biến tổng cho từng pixel
            float sum = 0.0f;
            // Quét các phần tử lân cận trên cùng một hàng (trục X)
            for (int wx = -pad; wx <= pad; wx++) {
                // Nhân giá trị pixel với trọng số Gaussian 1D tương ứng
                sum += *(src + y * width + (x + wx)) * G1D[wx + pad];
            }
            // Lưu kết quả quét ngang vào buffer trung gian
            *(temp + y * width + x) = sum;
        }
    }

    // Quét dọc (Vertical pass)
    for (int y = pad; y < height - pad; y++) {
        // Quét qua toàn bộ các cột (vì viền X đã được xử lý ở bước quét ngang)
        for (int x = 0; x < width; x++) {
            // Khởi tạo biến tổng cho từng pixel
            float sum = 0.0f;
            // Quét các phần tử lân cận trên cùng một cột (trục Y)
            for (int wy = -pad; wy <= pad; wy++) {
                // Nhân giá trị từ buffer trung gian với trọng số Gaussian 1D
                sum += *(temp + (y + wy) * width + x) * G1D[wy + pad];
            }
            // Lưu kết quả cuối cùng (đã mượt cả 2 chiều) vào mảng đích
            *(dst + y * width + x) = sum;
        }
    }
    // Giải phóng bộ nhớ của buffer trung gian
    delete[] temp;
}

/*
 * Hàm: harris
 * Mục đích: Dò tìm Harris corner cơ bản.
 * Tham số: src - ảnh đầu vào, dst - ảnh đầu ra vẽ sẵn điểm.
 * Trả về: Không
 */
void imageProcessor::harris(const cv::Mat &src, cv::Mat &dst)
{
    // Kiểm tra đầu vào
    if (src.empty()) return;

    // Giữ lại ảnh gốc để lát nữa vẽ kết quả lên
    dst = this->cloneImage(src);
    // Khai báo ma trận chứa ảnh xám
    cv::Mat grayImg;
    // Gọi hàm chuyển xám
    this->rgb2gray(src, grayImg);

    // --- BƯỚC 1: TÍNH ĐẠO HÀM THEO X VÀ Y BẰNG CONVOLUTION ---
    // Định nghĩa kernel Sobel X
    std::vector<std::vector<float>> Kx = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    // Định nghĩa kernel Sobel Y
    std::vector<std::vector<float>> Ky = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    // Áp dụng convolution để tìm Gradient X
    cv::Mat gradX = applyConvolution(grayImg, Kx);
    // Áp dụng convolution để tìm Gradient Y
    cv::Mat gradY = applyConvolution(grayImg, Ky);

    // Lấy chiều rộng
    int width = src.cols;
    // Lấy chiều cao
    int height = src.rows;
    // Lấy con trỏ float của ma trận Gradient X
    float* gx_data = (float*)gradX.data;
    // Lấy con trỏ float của ma trận Gradient Y
    float* gy_data = (float*)gradY.data;
    // Tính bước nhớ theo float cho mảng gradient
    int g_step = gradX.step / sizeof(float);

    // --- BƯỚC 2: TÍNH TENSOR CẤU TRÚC ---
    // Cấp phát mảng Ixx (Ix bình phương)
    float* Ixx = new float[width * height]{0};
    // Cấp phát mảng Iyy (Iy bình phương)
    float* Iyy = new float[width * height]{0};
    // Cấp phát mảng Ixy (Ix nhân Iy)
    float* Ixy = new float[width * height]{0};

    // Duyệt hàng
    for (int y = 0; y < height; y++)
    {
        // Duyệt cột
        for (int x = 0; x < width; x++)
        {
            // Lấy giá trị gradient x
            float ix = *(gx_data + y * g_step + x);
            // Lấy giá trị gradient y
            float iy = *(gy_data + y * g_step + x);
            
            // Tính index 1D
            int idx = y * width + x;
            // Lưu bình phương ix
            *(Ixx + idx) = ix * ix;
            // Lưu bình phương iy
            *(Iyy + idx) = iy * iy;
            // Lưu tích ix, iy
            *(Ixy + idx) = ix * iy;
        }
    }

    // --- BƯỚC 3: LÀM TRƠN TENSOR BẰNG GAUSSIAN FILTER---
    // Bán kính offset
    int offset = 2; 
    // Cấp phát mảng Sxx (đã làm mượt)
    float* Sxx = new float[width * height]{0};
    // Cấp phát mảng Syy (đã làm mượt)
    float* Syy = new float[width * height]{0};
    // Cấp phát mảng Sxy (đã làm mượt)
    float* Sxy = new float[width * height]{0};
    
    // Gọi phương thức Gaussian Float làm mượt Ixx
    applyGaussianFloat(Ixx, Sxx, width, height, 5);
    // Gọi phương thức Gaussian Float làm mượt Iyy
    applyGaussianFloat(Iyy, Syy, width, height, 5);
    // Gọi phương thức Gaussian Float làm mượt Ixy
    applyGaussianFloat(Ixy, Sxy, width, height, 5);

    // --- BƯỚC 4: TÍNH ĐỘ PHẢN HỒI HARRIS ---
    // Cấp phát mảng R
    float* R = new float[width * height]{0};
    // Hằng số kinh nghiệm k
    float k = 0.04f;
    // Biến lưu R cực đại
    float max_R = 0.0f;

    // Duyệt mảng (chừa viền)
    for (int y = offset; y < height - offset; y++)
    {
        // Duyệt mảng (chừa viền)
        for (int x = offset; x < width - offset; x++)
        {
            // Lấy index
            int idx = y * width + x;
            // Định thức ma trận cấu trúc = Sxx * Syy - Sxy^2
            float det = *(Sxx + idx) * *(Syy + idx) - *(Sxy + idx) * *(Sxy + idx);
            // Vết của ma trận = Sxx + Syy
            float trace = *(Sxx + idx) + *(Syy + idx);
            
            // R = det - k * trace^2
            float r_val = det - k * trace * trace;
            // Lưu vào mảng
            *(R + idx) = r_val;
            
            // Cập nhật max_R
            if (r_val > max_R) max_R = r_val;
        }
    }

    // --- BƯỚC 5: NON-MAXIMUM SUPPRESSION ---
    // Ngưỡng lọc
    float threshold = 0.01f * max_R;

    // Quét ảnh
    for (int y = offset + 1; y < height - offset - 1; y++)
    {
        // Quét ảnh
        for (int x = offset + 1; x < width - offset - 1; x++)
        {
            // Lấy R tại tâm
            float current_R = *(R + y * width + x);
            // So sánh với ngưỡng
            if (current_R > threshold)
            {
                // Cờ giữ cực đại
                bool isMax = true;
                // Quét 8 lân cận
                for (int ny = -1; ny <= 1; ny++)
                {
                    // Quét lân cận
                    for (int nx = -1; nx <= 1; nx++)
                    {
                        // Bỏ qua tâm
                        if (ny == 0 && nx == 0) continue;
                        // Nếu có điểm xung quanh lớn hơn
                        if (*(R + (y + ny) * width + (x + nx)) > current_R)
                        {
                            // Tắt cờ cực đại
                            isMax = false; 
                            // Dừng kiểm tra lân cận
                            break;
                        }
                    }
                    // Dừng hẳn nếu mất cờ
                    if (!isMax) break;
                }
                
                // Nếu vượt qua kiểm tra, vẽ điểm lên dst
                if (isMax)
                    cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), 1);
            }
        }
    }

    // Dọn dẹp bộ nhớ chống leak ram
    delete[] Ixx; delete[] Iyy; delete[] Ixy;
    delete[] Sxx; delete[] Syy; delete[] Sxy; delete[] R;
}

/*
 * Hàm: optHarris
 * Mục đích: Dò tìm Harris đã tối ưu.
 * Tham số: src - ảnh đầu vào, dst - ảnh đầu ra vẽ sẵn điểm.
 * Trả về: Không
 */
void imageProcessor::optHarris(const cv::Mat &src, cv::Mat &dst)
{
    // Kiểm tra rỗng
    if (src.empty()) return;
    
    // Copy sang ảnh đích
    dst = this->cloneImage(src);
    // Lấy thông số cấu trúc
    int width = src.cols, height = src.rows, channels = src.channels(), step = src.step;
    // Lấy con trỏ thô
    uchar* data = src.data;

    // Cấp mảng xám 1D
    uchar* gray = new uchar[width * height];
    // Cấp mảng Ixx
    float* Ixx = new float[width * height]{0};
    // Cấp mảng Iyy
    float* Iyy = new float[width * height]{0};
    // Cấp mảng Ixy
    float* Ixy = new float[width * height]{0};

    // --- TỐI ƯU 1: GỘP CHUYỂN XÁM, DÙNG TOÁN TỬ SỐ NGUYÊN ---
    for (int y = 0; y < height; y++)
    {
        // Tính sẵn offset hàng
        int row_offset = y * step;
        // Tính sẵn offset mảng 1D
        int gray_row_offset = y * width;
        // Duyệt cột
        for (int x = 0; x < width; x++)
        {
            // Lấy kênh b
            uchar b = *(data + row_offset + x * channels + 0);
            // Lấy kênh g
            uchar g = *(data + row_offset + x * channels + 1);
            // Lấy kênh r
            uchar r = *(data + row_offset + x * channels + 2);
            // Toán tử nguyên: chia 1000 nhanh hơn nhân float
            *(gray + gray_row_offset + x) = (uchar)((b * 114 + g * 587 + r * 299) / 1000);
        }
    }

    // --- TỐI ƯU 2: GỘP ĐẠO HÀM VÀ BÌNH PHƯƠNG TENSOR VÀO 1 VÒNG ---
    for (int y = 1; y < height - 1; y++)
    {
        // Offset hàng trên
        int prev_y = (y - 1) * width;
        // Offset hàng giữa
        int curr_y = y * width;
        // Offset hàng dưới
        int next_y = (y + 1) * width;
        
        // Quét cột
        for (int x = 1; x < width - 1; x++)
        {
            // Sobel X tính trực tiếp
            float gx = -*(gray + prev_y + x - 1) + *(gray + prev_y + x + 1)
                       -2.0f * *(gray + curr_y + x - 1) + 2.0f * *(gray + curr_y + x + 1)
                       -*(gray + next_y + x - 1) + *(gray + next_y + x + 1);
            // Sobel Y tính trực tiếp
            float gy = -*(gray + prev_y + x - 1) - 2.0f * *(gray + prev_y + x) - *(gray + prev_y + x + 1)
                       +*(gray + next_y + x - 1) + 2.0f * *(gray + next_y + x) + *(gray + next_y + x + 1);

            // Index hiện tại
            int idx = curr_y + x;
            // Lưu Ixx không cần biến đệm Ix
            *(Ixx + idx) = gx * gx;
            // Lưu Iyy
            *(Iyy + idx) = gy * gy;
            // Lưu Ixy
            *(Ixy + idx) = gx * gy;
        }
    }

    // --- TỐI ƯU 3: LÀM TRƠN TENSOR BẰNG SEPARABLE GAUSSIAN (1D) ---
    // Bán kính 2
    int offset = 2;
    // Cấp phát mảng Sxx
    float* Sxx = new float[width * height]{0};
    // Cấp phát mảng Syy
    float* Syy = new float[width * height]{0};
    // Cấp phát mảng Sxy
    float* Sxy = new float[width * height]{0};

    // Áp dụng Gaussian Separable tốc độ cao
    applyGaussianSeparableFloat(Ixx, Sxx, width, height, 5);
    applyGaussianSeparableFloat(Iyy, Syy, width, height, 5);
    applyGaussianSeparableFloat(Ixy, Sxy, width, height, 5);

    // Cấp bộ nhớ mảng R
    float* R = new float[width * height]{0};
    // Các biến phụ
    float max_R = 0.0f, k = 0.04f;

    // Duyệt mảng (chừa viền)
    for (int y = offset; y < height - offset; y++)
    {
        // Duyệt mảng (chừa viền)
        for (int x = offset; x < width - offset; x++)
        {
            // Lấy index
            int idx = y * width + x;
            // Định thức ma trận cấu trúc = Sxx * Syy - Sxy^2
            float det = *(Sxx + idx) * *(Syy + idx) - *(Sxy + idx) * *(Sxy + idx);
            // Vết của ma trận = Sxx + Syy
            float trace = *(Sxx + idx) + *(Syy + idx);
            
            // R = det - k * trace^2
            float r_val = det - k * trace * trace;
            // Lưu vào mảng
            *(R + idx) = r_val;
            
            // Cập nhật max_R
            if (r_val > max_R) max_R = r_val;
        }
    }

    // --- BƯỚC 4: LỌC NMS ---
    // Ngưỡng 1%
    float threshold = 0.01f * max_R;
    
    // Quét ảnh
    for (int y = offset + 1; y < height - offset - 1; y++)
    {
        // Offset
        int row_offset = y * width;
        // Quét ảnh
        for (int x = offset + 1; x < width - offset - 1; x++)
        {
            // Điểm tâm
            float current_R = *(R + row_offset + x);
            // Kiểm ngưỡng
            if (current_R > threshold)
            {
                // Cờ
                bool isMax = true;
                // Vòng y lân cận
                for (int ny = -1; ny <= 1; ny++)
                {
                    // Vòng x lân cận
                    for (int nx = -1; nx <= 1; nx++)
                    {
                        // Bỏ qua tâm
                        if (ny == 0 && nx == 0) continue;
                        // So sánh độ lớn
                        if (*(R + (y + ny) * width + (x + nx)) > current_R)
                        {
                            // Tắt cực đại
                            isMax = false; 
                            // Dừng check
                            break;
                        }
                    }
                    // Break luôn vòng y
                    if (!isMax) break;
                }
                
                // Trụ vững thì vẽ
                if (isMax) cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), 1);
            }
        }
    }

    // Free toàn bộ pointer
    delete[] gray; delete[] Ixx; delete[] Iyy; delete[] Ixy;
    delete[] Sxx; delete[] Syy; delete[] Sxy; delete[] R;
}