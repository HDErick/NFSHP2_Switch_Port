#include "config.hpp"

#include <array>
#include <charconv>
#include <fstream>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>
#include <stdexcept>

namespace hp2::nx {
namespace {
std::filesystem::path configPath;
std::ostream* journal{};
UserSettings savedSettings;
bool savedValid{};
std::string_view trim(std::string_view value) {
    const auto first=value.find_first_not_of(" \t\r\n");
    if(first==value.npos)return {};
    return value.substr(first,value.find_last_not_of(" \t\r\n")-first+1);
}
}
bool SaveUserSettings() noexcept {
    try {
        if(savedValid && userSettings.language==savedSettings.language && userSettings.frameLimit==savedSettings.frameLimit &&
            userSettings.showFps==savedSettings.showFps &&
            userSettings.adaptiveDrawDistance==savedSettings.adaptiveDrawDistance &&
            userSettings.testLowQuality==savedSettings.testLowQuality)return true;
        // Replace only after a complete write. Menu changes write on leaving
        // Display, never during the render loop or for adaptive transitions.
        auto pending=configPath;pending+=".tmp";
        std::ofstream file(pending,std::ios::trunc);
        file<<"# Nintendo Switch display settings\n"
            <<"# Gameplay presentation cap: 0 = uncapped, 1..60 = FPS. Menus use VSync.\n"
            <<"frame_limit="<<userSettings.frameLimit<<"\n"
            <<"# Language: -1 follows the game/profile; 0 EN, 1 FR, 2 DE, 3 IT, 4 ES, 5 SV.\n"
            <<"language="<<userSettings.language<<"\n"
            <<"# Show the FPS counter: 0 = off, 1 = on.\n"
            <<"show_fps="<<userSettings.showFps<<"\n"
            <<"# Scene quality: 0.6 normally, 0.4 under load, emergency 0.2 at 23 FPS.\n"
            <<"adaptive_draw_distance="<<userSettings.adaptiveDrawDistance<<"\n"
            <<"# Diagnostic baseline: 1 holds 0.3 draw distance with normal scene quality; 0 restores adaptive behavior.\n"
            <<"test_low_quality="<<userSettings.testLowQuality<<"\n";
        file.flush();const bool good=file.good();file.close();
        if(!good || file.fail())throw std::runtime_error("Could not write Switch config");
        std::error_code error;
        std::filesystem::rename(pending,configPath,error);
        if(error && std::filesystem::exists(configPath)) {
            // FAT/Windows can reject rename-over-existing. Keep the old
            // config recoverable until the replacement has succeeded.
            auto previous=configPath;previous+=".bak";
            std::filesystem::remove(previous);
            std::filesystem::rename(configPath,previous);
            std::filesystem::rename(pending,configPath,error);
            if(error) {std::filesystem::rename(previous,configPath);throw std::system_error(error);}
            std::filesystem::remove(previous);
        } else if(error)throw std::system_error(error);
        savedSettings=userSettings;savedValid=true;
        return true;
    } catch(const std::exception& error) {
        if(journal)*journal<<"config.save_failed "<<error.what()<<std::endl;
        return false;
    }
}
void LoadUserSettings(const std::filesystem::path& directory,std::ostream& log) {
    configPath=directory/"config.txt";journal=&log;userSettings={};savedValid=false;
    auto previous=configPath;previous+=".bak";
    if(!std::filesystem::exists(configPath) && std::filesystem::exists(previous))
        std::filesystem::rename(previous,configPath);
    const bool exists=std::filesystem::exists(configPath);
    std::ifstream file(configPath);
    if(exists && !file)throw std::runtime_error("Cannot read existing config.txt");
    std::string line;
    while(std::getline(file,line)) {
        auto view=trim(line);view=view.substr(0,view.find_first_of("#;"));
        const auto equals=view.find('=');if(equals==view.npos)continue;
        const auto key=trim(view.substr(0,equals)),value=trim(view.substr(equals+1));
        int number{};const auto parsed=std::from_chars(value.data(),value.data()+value.size(),number);
        if(value.empty() || parsed.ec!=std::errc{} || parsed.ptr!=value.data()+value.size())continue;
        if(key=="language" && number>=-1 && number<=5)userSettings.language=number;
        else if(key=="frame_limit" && number>=0 && number<=60)userSettings.frameLimit=number;
        else if(key=="show_fps" && (number==0 || number==1))userSettings.showFps=number!=0;
        else if(key=="adaptive_draw_distance" && (number==0 || number==1))userSettings.adaptiveDrawDistance=number!=0;
        else if(key=="test_low_quality" && (number==0 || number==1))userSettings.testLowQuality=number!=0;
    }
    if(!exists)SaveUserSettings();
    else {savedSettings=userSettings;savedValid=true;}
    log<<"config.loaded path="<<configPath.string()<<" frame_limit="<<userSettings.frameLimit
       <<" language="<<userSettings.language<<" show_fps="<<userSettings.showFps<<" adaptive_draw_distance="<<userSettings.adaptiveDrawDistance<<" test_low_quality="<<userSettings.testLowQuality<<std::endl;
}
void CycleFrameLimit(int direction) noexcept {
    constexpr std::array<int,9> choices{{20,24,25,30,40,45,50,60,0}};
    auto at=std::find(choices.begin(),choices.end(),userSettings.frameLimit);
    const auto index=at==choices.end() ? 3:static_cast<int>(at-choices.begin());
    userSettings.frameLimit=choices[static_cast<std::size_t>((index+(direction<0 ? 8:1))%9)];
}
}
