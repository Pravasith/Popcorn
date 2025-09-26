#pragma once

#include "CurveDefs.h"
#include "GlobalMacros.h"
#include "SplineFactory.h"
#include "Splines.h"
#include <cstddef>
#include <fstream>
#include <glm/fwd.hpp>
#include <json.hpp>
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

[[nodiscard]] static const Spline<glm::vec3> *
PC_MakeBezierSplineFromBlenderJson(const std::string &filename) {
  using json = nlohmann::json;
  std::ifstream ifs(filename);
  if (!ifs) {
    throw std::runtime_error("Failed to open JSON file: " + filename);
    return nullptr;
  }

  json j;
  ifs >> j;

  std::vector<CurveInfoBezierForm<glm::vec3>> bezierCurveInfos;
  std::vector<double> keyframeTs;

  bezierCurveInfos.reserve(j.size());
  keyframeTs.reserve(j.size());

  for (size_t i = 0; i < j.size(); ++i) {
    const json &seg = j[i];

    CurveInfoBezierForm<glm::vec3> cInfo;

    cInfo.b0 = glm::vec3{(float)seg["p0"][0].get<double>(),
                         (float)seg["p0"][1].get<double>(),
                         (float)seg["p0"][2].get<double>()};

    cInfo.b1 = glm::vec3{(float)seg["h0"][0].get<double>(),
                         (float)seg["h0"][1].get<double>(),
                         (float)seg["h0"][2].get<double>()};

    cInfo.b2 = glm::vec3{(float)seg["h1"][0].get<double>(),
                         (float)seg["h1"][1].get<double>(),
                         (float)seg["h1"][2].get<double>()};

    cInfo.b3 = glm::vec3{(float)seg["p1"][0].get<double>(),
                         (float)seg["p1"][1].get<double>(),
                         (float)seg["p1"][2].get<double>()};

    bezierCurveInfos.push_back(cInfo);
    keyframeTs.push_back((double)(i) / (double)(j.size()));
  }

  return SplineFactory::Get()->MakeSpline(bezierCurveInfos, keyframeTs, {});
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
