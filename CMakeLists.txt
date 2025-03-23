cmake_minimum_required(VERSION 3.15)
project(AudioWorkstation VERSION 0.1.0)

# Add JUCE as a subproject
add_subdirectory(JUCE)

# Set JUCE options
juce_add_gui_app(AudioWorkstation
    PRODUCT_NAME "Audio Workstation" 
    VERSION 0.1.0
    COMPANY_NAME "YourCompanyName"
    COMPANY_COPYRIGHT "Copyright (c) 2025 YourCompanyName"
)

# Set compiler options
target_compile_features(AudioWorkstation PRIVATE cxx_std_17)

# Enable JUCE format
juce_generate_juce_header(AudioWorkstation)

# Find all source files
file(GLOB_RECURSE AudioWorkstation_SOURCES
    "Source/*.cpp"
    "Source/*.h"
)

# Add source files to the target
target_sources(AudioWorkstation PRIVATE
    ${AudioWorkstation_SOURCES}
)

# Include header directories
target_include_directories(AudioWorkstation PRIVATE Source)

# Link required JUCE libraries
target_link_libraries(AudioWorkstation PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Options for macOS binaries
set_target_properties(AudioWorkstation PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Audio Workstation"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.audioworkstation"
    MACOSX_BUNDLE_INFO_STRING "Audio Workstation"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${CMAKE_PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${CMAKE_PROJECT_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${CMAKE_PROJECT_VERSION}"
)