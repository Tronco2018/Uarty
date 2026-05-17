#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

std::string port;
baud_t baud;

baud_t parse_baud(const std::string& s) {
    if (s == "9600") return B9600;
    if (s == "19200") return B19200;
    if (s == "38400") return B38400;
    if (s == "57600") return B57600;
    if (s == "115200") return B115200;
    if (s == "230400") return B230400;

    throw std::runtime_error("unsupported baud rate");
}

int check_args(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <port> <baud>\n", argv[0]);
        return 1;
    }

    port = argv[1];
    baud = parse_baud(argv[2]);
    
    return 0;
}

int main(int argc, char *argv[]) {
    if (check_args(argc, argv)) {
        return 1;
    }

    int fd = open(port.c_str(), O_RDWR | O_NOCTTY);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    termios tty{};
    tcgetattr(fd, &tty);

    // Set I/O baud rate
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    // read() behaiviour
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    tcsetattr(fd, TCSANOW, &tty);

    char buf[256];
    char buf_in[256];

    while (true) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        FD_SET(STDIN_FILENO, &set);

        int maxfd = std::max(fd, STDIN_FILENO);

        // Reads both fd without blocking 
        int ret = select(maxfd + 1, &set, nullptr, nullptr, nullptr);

        if (ret < 0) break;

        // Output to screen
        if (FD_ISSET(fd, &set)) {
            int n = read(fd, buf, sizeof(buf));
            if (n > 0) {
                std::cout.write(buf, n);
                std::cout.flush();
            }
        }

        // Input to serial
        if (FD_ISSET(STDIN_FILENO, &set)) {
            int i = read(STDIN_FILENO, buf_in, sizeof(buf_in));
            if (i > 0) {
                write(fd, buf_in, i);
            }
        }


    }
}