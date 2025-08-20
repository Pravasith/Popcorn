#pragma once

#include "AnimationProperty.h"
#include "CurveDefs.h"
#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Splines.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using AnimationPropertyPtr =
    std::variant<AnimationProperty<float> *, AnimationProperty<double> *,
                 AnimationProperty<glm::vec2> *, AnimationProperty<glm::vec3> *,
                 AnimationProperty<glm::vec4> *>;

using CurvePtr =
    std::variant<const Curve<float> *, const Curve<double> *,
                 const Curve<glm::vec2> *, const Curve<glm::vec3> *,
                 const Curve<glm::vec4> *>;
using SplinePtr =
    std::variant<const Spline<float> *, const Spline<double> *,
                 const Spline<glm::vec2> *, const Spline<glm::vec3> *,
                 const Spline<glm::vec4> *>;

class TimeTrain {
public:
  TimeTrain(AnimationPropertyPtr passengerPtr, CurvePtr curvePtr,
            double boardStation, double destStation) {

    std::visit(
        [&](auto *curvePtrVal) {
          std::visit(
              [&](auto *passengerPtrVal) {
                using PassengerValueType = typename std::decay_t<
                    decltype(*passengerPtrVal)>::value_type;
                using CurveValueType =
                    typename std::decay_t<decltype(*curvePtrVal)>::value_type;

                if constexpr (std::is_same_v<PassengerValueType,
                                             CurveValueType>) {
                  passengerPtrVal->BindRail(curvePtrVal);

                  m.trainExec.animPropPtr = passengerPtrVal;
                  m.trainExec.crvSplPtr = curvePtrVal;
                  m.trainExec.animateFast_Fptr =
                      &AnimateFast_Curve<CurveValueType>;
                  m.trainExec.animateSlow_Fptr =
                      &AnimateSlow_Curve<CurveValueType>;

                  m_passengerPtr = passengerPtr;
                  board = boardStation;
                  dest = destStation;
                  SetInvLen();
                } else {
                  throw std::runtime_error(
                      "trying to bind a Curve to an AnimationProperty that are "
                      "of different types");
                }
              },
              passengerPtr);
        },
        curvePtr);
  }
  TimeTrain(AnimationPropertyPtr passengerPtr, SplinePtr splinePtr,
            double boardStation, double destStation) {
    std::visit(
        [&](auto *splinePtrVal) {
          std::visit(
              [&](auto *passengerPtrVal) {
                using PassengerValueType = typename std::decay_t<
                    decltype(*passengerPtrVal)>::value_type;
                using SplineValueType =
                    typename std::decay_t<decltype(*splinePtrVal)>::value_type;
                if constexpr (std::is_same_v<PassengerValueType,
                                             SplineValueType>) {
                  passengerPtrVal->BindRail(splinePtrVal);

                  m.trainExec.animPropPtr = passengerPtrVal;
                  m.trainExec.crvSplPtr = splinePtrVal;
                  m.trainExec.animateFast_Fptr =
                      &AnimateFast_Spline<SplineValueType>;
                  m.trainExec.animateSlow_Fptr =
                      &AnimateSlow_Spline<SplineValueType>;

                  m_passengerPtr = passengerPtr;
                  board = boardStation;
                  dest = destStation;
                  SetInvLen();
                } else {
                  throw std::runtime_error("trying to bind a Spline to an "
                                           "AnimationProperty that are "
                                           "of different types");
                }
              },
              passengerPtr);
        },
        splinePtr);
  }
  TimeTrain() = delete;

  void AnimateFast(float u) {
    m.trainExec.animateFast_Fptr(m.trainExec.animPropPtr, m.trainExec.crvSplPtr,
                                 u);
  }

  void AnimateSlow(double u) {
    m.trainExec.animateSlow_Fptr(m.trainExec.animPropPtr, m.trainExec.crvSplPtr,
                                 u);
  }

private:
  inline void SetInvLen() {
    const double len = dest - board;
    // Treat tiny spans as instant (avoid 1/very-small blowing up)
    invLen = (len > PC_EPS_D) ? 1.0 / len : 0.0;
  }

  // Animate curve or spline thunks
  template <CurveFormType T>
  static void AnimateFast_Curve(void *animationPropertyPtr,
                                const void *curvePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Fast(u));
  };
  template <CurveFormType T>
  static void AnimateSlow_Curve(void *animationPropertyPtr,
                                const void *curvePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Slow(u));
  };
  template <CurveFormType T>
  static void AnimateFast_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Fast(u));
  };
  template <CurveFormType T>
  static void AnimateSlow_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Slow(u));
  };

public:
  double board;
  double dest;
  double invLen;

private:
  AnimationPropertyPtr m_passengerPtr;

  // --- perf stuff -----------------------------------------------------------
  struct TrainExec {
    void *animPropPtr = nullptr;
    const void *crvSplPtr = nullptr;
    void (*animateFast_Fptr)(void *animationPropertyPtr,
                             const void *curveOrSplinePtr, float u) = nullptr;
    void (*animateSlow_Fptr)(void *animationPropertyPtr,
                             const void *curveOrSplinePtr, double u) = nullptr;
  };

  struct {
    TrainExec trainExec;
  } m;
};

class AnimationTrack : public Subscriber {
public:
  AnimationTrack(std::vector<TimeTrain> &&timetrains)
      : m_timeTrains(std::move(timetrains)) {
    SweepLineSetUp();
  }
  ~AnimationTrack() = default;

  void Insert(TimeTrain timeTrain) {
    m_timeTrains.push_back(timeTrain);
    SweepLineSetUp();
  }

#define RESET_SWEEP_LINE_SETS                                                  \
  m_startSlider = m_endSlider = 0;                                             \
  m_active.clear();                                                            \
  std::fill(m_locInActive.begin(), m_locInActive.end(), -1);

  void Play(double durationInSecs) {
    m_elapsedTimeS = 0.0;
    m_durationS = durationInSecs;
    m_isPlaying = true;
    RESET_SWEEP_LINE_SETS
  }

  void Play(double durationInSecs,
            std::function<void(AnimationTrack *)> onFinishCb) {
    m_elapsedTimeS = 0.0;
    m_durationS = durationInSecs;
    m_isPlaying = true;
    m_onPlayFinishCb = std::move(onFinishCb);
    RESET_SWEEP_LINE_SETS
  }
#undef RESET_SWEEP_LINE_SETS

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }
    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  }

private:
  void SweepLineSetUp() {
    const size_t N = m_timeTrains.size();
    m_starts.resize(N);
    m_ends.resize(N);

    // Note: iota writes a sequence STARTING from 0u (like 0u, 1u, 2u, ... Nu)
    std::iota(m_starts.begin(), m_starts.end(), 0u);
    std::iota(m_ends.begin(), m_ends.end(), 0u);

    auto byBoardCb = [&](uint32_t a, uint32_t b) {
      return m_timeTrains[a].board < m_timeTrains[b].board;
    };
    auto byDestCb = [&](uint32_t a, uint32_t b) {
      return m_timeTrains[a].dest < m_timeTrains[b].dest;
    };

    std::sort(m_starts.begin(), m_starts.end(), byBoardCb);
    std::sort(m_ends.begin(), m_ends.end(), byDestCb);

    m_active.clear();
    m_active.reserve(N);
    m_locInActive.assign(N, -1);

    m_startSlider = m_endSlider = 0;
  }

  inline double GetNormalizedElapsedSecs() const {
    assert(m_durationS > 0);
    return std::clamp(m_elapsedTimeS / m_durationS, 0.0, 1.0);
  }

private:
#define RESET_PROPS                                                            \
  do {                                                                         \
    m_isPlaying = false;                                                       \
    m_elapsedTimeS = 0.0;                                                      \
    m_durationS = 0.0;                                                         \
    m_onPlayFinishCb = nullptr;                                                \
  } while (0);

  bool OnUpdate(TimeEvent &e) {
    if ((m_elapsedTimeS += e.GetDeltaS()) < m_durationS) {
      double t = GetNormalizedElapsedSecs();

      // activate timetrains from m_starts
      while (m_startSlider < m_starts.size() &&
             m_timeTrains[m_starts[m_startSlider]].board <= t) {
        auto tt_idx = m_starts[m_startSlider++];

        const auto &tr = m_timeTrains[tt_idx];
        assert(tr.dest > tr.board);

        m_locInActive[tt_idx] = (int32_t)m_active.size();
        m_active.push_back(tt_idx);
      }

      // deactivate timetrains from m_ends
      while (m_endSlider < m_ends.size() &&
             m_timeTrains[m_ends[m_endSlider]].dest <= t) {
        auto tt_idx = m_ends[m_endSlider++];

        if (m_locInActive[tt_idx] >= 0) {
          // element exists in m_active
          auto back = m_active.back();
          m_active[m_locInActive[tt_idx]] = back; // swap with back
          m_locInActive[back] =
              m_locInActive[tt_idx]; // update prior-"back" new locInActive
          m_active.pop_back();       // delete
          m_locInActive[tt_idx] = -1;
        }
      }

      // animate active timetrains
      for (uint32_t idx : m_active) {
        auto &train = m_timeTrains[idx];
        float u = float((t - train.board) * train.invLen);

        // clamp to 0.0 -> 1.0
        if (u < 0.f)
          u = 0.f;
        else if (u > 1.f)
          u = 1.f;
        train.AnimateFast(u);

        // double u = double((t - train.board) * train.invLen);
        //
        // // clamp to 0.0 -> 1.0
        // if (u < 0.f)
        //   u = 0.f;
        // else if (u > 1.f)
        //   u = 1.f;
        // train.AnimateSlow(u);
      }

    } else {
      if (m_onPlayFinishCb) {
        (m_onPlayFinishCb)(this);
      }
      RESET_PROPS
    }
    return true;
  };
#undef RESET_PROPS

private:
  bool m_isPlaying = false;
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;

  std::function<void(AnimationTrack *)> m_onPlayFinishCb;

private:
  std::vector<TimeTrain> m_timeTrains;

  size_t m_startSlider = 0;
  size_t m_endSlider = 0;

  std::vector<uint32_t> m_starts;
  std::vector<uint32_t> m_ends;
  std::vector<uint32_t>
      m_active; // active trains list during a "t" of the current frame
  std::vector<int32_t> m_locInActive; // for swap-deleting the deactivated ones
                                      // from m_active queue

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
