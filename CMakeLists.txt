cmake_minimum_required(VERSION 3.21)
project(RefractoryChamber VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RC_FETCH_JUCE "Fetch JUCE from GitHub when JUCE source dir is not provided" ON)
set(RC_JUCE_PATH "" CACHE PATH "Path to a local JUCE checkout")

if (RC_JUCE_PATH)
  message(STATUS "Using JUCE from RC_JUCE_PATH=${RC_JUCE_PATH}")
  add_subdirectory(${RC_JUCE_PATH} JUCE)
elseif (RC_FETCH_JUCE)
  include(FetchContent)
  FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.3
  )
  FetchContent_MakeAvailable(juce)
else()
  message(FATAL_ERROR "JUCE not found. Provide -DRC_JUCE_PATH=/path/to/JUCE or set -DRC_FETCH_JUCE=ON")
endif()

juce_add_plugin(RefractoryChamber
  COMPANY_NAME "Refractory"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_MANUFACTURER_CODE RFCH
  PLUGIN_CODE RfCh
  FORMATS VST3 AU
  PRODUCT_NAME "Refractory Chamber")

target_sources(RefractoryChamber PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
  Source/dsp/InputStage.h
  Source/dsp/Oversampler.h
  Source/dsp/MemristiveDrive.h
  Source/dsp/BandSplitter.h
  Source/dsp/ChugField.h
  Source/dsp/DefectStage.h
  Source/dsp/CollapseState.h
  Source/state/ParameterLayout.h
  Source/state/PresetManager.h)

target_compile_definitions(RefractoryChamber
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(RefractoryChamber
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

add_executable(RefractoryChamberDSPTests
  Source/tests/test_main.cpp)
target_include_directories(RefractoryChamberDSPTests PRIVATE Source)
target_link_libraries(RefractoryChamberDSPTests PRIVATE juce::juce_dsp)
