#ifndef NFSHP2_NATIVE_RENDER_SETTINGS_HPP
#define NFSHP2_NATIVE_RENDER_SETTINGS_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace hp2 {
struct NativeRenderSettings {
    float drawDistanceScale{1.0f};       // World views relative to authored visibility.
    float mirrorDistanceScale{1.0f};     // Rear view: 0.25 minimum on Switch, 0.5 on PC.
};
inline NativeRenderSettings validateNativeRenderSettings(NativeRenderSettings value) noexcept
{
#ifdef __SWITCH__
    constexpr float minimumDrawDistance=0.2f;
    constexpr float minimumMirrorDistance=0.25f;
#else
    constexpr float minimumDrawDistance=1.0f;
    constexpr float minimumMirrorDistance=0.5f;
#endif
    value.drawDistanceScale=std::isfinite(value.drawDistanceScale) ?
        std::clamp(value.drawDistanceScale,minimumDrawDistance,4.0f):1.0f;
    value.mirrorDistanceScale=std::isfinite(value.mirrorDistanceScale) ?
        std::clamp(value.mirrorDistanceScale,minimumMirrorDistance,4.0f):1.0f;
    return value;
}

// Configured on the application thread before a frame is built. Gather workers
// borrow these immutable settings for that frame; launcher edits apply at startup.
inline NativeRenderSettings nativeRenderSettingsStorage{};
// Application-thread policy, immutable while scene capture workers run.
inline bool nativeEmergencySceneQuality{};
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
