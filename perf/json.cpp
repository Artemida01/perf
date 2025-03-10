#include <benchmark/benchmark.h>

#include <sstream>
#include <string>

// DO NOT change this struct
struct User {
  int64_t id;
  std::string username;
};

std::ostream& operator<<(std::ostream& out, const User& user) {
  out << "{" << "\"id\":" << user.id << "," << "\"username\":\"" << user.username << "\"" << "}";
  return out;
}

template <typename... Args>
void add_multy(std::string& answer, Args&&... args) {
  (answer += ... += std::forward<Args>(args));
}

std::string to_json(const User& user) {
  std::string result;
  result.reserve(128);

  add_multy(result, "{\"id\":", std::to_string(user.id), ",\"username\":\"", user.username, "\"}");
  return result;
}

// DO NOT change this struct
struct Message {
  int64_t id;
  std::string subject;
  std::string body;

  User from;
  User to;
};

// DO NOT change this function's signature
std::string to_json(const Message& msg) {
  std::string result;
  result.reserve(256);

  add_multy(
      result,
      "{\"id\":",
      std::to_string(msg.id),
      ",\"subject\":\"",
      msg.subject,
      "\",\"body\":\"",
      msg.body,
      "\",\"from\":",
      to_json(msg.from),
      ",\"to\":",
      to_json(msg.to),
      "}"
  );

  return result;
}

namespace {

void bm_message_to_json(benchmark::State& state) {
  Message msg{
      .id = 1000,
      .subject = "About modules",
      .body = "So, when is that 'modules' proposal coming?",
      .from = User{.id = 12345, .username = "Herb Sutter"},
      .to = User{.id = 1, .username = "Bjarne Stroustrup"}
  };

  for (auto _ : state) {
    benchmark::DoNotOptimize(to_json(msg));
  }

  auto json = to_json(msg);
  std::string expected =
      R"({"id":1000,"subject":"About modules","body":"So, when is that 'modules' proposal coming?","from":{"id":12345,"username":"Herb Sutter"},"to":{"id":1,"username":"Bjarne Stroustrup"}})";
  if (json != expected) {
    state.SkipWithError("Wrong output");
  }
}

} // namespace

BENCHMARK(bm_message_to_json);

BENCHMARK_MAIN();
