#include "imgUtils.hpp"

/*
 * Hàm: main
 * Mục đích: Khởi tạo chương trình, chạy lệnh và đo thời gian thực thi.
 * Tham số:
 *   - argc: Số lượng param.
 *   - argv: Các chuỗi lệnh.
 * Trả về: Trạng thái thoát (0: OK).
 */
int main(int argc, char *argv[])
{
    // Cần truyền đủ tham số command line
    if (argc != 4)
    {
        // In hướng dẫn
        std::cout << "Usage: " << argv[0] << " <command> <InputFilePath> <OutputFilePath>" << std::endl;
        std::cout << "Example: " << argv[0] << " -optHarris input.jpg output.jpg" << std::endl;
        // Thoát
        return -1;
    }

    // Tên lệnh
    std::string command = argv[1];
    // File vào
    std::string inputFilePath = argv[2];
    // File ra
    std::string outputFilePath = argv[3];

    // Khởi tạo đối tượng
    imageProcessor proc;

    // Load ảnh src
    cv::Mat raw = proc.readImage(inputFilePath);
    // Crash nếu không có ảnh
    if (raw.empty()) return -1;
    
    // Ảnh dst
    cv::Mat dest; 

    // Kiểm tra lệnh cơ bản
    if (command == "-harris")
    {        
        // Ghi nhận mốc thời gian bắt đầu
        auto start = std::chrono::high_resolution_clock::now();
        // Gọi hàm Harris
        proc.harris(raw, dest);
        // Ghi nhận mốc thời gian kết thúc
        auto end = std::chrono::high_resolution_clock::now();
        
        // Tính toán khoảng thời gian (đơn vị mili-giây)
        std::chrono::duration<double, std::milli> time_taken = end - start;
        // In thời gian chạy ra màn hình
        std::cout << "[Basic Harris] Execution time: " << time_taken.count() << " ms" << std::endl;
    }
    // Kiểm tra lệnh tối ưu
    else if (command == "-optHarris")
    {
        // Ghi nhận mốc thời gian bắt đầu
        auto start = std::chrono::high_resolution_clock::now();
        // Gọi hàm optHarris
        proc.optHarris(raw, dest);
        // Ghi nhận mốc thời gian kết thúc
        auto end = std::chrono::high_resolution_clock::now();
        
        // Tính toán khoảng thời gian (đơn vị mili-giây)
        std::chrono::duration<double, std::milli> time_taken = end - start;
        // In thời gian chạy ra màn hình
        std::cout << "[Optimized Harris] Execution time: " << time_taken.count() << " ms" << std::endl;
    }
    // Cảnh báo nếu gõ sai lệnh
    else
    {
        // Lỗi
        std::cout << "Error: Unsupported command '" << command << "'." << std::endl;
        // Thoát
        return -1;
    }

    // Mở GUI xem kết quả
    proc.showImage("Harris Corner Result", dest);
    // Lưu ra đĩa
    if (!proc.saveImage(outputFilePath, dest)) return -1;

    // Kết thúc
    return 0;
}