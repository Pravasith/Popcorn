#pragma once

#include <fstream>
#include <iostream>

static void write_log(const char *msg) {
  std::cout << msg << std::endl;
  std::ofstream logs;
  logs.open("common_logs.txt", std::ofstream::app | std::ofstream::out);
  logs << msg;
  logs.close();
}
