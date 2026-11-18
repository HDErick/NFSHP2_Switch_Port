#include "indep/inc/Ecstasy.hpp"
#include <cassert>
int main() {
    using namespace hp2;
    configureNativeRenderSettings({});
#ifdef __SWITCH2__
    assert(nativeRenderSettings().drawDistanceScale==1 && !nativeSceneQuality().reducedOpponents);
    for(int limit : {30,60,75,90})assert(!configureNativeFrameLimitQuality(limit));
    assert(configureNativeFrameLimitQuality(120));
    assert(nativeSceneQuality().reducedOpponents && nativeSceneQuality().vehicleLod==1.75f);
    assert(nativeSceneQuality().sceneryLod==1 && nativeSceneQuality().nearDetailEnter==35);
    assert(nativeRenderSettings().drawDistanceScale==1 && nativeRenderSettings().mirrorDistanceScale==.5f);
    assert(!configureNativeFrameLimitQuality(0));
    assert(configureNativeFrameLimitQuality(60));
    assert(nativeSceneQuality().sceneryLod==3 && nativeSceneQuality().vehicleLod==2.5f);
    assert(nativeSceneQuality().nearDetailEnter==125 && nativeSceneQuality().nearDetailExit==165);
    assert(nativeRenderSettings().drawDistanceScale==1 && nativeRenderSettings().mirrorDistanceScale==1);
    assert(configureNativeFrameLimitQuality(0));
    assert(configureNativeFrameLimitQuality(90));
#else
    for(int limit : {0,30,60,120})assert(!configureNativeFrameLimitQuality(limit));
    assert(nativeSceneQuality().reducedOpponents && nativeSceneQuality().vehicleLod==1.75f);
    assert(nativeRenderSettings().drawDistanceScale==.3f && nativeRenderSettings().mirrorDistanceScale==.5f);
#endif
    assert(EcstasyAdvanceEnvironmentImages(false,false,false,0));
    assert(EcstasyAdvanceEnvironmentImages(true,true,true,0));
    assert(EcstasyAdvanceEnvironmentImages(true,true,false,3));
    assert(!EcstasyAdvanceEnvironmentImages(true,true,false,0));
    assert(!EcstasyAdvanceEnvironmentImages(true,false,false,0));
    // 120 Hz renders of a 50 Hz PAL schedule blend each new map once.
    int updates=0;
    for(int frame=0;frame<120;++frame) {
        const bool tick=frame*50/120!=(frame+1)*50/120;
        updates+=EcstasyAdvanceEnvironmentImages(true,true,tick,tick ? 1u:0u);
    }
    assert(updates==50);
    // A capped capture after a logical tick still consumes fresh pending faces.
    for(int frame=0;frame<30;++frame)assert(EcstasyAdvanceEnvironmentImages(true,true,false,1));
}
