#pragma once

#include "pp/frame_advance.h"

namespace PP {
namespace GlobalSettings {

extern int maxOsdLimit;
extern int maxLedgedashVizFrames;
extern bool enableFrameAdvance;
extern FrameAdvanceButton frameAdvanceButton;
extern int frameAdvanceRepeatDelayFrames;
extern bool enableInvisibleTechs;
extern int shieldActionabilityTolerance;

}} // namespace PP::GlobalSettings