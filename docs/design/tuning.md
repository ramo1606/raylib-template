# Tuning

Every field of `Tuning` (src/sim.h). Live-editable in debug builds with F1. When a value changes in `SimDefaultTuning`, add a history line.

| Field | Value | Intended feel |
|---|---|---|
| playerAccel | 2200 | reaches top speed in ~0.1 s: responsive, slightly floaty |
| playerMaxSpeed | 220 | crosses the arena in ~3 s |
| playerFriction | 0.0005 | stops in ~0.2 s after release |
| enemyBaseSpeed | 90 | dodgeable on reaction at t=0 |
| enemySpeedRamp | 1.5 | +90 px/s after one minute |
| spawnInterval | 1.2 | one new threat per ~second at start |
| spawnIntervalMin | 0.35 | late-game density cap |

## History
| Date | Field | Old → New | Why (playtest ref) |
|---|---|---|---|
| | | | |
