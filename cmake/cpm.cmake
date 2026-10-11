# Pin and verify the CPM bootstrap; downloaded files stay outside the source tree.
set(pokerstove_cpm_version 0.43.2)
set(cpm_sha256 49a3bef91ceb65bb66d57255e12d1ffd22abc2f6408fa9fe4534c544a2f232aa)
if(CPM_SOURCE_CACHE)
    set(cpm_directory "${CPM_SOURCE_CACHE}/cpm")
elseif(DEFINED ENV{CPM_SOURCE_CACHE} AND NOT "$ENV{CPM_SOURCE_CACHE}" STREQUAL "")
    set(cpm_directory "$ENV{CPM_SOURCE_CACHE}/cpm")
else()
    set(cpm_directory "${PROJECT_BINARY_DIR}/cmake")
endif()
get_filename_component(cpm_directory "${cpm_directory}" ABSOLUTE BASE_DIR "${PROJECT_BINARY_DIR}")
file(MAKE_DIRECTORY "${cpm_directory}")
set(cpm_file "${cpm_directory}/CPM_${pokerstove_cpm_version}.cmake")
# Serialize downloads when independent build trees share the cache.
file(LOCK "${cpm_file}.lock" GUARD FILE TIMEOUT 120)
if(EXISTS "${cpm_file}")
    file(SHA256 "${cpm_file}" actual_sha256)
    if(NOT actual_sha256 STREQUAL cpm_sha256)
        message(FATAL_ERROR "Invalid CPM checksum: remove ${cpm_file} and configure again")
    endif()
else()
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${pokerstove_cpm_version}/CPM.cmake"
        "${cpm_file}.download"
        EXPECTED_HASH "SHA256=${cpm_sha256}"
        TLS_VERIFY ON TIMEOUT 120 STATUS download_status)
    list(GET download_status 0 download_code)
    if(NOT download_code EQUAL 0)
        file(REMOVE "${cpm_file}.download")
        message(FATAL_ERROR "CPM download failed: ${download_status}")
    endif()
    file(RENAME "${cpm_file}.download" "${cpm_file}")
endif()
include("${cpm_file}")
