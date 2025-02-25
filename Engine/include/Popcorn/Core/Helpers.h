#pragma once

#include "GlobalMacros.h"
#include <cstddef>
#include <cstdint>

#ifdef PC_DEBUG
#include <iostream>
#include <sstream>
#include <string>
#endif

ENGINE_NAMESPACE_BEGIN
constexpr std::size_t shift_l(std::size_t n) { return 1 << n; }

enum TagType { Constr, Destr, Print };

using byte_t = uint8_t;

extern int PC_print_lvl;

static void PC_Print(const std::string &msg, const TagType tag,
                     const std::string &className) {

  if (PC_print_lvl < 0) {
    std::cout << "MISSING CONSTRUCTOR MESSAGE" << '\n';
    return;
  };

  if (tag == TagType::Constr) {
    PC_print_lvl++;
  } else if (tag == TagType::Destr)
    PC_print_lvl--;

  std::stringstream ss;
  auto lvl = PC_print_lvl < 10 ? " " + std::to_string(PC_print_lvl)
                               : std::to_string(PC_print_lvl);
  if (tag == TagType::Print) {
    lvl = "  ";
  };

  ss << lvl << " ";

  for (int i = 0; i < PC_print_lvl; ++i) {
    ss << "| ";
  }

  ss << "| " << className << ": " << msg << '\n';
  std::cout << ss.str();
};

#if defined(__GNUC__) || defined(__clang__)
#define PC_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define PC_FUNC_NAME __FUNCSIG__
#else
#define PC_FUNC_NAME __func__
#endif

ENGINE_NAMESPACE_END
