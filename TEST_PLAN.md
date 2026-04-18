# Test Plan — Refractory Chamber

## 1. Smoke tests
- plugin instantiates in supported DAWs
- sample-rate changes do not crash
- block-size changes do not crash
- bypass works cleanly
- preset load/save works

## 2. Correctness tests

### Memristive preamp
- memory state remains bounded [0,1]
- dense burst input produces higher average memory than isolated transients
- long silence decays memory toward baseline
- no NaN / Inf under max drive

### Chug field
- u/v states remain finite and bounded under sustained excitation
- higher Refractory increases recovery time measurably
- higher Spill increases neighbor activity measurably
- same input repeated yields stateful evolution, not identical frame outputs

### Defect stage
- Gap widens suppression region without blowing overall gain
- Defect morph changes response shape predictably
- Bite increases articulation corridor emphasis
- resonator coefficients remain stable

### Collapse state
- threshold and hysteresis produce deterministic state transitions
- collapse does not chatter at constant input near threshold

## 3. Audio behavior tests
Use a fixed test pack:
- down-tuned DI chugs
- slow breakdown notes
- trem-picked line
- sustained dissonant chord
- bass DI for abuse test

Listen for:
- pick attack retention
- note separation under dense chugs
- low-end control
- absence of static fizz
- meaningful per-hit variation

## 4. Null / repeatability tests
- repeated offline renders with same seed/settings should match
- automation playback should be deterministic

## 5. Performance tests
- CPU usage at 44.1 / 48 / 96 kHz
- CPU usage at 64 / 128 / 256 / 512 buffers
- compare 2x vs 4x oversampling
- verify no audio-thread allocations

## 6. Preset validation tests
Every factory preset must be checked for:
- output level sanity
- no runaway resonance
- audible identity distinct from neighbors
- useful starting point on low-tuned DI

## 7. Metrics worth logging
- average memory state
- average field excitation
- average field recovery
- collapse dwell time
- peak output
- oversampling mode
- processing time per block in debug/profiling builds

## 8. Red flags
- sounds like a normal amp sim with extra knobs
- stateful behavior only visible on meters, not audible
- low strings get bigger but less readable
- attack is lost once the field engages
- defect stage just sounds like a cocked wah or static resonator
