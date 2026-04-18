# Implementation Plan — Refractory Chamber

## Recommended stack
- **Framework:** JUCE
- **Language:** C++17 or newer
- **Plugin targets:** VST3, AU
- **Build:** CMake + JUCE
- **Tests:** Catch2 or JUCE unit tests for DSP core

## Recommended repo structure

```text
refractory-chamber/
  CMakeLists.txt
  JUCE/
  Source/
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
    dsp/
      InputStage.h
      Oversampler.h
      MemristiveDrive.h
      BandSplitter.h
      ChugField.h
      DefectStage.h
      CollapseState.h
      ParameterMap.h
      Utils.h
    state/
      PresetManager.h
      ParameterLayout.h
    tests/
      test_memristive_drive.cpp
      test_chug_field.cpp
      test_defect_stage.cpp
      test_processor_smoke.cpp
  Assets/
  Docs/
```

## Module responsibilities

### InputStage
- DC block
- HP filter
- gate / expander
- pre-emphasis tilt

### Oversampler
- 2x / 4x modes
- clean up/down sample APIs

### MemristiveDrive
- maintain memory state
- expose smoothed derived parameters
- render main preamp nonlinearity

### BandSplitter
- compute coarse analysis envelopes
- optionally host 4-band audio split for state application

### ChugField
- control-rate excitable lattice
- expose band activity / recovery / stress

### DefectStage
- resonant/notch structure implementing the material logic
- dynamic emphasis by field state

### CollapseState
- global regime state machine with hysteresis

### ParameterMap
- maps knobs to internal coefficients safely

## Build phases

### Phase 0 — Skeleton
Tasks:
- create JUCE project
- create parameter layout
- wire audio pass-through
- add oversampling shell

Acceptance:
- plugin loads in DAW
- parameters automate
- no crashes under sample-rate changes

### Phase 1 — Memristive Preamp MVP
Tasks:
- implement memory state update
- implement waveshaper with state-dependent drive/bias
- expose Heat / Imprint / Hardening / Relax
- add oversampled render path

Acceptance:
- audible difference between isolated hits and rapid bursts
- stable at 44.1k, 48k, 96k
- no denormals / NaNs

### Phase 2 — Chug Field MVP
Tasks:
- implement 8-band envelope analysis
- implement control-rate excitable lattice update
- map field state to 4-band gain/drive modulation
- expose Ignition / Refractory / Spill / Crush

Acceptance:
- repeated palm mutes change response over time
- effect is audible, not just visible
- CPU remains acceptable at 48k / 128 buffer

### Phase 3 — Defect Stage MVP
Tasks:
- implement mud-gap + defect corridor network
- tie corridor emphasis to field / memory state
- expose Core / Defect / Gap / Bite

Acceptance:
- plugin can create heavier + clearer output than memristive stage alone
- defect settings audibly shift overtone emphasis
- no unstable resonant blowups

### Phase 4 — Collapse mode
Tasks:
- implement global stress estimator
- implement NORMAL / ARMED / COLLAPSED state machine
- expose Collapse

Acceptance:
- high-energy passages trigger a distinct but controllable regime
- transitions do not chatter

### Phase 5 — Product polish
Tasks:
- presets
- simple visual meters
- tooltips / labels
- output safety limiter if needed

Acceptance:
- usable by guitarists without reading theory docs
- presets clearly demonstrate different behaviors

## Detailed task list by file/module

### `ParameterLayout.h`
- define all AudioParameterFloat / choice params
- centralize ranges, defaults, skew factors
- acceptance: all parameters serializable and automatable

### `InputStage.h`
- implement processSample / processBlock
- expose setSampleRate, reset
- acceptance: HP and gate stable; no clicks on reset

### `Oversampler.h`
- wrap JUCE dsp::Oversampling or custom half-band implementation
- acceptance: no invalid state during buffer-size changes

### `MemristiveDrive.h`
- state struct with memory variable, smoothing, derived values
- updateMemory(sample, env, slope)
- processSample(sample)
- acceptance: memory trace responds differently to dense vs sparse playing

### `BandSplitter.h`
- create 8 envelope taps using efficient filters
- acceptance: envelope outputs are stable and bounded

### `ChugField.h`
- maintain u/v vectors
- updateControlTick(envelopes)
- expose per-band and aggregate states
- acceptance: refractory behavior visibly and audibly occurs

### `DefectStage.h`
- implement corridor/notch network
- dynamic coefficients from field state
- acceptance: audible corridor morphing, no unstable Q explosions

### `CollapseState.h`
- aggregate mem + field + env into stress
- hysteretic state machine
- acceptance: deterministic transitions across repeated renders

### `PluginProcessor.cpp`
- integrate all stages
- support reset, prepareToPlay, automation, bypass
- acceptance: offline render matches real-time path closely given same settings

### `PluginEditor.cpp`
- create one-page UI
- meters for memory, field, collapse
- acceptance: controls readable at small sizes

## Performance targets
- < 3% CPU on one mono instance at 48k / 256 on a modern desktop in 2x mode is an aspirational early target
- graceful scaling with 4x mode
- no heap allocation in processBlock

## Safety rules for Codex
- never allocate in the audio thread
- no exceptions from DSP code
- clamp every nonlinear state
- provide reset() for every DSP class
- add unit tests before exotic refactors

## Suggested dev order inside each phase
1. core DSP class without JUCE coupling
2. small unit tests
3. processor integration
4. profiling
5. tuning by ear
