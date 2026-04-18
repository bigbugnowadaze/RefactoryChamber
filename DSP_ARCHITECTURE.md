# DSP Architecture — Refractory Chamber

## Signal chain overview

DI Input
-> Input HP / gate / pick pre-emphasis
-> Oversampling wrapper (2x/4x switchable)
-> Memristive Preamp
-> Refractory Chug Field (analysis/control + optional cross-band drive)
-> Defect-Mode Output Stage
-> Post low-pass / anti-fizz trim / output level

The core novelty is not in one stage alone. It is in the **interaction** between memory, recovery, and constrained overtone routing.

---

## 1. Input conditioning

### Purpose
Prepare absurdly low guitar DI for stable nonlinear processing without killing feel.

### Components
- DC blocker
- High-pass filter (tunable but constrained range, e.g. 25–120 Hz)
- Fast downward expander / gate
- Pick pre-emphasis tilt (broad upper-mid lift before distortion)

### Notes
This stage should be simple and practical. Do not waste novelty budget here.

---

## 2. Oversampling scaffold

### Purpose
Control aliasing from multiple nonlinear stages.

### Requirements
- 2x and 4x oversampling modes
- Latency-aware but low complexity
- Efficient polyphase or half-band implementation

### Acceptance target
- 2x should be default and affordable
- 4x should be optional for render / single-track use

---

## 3. Memristive Preamp

## Core idea
A path-dependent nonlinear stage whose transfer behavior is biased by recent excitation history.

Instead of:
- y = tanh(g * x)

Use:
- a nonlinear state variable m(t)
- transfer parameters that depend on m(t)
- m(t) updated by envelope, slope, or signed drive history

### State model (starter)
Let x[n] be input after pre-conditioning.

Envelope estimate:
- e[n] = max(abs(x[n]), a_e * e[n-1] + (1-a_e)*abs(x[n]))

Transient emphasis estimate:
- d[n] = abs(x[n] - x[n-1])

Memory state:
- m[n] = clamp((1-r) * m[n-1] + i1 * e[n] + i2 * d[n], 0, 1)

Where:
- r = relaxation coefficient (derived from Relax knob)
- i1 = sustained imprint contribution
- i2 = attack-sensitive imprint contribution

Drive law:
- g_eff[n] = g0 + g1 * m[n]

Bias / symmetry law:
- b_eff[n] = b0 + b1 * (m[n] - 0.5)

Dynamic waveshaper:
- y1[n] = sat(g_eff[n] * x[n] + b_eff[n])

Where sat() can be a custom odd/even hybrid saturator, e.g.:
- sat(z) = tanh(z) + c3*z^3/(1 + abs(z))

### Behavior goal
- isolated notes: broader / heavier / more open
- rapid machine-gun chugs: tighter, harder, more carved
- sustained abuse: increased aggression without uncontrolled fizz

### Design note
Do not simulate a real memristor literally. Use memristive principles to create audible path dependence.

---

## 4. Refractory Chug Field

## Core idea
Convert audio into a small control lattice over frequency bands. A hit excites local nodes. Once excited, a node enters a refractory state and cannot immediately fire again at full strength. This forces repeated chugs to distribute energy differently over time.

### Practical implementation strategy
Avoid a full PDE solver in v1. Use a lightweight band-lattice approximation.

#### Analysis stage
- Split signal into N bands (start with N = 8 or 12, not 64)
- For each band i, compute band envelope a_i[n]
- Maintain excitation u_i[n] and recovery v_i[n]

#### Update equations (discrete excitable model)
For each control tick (not every sample; e.g. 0.5–1 ms blocks):

- input_i = k_in * a_i[n]
- neighbor_i = sum_j W_ij * (u_j[n-1] - u_i[n-1])
- u_i[n] = u_i[n-1] + dt*(u_i[n-1] - u_i[n-1]^3/3 - v_i[n-1] + input_i + D*neighbor_i)
- v_i[n] = v_i[n-1] + dt*eps*(u_i[n-1] + a - b*v_i[n-1])

Then squash/clamp to stable range.

### Readout uses
The field should not directly resynthesize band audio in v1. Instead use it as a control structure.

For each band i:
- local_drive_i = base_drive_i + crush * relu(u_i)
- local_gain_i = 1 - refractory_mix * relu(v_i)
- spill_send_i_to_j = spill * max(0, u_i - threshold)

### Audio application options
Option A (simpler MVP):
- multiband control only
- one or two shaped audio bands get modulated by field outputs

Option B (stronger MVP):
- 4-band audio split
- each audio band has field-driven gain + drive + crossfeed into neighbor band drive

### Behavior goal
- repeated palm mutes do not stack identically
- adjacent upper bands light up in a structured way during dense chugs
- slower riffs stay massive; faster riffs self-tighten

### Performance note
Use coarse control-rate updates plus smoothed parameters to keep CPU manageable.

---

## 5. Defect-Mode Output Stage

## Core idea
Treat the output section like an impossible resonant medium with forbidden zones and defect corridors.

This is the stage that replaces ordinary “cab flavor” logic.

### Simplified interpretation
Build a sculpted transfer network with:
- one retained low core zone
- one suppressed mud band-gap zone
- two or three emphasized defect corridors above it
- optional dynamic instability in defect resonance under hard excitation

### Implementation candidates
#### Candidate A — Parallel resonant corridor filters
- Create 3–4 tuned resonant bands / shelf shapes
- One broad notch or suppressed corridor in the mud zone
- Dynamic gains tied to Memristive / Chug Field state

#### Candidate B — Small modal network
- 4–6 resonant modes with cross-coupling
- certain modes remain weak until field activity opens them
- gives more “cab as hostile material” behavior

### Suggested default band logic
- Core zone: low-mid weight, tightly limited
- Forbidden zone: broad low-mid mud suppression
- Defect corridor 1: note definition / growl
- Defect corridor 2: pick articulation / clank
- Defect corridor 3: upper aggression / edge

Exact frequencies should be tuned by ear, not hard-coded from theory.

### Example parameterization
- gap_center
- gap_width
- defect_1_gain
- defect_2_gain
- defect_3_gain
- defect_state_sensitivity

### Behavior goal
The output should sound like the signal is being forced through a designed material that refuses some energy and channels the rest into brutal overtone lanes.

---

## 6. Collapse / regime-shift layer

## Core idea
Introduce a controlled non-smooth regime shift at high excitation.

### Practical rule
When global stress exceeds threshold T:
- increase asymmetry
- narrow bandwidth slightly
- increase upper defect activity
- shorten refractory recovery a bit

This creates a feeling of the amp “snapping into a killing mode.”

### Global stress estimate
- s[n] = weighted_sum(memory_state, average_field_excitation, input_envelope)

### State machine
- NORMAL
- ARMED
- COLLAPSED

Hysteresis between these states is mandatory so it does not chatter.

---

## 7. Suggested default parameter mappings

### Tight
- raises HP cutoff modestly
- shortens low-band recovery tail
- reduces low defect spill

### Punch
- increases transient emphasis into memory state
- raises articulation corridor weighting

### Heat
- increases base preamp drive
- mildly increases collapse susceptibility

### Imprint
- increases i1 / i2 in memory update

### Refractory
- changes eps / recovery speed in field

### Spill
- increases off-diagonal coupling W_ij and cross-band drive bleed

### Defect
- morphs among preset corridor templates

### Collapse
- lowers regime threshold and increases collapsed-state contrasts

---

## 8. MVP DSP simplifications
Do these unless needed otherwise:
- band field at control rate
- 8 control bands first, not 32+
- simple IIR or lightweight FIR analysis bands
- no FFT in v1 unless profiling proves harmless
- no cabinet convolution in v1
- no full PDE solver in v1
- no full physical tube model in v1

---

## 9. Anti-aliasing / stability concerns
- nonlinear preamp must be oversampled
- control-rate field outputs must be smoothed before audio-rate modulation
- defect resonators need careful Q limiting
- collapse mode transitions need hysteresis and slew limiting
- extreme presets must not output NaN/Inf or self-oscillate uncontrollably

---

## 10. Pseudocode skeleton

```cpp
processBlock(audio):
    conditioned = inputStage(audio)
    os = oversample(conditioned)

    for each sample in os:
        env.update(sample)
        mem.update(sample, env, diff)
        pre = memristiveDrive(sample, mem)
        bandAnalysis.push(pre)

        if controlTick:
            field.update(bandEnvelopes)
            globalState.update(mem, field, env)
            paramsFromState = mapState(mem, field, globalState)

        shaped = applyBandState(pre, paramsFromState)
        out = defectStage.process(shaped, paramsFromState)
        out = postTrim(out, globalState)
        write(out)

    return downsample(osOut)
```

---

## 11. Stretch ideas for v2
- hidden-state unlocks via pulsed excitation
- dual-field stereo interaction
- optional subharmonic restraint mode
- adaptive pick detector per string region
- lightweight learned preset recommender only if ever desired (not audio path)
