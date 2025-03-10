#include <benchmark/benchmark.h>

#include <random>
#include <vector>

namespace {

using Pixel = std::vector<int>;

struct Picture {
  size_t height;
  size_t width;
  std::vector<int> red;
  std::vector<int> green;
  std::vector<int> blue;

  Picture(size_t h, size_t w)
      : height(h)
      , width(w)
      , red(h * w)
      , green(h * w)
      , blue(h * w) {}
};

Picture random_picture() {
  static constexpr size_t HEIGHT = 600;
  static constexpr size_t WIDTH = 800;

  std::mt19937 rng(std::mt19937::default_seed);
  std::uniform_int_distribution<int> color_dist(0, 255);

  Picture picture(HEIGHT, WIDTH);

  for (size_t i = 0; i < HEIGHT; i++) {
    for (size_t j = 0; j < WIDTH; j++) {
      size_t index = i * WIDTH + j;
      picture.red[index] = color_dist(rng);
      picture.green[index] = color_dist(rng);
      picture.blue[index] = color_dist(rng);
    }
  }
  return picture;
}

Picture scale_down(const Picture& picture) {
  size_t scaled_height = picture.height / 2;
  size_t scaled_width = picture.width / 2;

  Picture scaled(scaled_height, scaled_width);

  for (size_t i = 0; i < scaled_height; i++) {
    for (size_t j = 0; j < scaled_width; j++) {
      size_t original_i = i * 2;
      size_t original_j = j * 2;

      // idxs for initial image
      size_t idx1 = original_i * picture.width + original_j;
      size_t idx2 = (original_i + 1) * picture.width + original_j;
      size_t idx3 = original_i * picture.width + (original_j + 1);
      size_t idx4 = (original_i + 1) * picture.width + (original_j + 1);

      int avg_red = (picture.red[idx1] + picture.red[idx2] + picture.red[idx3] + picture.red[idx4]) / 4;

      int avg_green = (picture.green[idx1] + picture.green[idx2] + picture.green[idx3] + picture.green[idx4]) / 4;

      int avg_blue = (picture.blue[idx1] + picture.blue[idx2] + picture.blue[idx3] + picture.blue[idx4]) / 4;

      // Idxs for new image
      size_t scaled_idx = i * scaled_width + j;
      scaled.red[scaled_idx] = avg_red;
      scaled.green[scaled_idx] = avg_green;
      scaled.blue[scaled_idx] = avg_blue;
    }
  }

  return scaled;
}

void bm_scale_components(benchmark::State& state) {
  const auto picture = random_picture();

  for (auto _ : state) {
    const auto copy = picture;
    benchmark::DoNotOptimize(scale_down(copy));
  }
}

} // namespace

BENCHMARK(bm_scale_components)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
