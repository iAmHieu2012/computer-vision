# Harris Corner Detection

Đây là chương trình giao diện dòng lệnh (CLI) được viết bằng **C/C++** kết hợp với thư viện **OpenCV 4**. Chương trình thực hiện thuật toán phát hiện góc Harris được cài đặt thủ công 100% từ đầu bằng cách thao tác trực tiếp trên bộ nhớ ảnh qua con trỏ, hoàn toàn không sử dụng các hàm xử lý ảnh có sẵn của OpenCV.

## Các tính năng chính

* **Phát hiện corner Harris cơ bản (`-harris`):** Cài đặt tuần tự các bước tính đạo hàm Sobel, Tensor cấu trúc, làm trơn bằng bộ lọc Gauss 2D, tính độ phản hồi và triệt tiêu phi cực đại (NMS).
* **Phát hiện corner Harris tối ưu (`-optHarris`):** Cải thiện mạnh mẽ tốc độ thực thi thông qua các kỹ thuật:
  * Sử dụng toán tử số nguyên (Integer Arithmetic) thay cho số phẩy động.
  * Gộp vòng lặp (Loop Fusion) để giảm thiểu truy xuất bộ nhớ.
  * Làm trơn ảnh bằng bộ lọc Gaussian 1D phân tách (Separable Gaussian Filter).

## Cấu trúc dự án

Dự án được tổ chức theo chuẩn yêu cầu đồ án:

```text
.
├── Data/
│   ├── input/                     # Thư mục chứa ảnh đầu vào (vd: test.jpg)
│   └── output/                    # Thư mục lưu ảnh kết quả (vẽ sẵn các vòng tròn đỏ)
├── Document/
│   ├── 23122027-report.pdf        # Báo cáo đồ án chi tiết
│   └── README.md                  # File hướng dẫn này
├── Executable/
│   └── 23122027                   # File thực thi đã build sẵn
└── Sources/
    ├── bin/                       # Chứa file thực thi sau khi build (23122027)
    ├── build/                     # Chứa các file object (.o) và dependencies (.d)
    ├── images/
    │   ├── input/                 # Thư mục chứa ảnh đầu vào (vd: test.jpg)
    │   └── output/                # Thư mục lưu ảnh kết quả (vẽ sẵn các vòng tròn đỏ)
    ├── include/
    │   ├── imgUtils.hpp           # Header khai báo lớp imageProcessor và các hàm
    │   └── libs.hpp               # Header chứa các thư viện C++ và OpenCV cần thiết
    ├── src/
    │   ├── imgUtils.cpp           # Cài đặt chi tiết logic thuật toán Harris và thao tác pixel
    │   └── main.cpp               # Hàm main xử lý tham số dòng lệnh và đo lường thời gian
    ├── .vscode/                   # Cấu hình môi trường VS Code (c_cpp_properties, launch, tasks)
    ├── Makefile                   # Kịch bản biên dịch dự án tự động
    └── run.sh                     # Script chạy kiểm thử tự động
```

## Yêu cầu hệ thống

Dự án được phát triển và kiểm thử trên môi trường **Linux** (Arch Linux). Các package cần cài đặt:

* Trình biên dịch C++ hỗ trợ chuẩn C++17 (`gcc`/`g++`).
* Công cụ `make` và `pkg-config`.
* Thư viện **OpenCV 4**.

**Cài đặt trên Arch Linux:**

```bash
sudo pacman -Syu
sudo pacman -S base-devel gdb pkgconf opencv
```

## Hướng dẫn biên dịch

Để biên dịch chương trình, bạn cần di chuyển vào thư mục `Sources`:

```bash
cd Sources
```

Chạy lệnh `make` để biên dịch toàn bộ mã nguồn:

```bash
make
```

*Lưu ý: Lệnh này sẽ tạo ra file thực thi mang tên `23122027` nằm trong thư mục `Sources/bin/`.*

Để dọn dẹp các file đã biên dịch:

```bash
make clean
```

## Lưu ý cho Visual Studio Code

Để tránh lỗi IntelliSense không nhận diện được thư viện OpenCV và thuận tiện cho việc debug C++ trong VSCode, bạn nên mở trực tiếp thư mục `Sources`:

```bash
cd Sources
code .
```

## Hướng dẫn sử dụng

Đảm bảo bạn đang ở trong thư mục `Sources` và đã build thành công. Chương trình hoạt động theo cú pháp dòng lệnh sau:

```bash
./bin/23122027 <command> <input_image_path> <output_image_path>
```

Hoặc nếu đang ở thư mục gốc (`23122027`), ta cũng có thể chạy file thực thi đã build sẵn:

```bash
./Executable/23122027 <command> <input_image_path> <output_image_path>
```

### Danh sách các lệnh (`<command>`)

*Lưu ý: Vẫn đang ở thư mục `Sources`*

| Lệnh | Chức năng | Ví dụ |
| :--- | :--- | :--- |
| `-harris` | Phát hiện corner bằng thuật toán Harris cơ bản | `./bin/23122027 -harris images/input/test.jpg images/output/harris_basic.jpg` |
| `-optHarris` | Phát hiện corner bằng thuật toán Harris đã tối ưu | `./bin/23122027 -optHarris images/input/test.jpg images/output/harris_opt.jpg` |

*(Chương trình sẽ in thời gian thực thi ra terminal. Sau khi hoàn tất, một cửa sổ GUI hiển thị ảnh kết quả sẽ hiện lên. Nhấn phím bất kỳ trên cửa sổ ảnh để đóng và lưu kết quả vào đường dẫn output).*

## Chạy kiểm thử tự động

Dự án có cung cấp sẵn kịch bản `run.sh` giúp bạn tự động chạy đối chiếu thời gian thực thi của cả hai phiên bản thuật toán trên bức ảnh mẫu `test.jpg`.

1. Di chuyển vào thư mục chứa mã nguồn:

    ```bash
    cd Sources
    ```

2. Cấp quyền thực thi và chạy script:

    ```bash
    chmod +x run.sh
    ./run.sh
    ```

3. Terminal sẽ hiển thị tiến trình và thời gian chạy. Các ảnh kết quả sẽ được tự động xuất ra thư mục `Sources/images/output/`.
