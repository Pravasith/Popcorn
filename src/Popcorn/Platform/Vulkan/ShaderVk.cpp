#include "ShaderVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstddef>
#include <fstream>
#include <vector>

ENGINE_NAMESPACE_BEGIN

std::vector<char> ShaderVk::LoadFile(const std::string &fname) {
  // FLAGS MEANING: OPEN THE FILE AND SEEK TO THE END (ate FLAG) AND READ IT AS
  // A BINARY (binary FLAG)

  std::ifstream file(fname, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("FAILED TO OPEN SHADER FILE! " + fname);
  }

  // GET THE CURRENT (get) POINTER POSITION MEASURED IN BYTES WITH tellg().
  // BECAUSE IT IS AT THE END, IT GIVES US THE SIZE FROM begin() (0 IN THIS
  // CASE) TO end()
  size_t fsize = static_cast<size_t>(file.tellg());
  std::vector<char> bfr(fsize);

  // SEEK THE get PTR BACK TO 0
  file.seekg(0);
  file.read(bfr.data(), fsize);

  file.close();

  PC_PRINT(fname << " SIZE" << fsize << " bytes", TagType::Print, "SHADER-VK")

  return bfr;
};

ENGINE_NAMESPACE_END
