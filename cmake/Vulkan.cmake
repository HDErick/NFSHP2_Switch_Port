# The Windows checkout includes the Khronos C headers and a loader import
# library. Driver/runtime installation belongs to the graphics driver.
if(WIN32 AND MSVC)
    set(_hp2_vulkan "${CMAKE_CURRENT_LIST_DIR}/../third_party/vulkan")
    foreach(_hp2_file IN ITEMS include/vulkan/vulkan.h lib/x64/vulkan-1.lib)
        if(NOT EXISTS "${_hp2_vulkan}/${_hp2_file}")
            message(FATAL_ERROR "Missing pinned Vulkan dependency: third_party/vulkan/${_hp2_file}")
        endif()
    endforeach()
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "The native Windows port requires an x64 configuration")
    endif()
    add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
    set_target_properties(Vulkan::Vulkan PROPERTIES
        IMPORTED_LOCATION "${_hp2_vulkan}/lib/x64/vulkan-1.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${_hp2_vulkan}/include")
    unset(_hp2_vulkan)
    unset(_hp2_file)
else()
    find_package(Vulkan 1.1 REQUIRED)
endif()
