#pragma once
#include <cstdint>

namespace flatland {

struct Viewport {
  int32_t width = 480;
  int32_t height = 360;
};

constexpr double pixelsPerBlock = 16.0;

constexpr double viewportOriginX = 8.0;
constexpr double viewportOriginY = 101.5;
constexpr double viewportPlaneZ = 8.0;

inline double viewportWorldHalfWidth(const Viewport &viewport) { return (viewport.width / pixelsPerBlock) / 2.0; }
inline double viewportWorldHalfHeight(const Viewport &viewport) { return (viewport.height / pixelsPerBlock) / 2.0; }

inline double viewportPixelToWorldX(const Viewport &viewport, double px) { return viewportOriginX - viewportWorldHalfWidth(viewport) + px / pixelsPerBlock; }
inline double viewportPixelToWorldY(const Viewport &viewport, double py) { return viewportOriginY + viewportWorldHalfHeight(viewport) - py / pixelsPerBlock; }

} // namespace flatland
