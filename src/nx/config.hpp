#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iosfwd>

namespace hp2::nx {
struct UserSettings {
    int language{-1}; // -1 follows the game/profile until explicitly selected.
    int frameLimit{30}; // 0 = uncapped, otherwise 1..60.
    bool showFps{};
};
inline UserSettings userSettings;
inline int PreferredLanguage(int profileLanguage) noexcept {
    return userSettings.language>=0 ? userSettings.language:profileLanguage;
}
inline bool gameplayPresentation{};
void LoadUserSettings(const std::filesystem::path& directory,std::ostream& log);
bool SaveUserSettings() noexcept;
void CycleFrameLimit(int direction) noexcept;

}
