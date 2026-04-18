# Product Spec — Refractory Chamber

## Product category
A novel **distortion / amp-like plugin** for down-tuned extreme guitar.

## Market position
This should be legible to guitarists as an amp/distortion plugin, but should clearly produce behaviors standard amp sims do not:
- chug separation via refractory recovery
- burst-density-dependent hardening
- controlled overtone routing into specific aggressive bands
- regime shifts that feel like the amp “locks in” under abuse

## User problem
Ultra-low rhythm guitar often fails in one of two ways:
1. It becomes huge but undefined.
2. It becomes clear but loses brutality.

The plugin should solve that by making **clarity emerge from stateful structure**, not just EQ cleanup.

## Target users
- deathcore / thall / downtempo / slam / beatdown guitarists
- producers reamping DI tracks for absurdly low tunings
- sound designers wanting weaponized low-register distortion

## User promise
"This plugin does not just add gain. It turns absurdly low guitar into a controlled hostile medium."

## Primary UX principle
Every control should feel like it is changing the **physics of the medium**, not tweaking studio utilities.

## Top-level controls
### Section A — Input / Tightness
- **Gate** — fast input cleanup, minimal controls
- **Tight** — high-pass tightening before main nonlinearity
- **Punch** — pre-emphasis for pick attack / upper articulation

### Section B — Memristive Preamp
- **Heat** — overall preamp drive
- **Imprint** — memory depth; how much recent playing biases the transfer curve
- **Hardening** — how burst density sharpens/clamps later hits
- **Relax** — how quickly the stage returns toward baseline

### Section C — Refractory Chug Field
- **Ignition** — how easily hits excite the field
- **Refractory** — recovery time after local band activation
- **Spill** — neighbor coupling; how energy escapes into adjacent overtone lanes
- **Crush** — how strongly field activity modulates saturation/gain

### Section D — Defect-Mode Output Stage
- **Core** — retained low-frequency mass
- **Defect** — chooses the overtone corridor / defect pattern
- **Gap** — width/depth of forbidden mud zone
- **Bite** — upper defect emphasis / aggressive readout

### Section E — Master behavior
- **Collapse** — catastrophe/regime-shift intensity when pushed
- **Mix** — wet/dry blend for parallel brutality
- **Level** — output trim

## Internal processing concept
The plugin has four internal identities:
1. **A responsive input conditioner**
2. **A preamp that remembers**
3. **A band-lattice that recovers**
4. **A cab/material stage that favors selected aggressive corridors**

## Audio philosophy
### What it should preserve
- pick attack
- note starts
- low-string rhythm definition
- per-hit variation during repeated bursts

### What it should suppress
- indistinct low-mid stacking
- repeated identical mush
- static fizz
- broad unstructured sub bloom

## MVP modes
Provide three voicings, not a huge matrix:
- **Ruin** — tight and violent, fastest recovery
- **Monolith** — slower, heavier, more sustained mass
- **Virus** — most unstable, strongest spill and collapse behavior

## Preset direction
Ship with presets named by behavior, not fake amp references:
- Surgical Chug
- Permafrost Drop
- Quarantine Breakdown
- Defect Wall
- Pulse Engine
- Waste Cathedral

## UI notes
- Dark, minimal, brutal
- One-screen UI
- Avoid faux tubes, faux cabs, fake screws
- Visual meters should show state, not realism:
  - memory / hardening trace
  - chug field activation
  - defect corridor emphasis

## v1 constraints
- Mono-in / stereo-out acceptable
- No standalone app required in v1
- VST3 + AU only acceptable in v1
- Max CPU target: playable on multiple rhythm tracks with oversampling optional

## Hard design rules
- No AI inference
- No black-box learned models
- No hidden auto-EQ magic
- Any macro must map to real internal parameters
- Every weird concept must have audible consequence
