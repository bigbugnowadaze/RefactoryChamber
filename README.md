# Refractory Chamber — Codex Handoff Package

## One-line product thesis
A CPU-light, non-AI distortion/amp plugin for absurdly low-tuned deathcore guitar that stays brutal by treating the signal as a **stateful hostile medium** instead of a static transfer curve.

## Core claim
This is not a conventional amp sim with cosmetic novelty. It is a hybrid guitar processor whose tone is defined by three outsider/physics-derived engines:

1. **Memristive Preamp** — a path-dependent nonlinear stage that hardens under repeated picking and relaxes between phrases.
2. **Refractory Chug Field** — an excitable-band lattice that prevents consecutive chugs from piling into the same muddy lane.
3. **Defect-Mode Cab / Material Stage** — a resonant output stage with forbidden zones and designed overtone corridors, inspired by defect-mode metamaterials.

The plugin should feel like an amp, but behave like a **nonlinear medium with memory, recovery, and constrained transport**.

## Product goals
- Serve 8/9/10-string extreme metal, including absurd tunings like double-drop-F.
- Preserve pick attack and riff identity under dense palm-muted passages.
- Create a signature brutality that is not just “more gain” or “more low end.”
- Stay CPU-light enough for real-time play and multi-track use.
- Avoid AI/ML inference, training, IR convolution dependency, or heavyweight spectral processing.

## What success sounds like
- Fast chugs remain separated instead of collapsing into low-mid blur.
- Repeated attacks feel like they alter the “material” of the amp in a musical way.
- The low register stays violent, but note identity survives because energy is redirected into selected overtone lanes.
- The user can get a tone that feels impossible / unnatural / futuristic without sounding like a novelty FX plugin.

## Package contents
- `PRODUCT_SPEC.md` — full product spec and UX thesis
- `DSP_ARCHITECTURE.md` — signal flow, math models, module behavior
- `IMPLEMENTATION_PLAN.md` — repo layout, milestones, acceptance criteria
- `TEST_PLAN.md` — correctness, audio quality, and performance tests
- `CODEX_PROMPT.md` — exact handoff prompt for Codex

## MVP recommendation
Build the plugin in this order:
1. Input conditioning + oversampling scaffold
2. Memristive preamp
3. Refractory Chug Field
4. Defect-Mode Cab / Material stage
5. Simple gate / tone / output polish
6. Preset system + validation tools

## Non-goals
- Faithful emulation of a real amp head
- Visual metaphors with no audible consequence
- Neural “smart tone” assistance
- Huge modular routing environment
- Convolution IR loader in v1

## Suggested product name candidates
- Refractory Chamber
- Hostile Medium
- Defect Engine
- Chug Reactor
- Furnace Lattice

My recommendation: **Refractory Chamber**.
