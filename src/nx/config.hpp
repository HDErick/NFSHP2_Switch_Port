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
    bool adaptiveDrawDistance{true};
    bool testLowQuality{true}; // Temporary fixed 0.2 scene-quality test baseline.
};
inline UserSettings userSettings;
inline int PreferredLanguage(int profileLanguage) noexcept {
    return userSettings.language>=0 ? userSettings.language:profileLanguage;
}
inline bool gameplayPresentation{};
inline bool adaptiveGameplay{};
void LoadUserSettings(const std::filesystem::path& directory,std::ostream& log);
bool SaveUserSettings() noexcept;
void CycleFrameLimit(int direction) noexcept;

// One-second windows. Severe load can escalate during a recovery hold;
// quality only increases after five stable windows and a minimum dwell.
class AdaptiveDrawDistance {
public:
    void Reset() noexcept {elapsed_=0;frames_=low_=high_=hold_=tier_=0;}
    float Scale() const noexcept {return tier_==2 ? 0.2f:tier_==1 ? 0.4f:0.6f;}
    bool Emergency() const noexcept {return tier_==2;}
    bool Observe(double seconds,int cap) noexcept {
        if(!std::isfinite(seconds) || seconds<=0)return false;
        elapsed_+=seconds;++frames_;
        if(elapsed_<1.0)return false;
        const double fps=frames_/elapsed_;
        elapsed_=0;frames_=0;
        const double target=cap>0 ? std::min(30,cap):30;
        low_=fps<target*(28.0/30.0)-1.0e-6 ? low_+1:0;
        if(tier_<2 && fps<=target*(23.0/30.0)+1.0e-6) {
            tier_=2;hold_=5;low_=high_=0;return true;
        }
        if(tier_==0 && low_>=2) {
            tier_=1;hold_=5;low_=high_=0;return true;
        }
        if(hold_)--hold_;
        const double recovery=target*(tier_==2 ? 28.0/30.0:29.5/30.0);
        high_=fps+1.0e-6>=recovery ? high_+1:0;
        if(tier_ && !hold_ && high_>=5) {
            --tier_;hold_=5;low_=high_=0;return true;
        }
        return false;
    }
private:
    double elapsed_{};
    unsigned frames_{},low_{},high_{},hold_{},tier_{};
};
}
