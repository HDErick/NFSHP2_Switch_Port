#ifndef NFSHP2_NATIVE_RENDER_SETTINGS_HPP
#define NFSHP2_NATIVE_RENDER_SETTINGS_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace hp2 {
struct NativeRenderSettings {
#ifdef __SWITCH__
    float drawDistanceScale{0.3f};
    float mirrorDistanceScale{0.5f};
#else
    float drawDistanceScale{1.0f};
    float mirrorDistanceScale{1.0f};
#endif
};
inline NativeRenderSettings validateNativeRenderSettings(NativeRenderSettings value) noexcept
{
#ifdef __SWITCH__
    // The Switch port uses one fixed scene profile, including legacy configs.
    value.drawDistanceScale=0.3f;value.mirrorDistanceScale=0.5f;
#else
    value.drawDistanceScale=std::isfinite(value.drawDistanceScale) ?
        std::clamp(value.drawDistanceScale,1.0f,4.0f):1.0f;
    value.mirrorDistanceScale=std::isfinite(value.mirrorDistanceScale) ?
        std::clamp(value.mirrorDistanceScale,0.5f,4.0f):1.0f;
#endif
    return value;
}

// Configured on the application thread before a frame is built. Gather workers
// borrow these immutable settings for that frame; launcher edits apply at startup.
inline NativeRenderSettings nativeRenderSettingsStorage{};
inline void configureNativeRenderSettings(const NativeRenderSettings& value) noexcept
{
    nativeRenderSettingsStorage=validateNativeRenderSettings(value);
}
inline const NativeRenderSettings& nativeRenderSettings() noexcept
{
    return nativeRenderSettingsStorage;
}
inline float nativeViewDistanceScale(std::int32_t viewId) noexcept
{
    return viewId==3 ? nativeRenderSettings().mirrorDistanceScale :
        viewId==1 || viewId==2 ? nativeRenderSettings().drawDistanceScale:1.0f;
}

inline constexpr std::uint32_t nativeGpuSceneryClipMaximumStrips=32;

} // namespace hp2
#endif
