# Computer Vision - CSC16004

*Lưu ý*: Để xem chi tiết các bài tập lab, vui lòng xem trong các branch khác.
Đây là chương trình giao diện dòng lệnh (CLI) được viết bằng **C++17** kết hợp với thư viện **OpenCV 4**. Chương trình thực hiện các thuật toán xử lý ảnh từ cơ bản (lọc nhiễu, dò biên) đến nâng cao (phát hiện góc Harris). Tất cả các thuật toán đều được cài đặt thủ công 100% từ đầu bằng cách thao tác trực tiếp trên bộ nhớ ảnh qua con trỏ, hoàn toàn không sử dụng các hàm xử lý thuật toán có sẵn của OpenCV.

## Các tính năng chính

* **Xử lý ảnh cơ bản:**
  * Chuyển đổi ảnh màu sang ảnh mức xám.
  * Tăng/giảm độ sáng và độ tương phản.
* **Lọc nhiễu (Image Filtering):**
  * Lọc trung bình (Average Filter), Lọc trung vị (Median Filter), Lọc Gauss (Gaussian Filter).
* **Phát hiện biên cạnh (Edge Detection):**
  * Toán tử Sobel, Toán tử Laplace.
* **Phát hiện đặc trưng (Feature Detection - Harris Corner):**
  * **Harris cơ bản (`-harris`):** Cài đặt tuần tự các bước tính đạo hàm Sobel, Tensor cấu trúc, làm trơn bằng bộ lọc Gauss 2D, tính độ phản hồi và triệt tiêu phi cực đại (NMS).
  * **Harris tối ưu (`-optHarris`):** Cải thiện mạnh mẽ tốc độ thực thi thông qua các kỹ thuật: Sử dụng toán tử số nguyên (Integer Arithmetic), Gộp vòng lặp (Loop Fusion) giảm thiểu truy xuất bộ nhớ, và Làm trơn ảnh bằng bộ lọc Gaussian 1D phân tách (Separable Gaussian Filter).

## Cấu trúc dự án

Dự án được tổ chức theo chuẩn phân cấp thư mục rõ ràng:

```text
.
├── bin/                       # Chứa file thực thi sau khi build lệnh make
├── build/                     # Chứa các file object (.o) và dependencies (.d)
├── images/
│   ├── input/                 # Thư mục chứa ảnh đầu vào (vd: test.jpg)
│   └── output/                # Thư mục lưu ảnh kết quả (sau khi xử lý/dò góc)
├── include/
│   ├── imgUtils.hpp           # Header khai báo lớp imageProcessor và các hàm
│   └── libs.hpp               # Header chứa các thư viện C++ và OpenCV cần thiết
├── src/
│   ├── imgUtils.cpp           # Cài đặt chi tiết logic thuật toán và thao tác pixel
│   └── main.cpp               # Hàm main xử lý tham số dòng lệnh và đo lường thời gian
├── .vscode/                   # Cấu hình môi trường VS Code (c_cpp_properties, launch, tasks)
├── Makefile                   # Kịch bản biên dịch dự án tự động
└── run.sh                     # Script chạy kiểm thử tự động toàn bộ tính năng
```

## Yêu cầu hệ thống

Dự án được phát triển và kiểm thử trên môi trường **Linux** (Arch Linux). Các package cần cài đặt:

* Trình biên dịch C++ hỗ trợ chuẩn C++17 (`gcc`/`g++`).
* Công cụ `make` và `pkg-config`.
* Thư viện **OpenCV 4**.

**Lệnh cài đặt trên Arch Linux:**

```bash
sudo pacman -Syu
sudo pacman -S base-devel gdb pkgconf opencv
```

## Hướng dẫn biên dịch

Chạy lệnh `make` để biên dịch toàn bộ mã nguồn:

```bash
make
```

*Lưu ý: Lệnh này sẽ tạo ra file thực thi mang tên `23122027` nằm trong thư mục `./bin/`.*

Để dọn dẹp các file đã biên dịch (khi cần build lại từ đầu):

```bash
make clean
```

## Lưu ý cho Visual Studio Code

Để tránh lỗi IntelliSense không nhận diện được thư viện OpenCV và thuận tiện cho việc debug C++ trong VSCode, bạn nên mở trực tiếp thư mục `./`:

```bash
code .
```

## Hướng dẫn sử dụng

Đảm bảo bạn đang ở trong thư mục gốc và đã build thành công. Chương trình hoạt động theo cú pháp dòng lệnh sau:

```bash
./bin/23122027 <command> <input_image_path> <output_image_path> [<args>]
```

*(Lưu ý: Sau khi chạy xong mỗi lệnh, chương trình sẽ in thời gian thực thi ra terminal và hiển thị một cửa sổ GUI chứa ảnh kết quả. **Nhấn phím bất kỳ trên cửa sổ ảnh để đóng và tự động lưu kết quả** vào đường dẫn output).*

### Danh sách các lệnh (`<command>`)

| Lệnh | Chức năng | Tham số phụ `<args>` | Ví dụ dòng lệnh |
| :--- | :--- | :--- | :--- |
| `-rgb2gray` | Chuyển ảnh màu sang xám | Không có | `./bin/23122027 -rgb2gray images/input/test.jpg images/output/1_gray.jpg` |
| `-brightness` | Thay đổi độ sáng | `c` (int): lượng sáng | `./bin/23122027 -brightness images/input/test.jpg images/output/2_brightness.jpg 50` |
| `-contrast` | Thay đổi độ tương phản | `c` (float): hệ số | `./bin/23122027 -contrast images/input/test.jpg images/output/3_contrast.jpg 1.5` |
| `-avg` | Lọc trung bình | `k` (int): size kernel (lẻ) | `./bin/23122027 -avg images/input/test.jpg images/output/4_avg.jpg 5` |
| `-med` | Lọc trung vị | `k` (int): size kernel (lẻ) | `./bin/23122027 -med images/input/test.jpg images/output/5_med.jpg 5` |
| `-gau` | Lọc Gauss | `k` (int): size kernel (lẻ) | `./bin/23122027 -gau images/input/test.jpg images/output/6_gau.jpg 5` |
| `-sobel` | Dò biên Sobel | Không có | `./bin/23122027 -sobel images/input/test.jpg images/output/7_sobel.jpg` |
| `-laplace` | Dò biên Laplace | Không có | `./bin/23122027 -laplace images/input/test.jpg images/output/8_laplace.jpg` |
| `-harris` | Dò góc Harris (Cơ bản) | Không có | `./bin/23122027 -harris images/input/test.jpg images/output/9_harris_basic.jpg` |
| `-optHarris` | Dò góc Harris (Tối ưu) | Không có | `./bin/23122027 -optHarris images/input/test.jpg images/output/10_harris_opt.jpg` |

## Chạy kiểm thử tự động

Dự án có cung cấp sẵn kịch bản `run.sh` giúp bạn tự động chạy đối chiếu thời gian thực thi của toàn bộ 10 chức năng trên bức ảnh mẫu `test.jpg`.

1. Di chuyển vào thư mục chứa mã nguồn:

2. Cấp quyền thực thi và chạy script:

    ```bash
    chmod +x run.sh
    ./run.sh
    ```

3. Terminal sẽ hiển thị tiến trình và thời gian chạy. Các ảnh kết quả sẽ được tự động xuất ra thư mục `./images/output/`. *(Nhớ bấm phím bất kỳ sau mỗi lần popup ảnh hiện lên để kịch bản tiếp tục chạy).*
