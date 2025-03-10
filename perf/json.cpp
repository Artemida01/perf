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

// DO NOT change this struct
struct Message {
  int64_t id;
  std::string subject;
  std::string body;

  User from;
  User to;
};

std::string to_json(const User& user) {
  std::string result;
  result.reserve(128);
  result.append("{\"id\":")
      .append(std::to_string(user.id))
      .append(",\"username\":\"")
      .append(user.username)
      .append("\"}");
  return result;
}

// DO NOT change this function's signature
std::string to_json(const Message& msg) {
  std::string result;
  result.reserve(256);

  result.append("{\"id\":")
      .append(std::to_string(msg.id))
      .append(",\"subject\":\"")
      .append(msg.subject)
      .append("\",\"body\":\"")
      .append(msg.body)
      .append("\",\"from\":")
      .append(to_json(msg.from))
      .append(",\"to\":")
      .append(to_json(msg.to))
      .append("}");
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
