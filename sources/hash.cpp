// Copyright 2021 Your Name <your_email>

#include "hash.hpp"

Hash::Hash(std::string path) {
  json_path = std::move(path);
  Number_of_threads = std::thread::hardware_concurrency();//потоки по умолчанию
  srand(time(nullptr));
}

Hash::Hash(std::string path, size_t amount) {
  json_path = std::move(path);
  auto max = std::thread::hardware_concurrency();
  srand(time(nullptr));
  if (amount <= 0 || amount > max) {
    throw std::invalid_argument("INCORRECT NUMBER OF THREADS");
  }
  Number_of_threads = amount;
}

void Hash::start() {
  signal(SIGINT, signalHandler);  // ctrl+c

  directory_work();  // clear or create /logs and move into it
  logs();            // set logs

  std::vector<std::thread> threads;
  for (size_t i = 0; i < Number_of_threads; ++i) { //создаем потоки и запускаем
    std::thread thr(&Hash::check_hash, this, i);
    threads.emplace_back(std::move(thr));
  }
  for (auto& thr : threads) {
    thr.join();
  }
}

void Hash::signalHandler(int signum) {
  flag = 0;  // stop threads
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  std::cout << "SAVING DATA TO JSON" << std::endl;
  std::cout << "PLEASE, WAIT" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(4100));
  std::cout << "FINISHED" << std::endl;
  exit(signum);
}

std::string Hash::data_generator() { //создание случайных входных данных
  std::string data;
  std::random_device rd;
  std::mt19937 mersenne(rd());
  for (size_t i = 0; i < data_len; ++i) {
    if (mersenne() % 2) {
      data.push_back(mersenne() % 26 + 65);
    } else {
      data.push_back(mersenne() % 10 + 48);
    }
  }
  return data;
}

void Hash::directory_work() { //проверка директории
  auto path = boost::filesystem::path("../");
  boost::filesystem::current_path(path);

  if (boost::filesystem::exists("logs")) {
    boost::filesystem::current_path("logs");
    boost::filesystem::directory_iterator begin(
        boost::filesystem::current_path());
    boost::filesystem::directory_iterator end;
    for (auto it = begin; it != end; ++it) {
      boost::filesystem::remove(*it);
    }
  } else {
    boost::filesystem::path dir = "logs";
    boost::filesystem::create_directory(dir);
    boost::filesystem::current_path("logs");
  }
}

void Hash::check_hash(int pos) {
  json js_data;

  while (flag) { //пока пользователь не нажал ctrl + c
    std::string data = data_generator(); //генерируем дату
    const std::string hash = picosha2::hash256_hex_string(data);

    const auto time = std::chrono::system_clock::now();

    if (hash.substr(60, 4) == "0000") { //проверяем дату
      BOOST_LOG_TRIVIAL(info) << data << " " << hash << std::endl;
    } else {
      BOOST_LOG_TRIVIAL(trace) << data << " " << hash << std::endl;
    }

    json temp; //создаем json файл
    temp["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                            time.time_since_epoch())
                            .count();
    temp["hash"] = hash;
    temp["data"] = data;
    js_data.push_back(temp);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds((pos + 1) * 500)); //после флага задержка, чтобы остальные потоки поочередно записались
  std::cout << pos + 1 << " THREAD SAVED" << std::endl;

  if (pos == 0) {
    file.open(json_path, std::ios_base::trunc);
  } else {
    file.open(json_path, std::ios_base::app);
  }

  file << std::setw(4) << js_data << std::endl;
  file.close();
}

void Hash::logs() { //логирование в файл
  logging::add_common_attributes();
  logging::add_file_log(
      logging::keywords::file_name = log_info_filename,
      logging::keywords::target_file_name = log_info_filename,
      logging::keywords::rotation_size = log_info_file_size,
      logging::keywords::time_based_rotation =
          logging::sinks::file::rotation_at_time_point(0, 0, 0),
      logging::keywords::filter =
          logging::trivial::severity >= logging::trivial::info,
      logging::keywords::format = "[%TimeStamp%]:   [%ThreadID%]   %Message%");

  logging::add_common_attributes();
  logging::add_file_log(
      logging::keywords::file_name = log_trace_filename,
      logging::keywords::target_file_name = log_trace_filename,
      logging::keywords::rotation_size = log_trace_file_size,
      logging::keywords::time_based_rotation =
          logging::sinks::file::rotation_at_time_point(0, 0, 0),
      logging::keywords::filter =
          logging::trivial::severity <= logging::trivial::trace,
      logging::keywords::format = "[%TimeStamp%]:  [%ThreadID%]   %Message%");

  logging::add_console_log(std::cout, logging::keywords::format =
                                          "[%TimeStamp%]: [%ThreadID%]  "
                                          "[%Severity%]: %Message%");
}
