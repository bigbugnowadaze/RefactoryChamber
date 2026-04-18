# Codex Prompt — Refractory Chamber

Build a JUCE-based VST3/AU guitar distortion plugin named **Refractory Chamber**.

This is **not** a standard amp sim and **not** an AI-based effect. The plugin must implement a CPU-light hybrid architecture designed for ultra-low tuned deathcore guitar.

Read the accompanying docs first:
- README.md
- PRODUCT_SPEC.md
- DSP_ARCHITECTURE.md
- IMPLEMENTATION_PLAN.md
- TEST_PLAN.md

## Non-negotiable concept
The plugin's tone must come from three interacting engines:
1. **Memristive Preamp** — path-dependent nonlinear stage that hardens under repeated attacks and relaxes between phrases.
2. **Refractory Chug Field** — a control-rate excitable band lattice that prevents identical low-band pileup during repeated chugs.
3. **Defect-Mode Output Stage** — a resonant/notch network that suppresses mud and channels aggression into selected overtone corridors.

## Constraints
- No AI/ML inference or training
- No convolution IR loading in v1
- No FFT-heavy architecture unless strictly required
- Keep CPU reasonable for real-time guitar tracking
- Prefer control-rate state systems and efficient IIR-style structures
- No heap allocation in the audio thread

## Implementation order
1. Create project skeleton and parameter layout.
2. Implement oversampled memristive preamp.
3. Implement control-rate 8-band analysis and refractory field.
4. Map field state onto a simple 4-band audio modulation path.
5. Implement defect-mode output stage.
6. Add global collapse state.
7. Add tests and basic UI.

## Required deliverables
- Buildable JUCE project
- Clean modular DSP classes
- Unit tests / smoke tests for core DSP behavior
- Minimal usable UI
- At least 3 factory presets

## Design rules
- Every “weird” control must map to real internal state.
- Every module must expose reset(), prepare(), and process methods.
- Clamp all nonlinear states.
- Favor stability and audibility over theoretical purity.
- The plugin must feel brutal and playable on low-tuned guitar DI.

## Suggested first milestone
Deliver a compilable plugin with:
- pass-through audio
- oversampling wrapper
- memristive preamp
- Heat / Imprint / Hardening / Relax controls
- one debug meter for memory state

Then continue iteratively using the docs.
