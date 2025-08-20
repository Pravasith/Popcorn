
#include "Animation.h"
#include "GlobalMacros.h"
#include <numeric>
#include <stdexcept>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
//
// -----------------------------------------------------------------------------
// --- TIME TRAIN STUFF --------------------------------------------------------
//
TimeTrain::TimeTrain(AnimationTrackPtr passengerPtr, double boardStation,
                     double destStation) {
  m_isPsgrAnimTrack = true;
  // TODO: Rest of the thunks fuckery
}

TimeTrain::TimeTrain(AnimationPropertyPtr passengerPtr, CurvePtr curvePtr,
                     double boardStation, double destStation) {
  std::visit(
      [&](auto *curvePtrVal) {
        std::visit(
            [&](auto *passengerPtrVal) {
              using PassengerValueType =
                  typename std::decay_t<decltype(*passengerPtrVal)>::value_type;
              using CurveValueType =
                  typename std::decay_t<decltype(*curvePtrVal)>::value_type;

              if constexpr (std::is_same_v<PassengerValueType,
                                           CurveValueType>) {
                passengerPtrVal->BindRail(curvePtrVal);

                m.ttExec.psgrPtr = passengerPtrVal;
                m.ttExec.railPtr = curvePtrVal;
                m.ttExec.animateFast_Fptr = &AnimateFast_Curve<CurveValueType>;
                m.ttExec.animateSlow_Fptr = &AnimateSlow_Curve<CurveValueType>;

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

TimeTrain::TimeTrain(AnimationPropertyPtr passengerPtr, SplinePtr splinePtr,
                     double boardStation, double destStation) {
  std::visit(
      [&](auto *splinePtrVal) {
        std::visit(
            [&](auto *passengerPtrVal) {
              using PassengerValueType =
                  typename std::decay_t<decltype(*passengerPtrVal)>::value_type;
              using SplineValueType =
                  typename std::decay_t<decltype(*splinePtrVal)>::value_type;
              if constexpr (std::is_same_v<PassengerValueType,
                                           SplineValueType>) {
                passengerPtrVal->BindRail(splinePtrVal);

                m.ttExec.psgrPtr = passengerPtrVal;
                m.ttExec.railPtr = splinePtrVal;
                m.ttExec.animateFast_Fptr =
                    &AnimateFast_Spline<SplineValueType>;
                m.ttExec.animateSlow_Fptr =
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

//
//
//
// -----------------------------------------------------------------------------
// --- ANIMATION TRACK STUFF ---------------------------------------------------
//

void AnimationTrack::SweepLineSetUp() {
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

#define RESET_SWEEP_LINE_SETS                                                  \
  do {                                                                         \
    m_startSlider = m_endSlider = 0;                                           \
    m_active.clear();                                                          \
    std::fill(m_locInActive.begin(), m_locInActive.end(), -1);                 \
  } while (0);
//
// --- Play --------------------------------------------------------------------
void AnimationTrack::Play(double durationInSecs) {
  m_elapsedTimeS = 0.0;
  m_durationS = durationInSecs;
  m_isPlaying = true;
  RESET_SWEEP_LINE_SETS
}
void AnimationTrack::Play(double durationInSecs,
                          std::function<void(AnimationTrack *)> onFinishCb) {
  m_elapsedTimeS = 0.0;
  m_durationS = durationInSecs;
  m_isPlaying = true;
  m_onPlayFinishCb = std::move(onFinishCb);
  RESET_SWEEP_LINE_SETS
}
#undef RESET_SWEEP_LINE_SETS

#define RESET_PROPS                                                            \
  do {                                                                         \
    m_isPlaying = false;                                                       \
    m_elapsedTimeS = 0.0;                                                      \
    m_durationS = 0.0;                                                         \
    m_onPlayFinishCb = nullptr;                                                \
  } while (0);

void AnimationTrack::Animate(double t, bool useSlowAlgo) {
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
    double u = double((t - train.board) * train.invLen);

    // clamp to 0.0 -> 1.0
    if (u < 0.0)
      u = 0.0;
    else if (u > 1.0)
      u = 1.0;

    if (useSlowAlgo) {
      train.Animate_Slow(u);
    } else
      train.Animate_Fast(u);
  }
}

//
// --- Update ------------------------------------------------------------------
bool AnimationTrack::OnUpdate(TimeEvent &e) {
  if ((m_elapsedTimeS += e.GetDeltaS()) < m_durationS) {
    double t = GetNormalizedElapsedSecs();

    Animate(t); // default fast
    // Animate(t, true); // for slow
  } else {
    if (m_onPlayFinishCb) {
      (m_onPlayFinishCb)(this);
    }
    RESET_PROPS
  }
  return true;
};
#undef RESET_PROPS

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
