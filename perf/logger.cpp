#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string_view>
#include <system_error>

namespace {

  class Logger {
    public:
      explicit Logger(std::string_view path) {
        const int MODE_BITS = O_CREAT | O_APPEND | O_WRONLY;
        const int PROTECTION_BITS = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
        fd = ::open(path.data(), MODE_BITS, PROTECTION_BITS);
        if (fd == -1) {
          throw std::system_error(errno, std::system_category(), "open");
        }
      }
    
      ~Logger() {
        _flush();
        ::close(fd);
      }
    
      void write(const std::string& msg) {
        buffer += msg;
        if (buffer.size() >= BUFFER_SIZE) {
          _flush();
        }
      }
    
    private:
      int fd = -1;
      std::string buffer;
      static constexpr size_t BUFFER_SIZE = 2048;


      void _flush() {
        if (!buffer.empty()) {
          if (::write(fd, buffer.data(), buffer.size()) != static_cast<ssize_t>(buffer.size())) {
            throw std::system_error(errno, std::system_category(), "write");
          }
          buffer.clear();
        }
      }
    };
    

void bm_logger(benchmark::State& state) {
  static Logger logger("/tmp/benchmark_logger");

  for (auto _ : state) {
    logger.write("test message\n");
  }
}

} // namespace

BENCHMARK(bm_logger)->Threads(1);

BENCHMARK_MAIN();
