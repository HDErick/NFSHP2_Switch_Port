#include "pipeline_cache.hpp"
namespace hp2::nx {
#include "pipeline_cache_seed.inc"
PipelineCacheSeed BundledPipelineCache() noexcept
{
    return {embeddedPipelineCache,sizeof(embeddedPipelineCache)};
}
}
