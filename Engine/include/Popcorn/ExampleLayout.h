#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

//
// GRAPHICS CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GRAPHICS {
public:
  GRAPHICS() { PC_PRINT("CREATED", TagType::Constr, "GRAPHICS.h") };
  ~GRAPHICS() { PC_PRINT("DESTROYED", TagType::Destr, "GRAPHICS.h") };

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END

// REGULAR CLASS -----------------------------------------------------

ENGINE_NAMESPACE_BEGIN

class EXAMPLE {
public:
  EXAMPLE() { PC_PRINT("CREATED", TagType::Constr, "EXAMPLE.h") };
  ~EXAMPLE() { PC_PRINT("DESTROYED", TagType::Destr, "EXAMPLE.h") };

private:
};

ENGINE_NAMESPACE_END

// SINGLETON CLASS

ENGINE_NAMESPACE_BEGIN

class SINGLETON {
public:
  [[nodiscard]] inline static SINGLETON *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new SINGLETON();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SINGLETON")
    };
  };

private:
  SINGLETON() { PC_PRINT("CREATED", TagType::Constr, "SINGLETON.h") };
  ~SINGLETON() { PC_PRINT("DESTROYED", TagType::Destr, "SINGLETON.h") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  SINGLETON(const SINGLETON &) = delete;
  SINGLETON &operator=(const SINGLETON &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  SINGLETON(SINGLETON &&) = delete;
  SINGLETON &operator=(SINGLETON &&) = delete;

private:
  static SINGLETON *s_instance;
};

ENGINE_NAMESPACE_END
