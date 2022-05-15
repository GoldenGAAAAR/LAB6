// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_HASH_HPP_
#define INCLUDE_HASH_HPP_

#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <random>
#include <vector>

#include "boost/filesystem.hpp"
#include "nlohmann/json.hpp"
#include "../third-party/PicoSHA2/picosha2.h"

namespace logging = boost::log;
using json = nlohmann::json;
const unsigned log_info_file_size = 1024 * 1024;        // 1 Mb
const unsigned log_trace_file_size = 50 * 1024 * 1024;  // 50 Mb
const char log_info_filename[] = "info_%N.log";
const char log_trace_filename[] = "trace_%N.log";

#define data_len 10
static volatile int flag = 1;

class Hash {
 private:
  size_t Number_of_threads;
  std::ofstream file;
  std::string json_path;

 public:
  explicit Hash(std::string path);

  Hash(std::string path, size_t amount);

  void start();

 private:
  static void signalHandler(int signum);

  static std::string data_generator();

  static void directory_work();

  void check_hash(int pos);

  void logs();
};

#endif  // INCLUDE_HASH_HPP_