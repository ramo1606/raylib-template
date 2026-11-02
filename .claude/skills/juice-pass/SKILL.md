---
name: juice-pass
description: Checklist and code recipes for game feel ("juice") in raylib - easing, screen shake, hit-stop, squash and stretch, flashes, particles, camera kick, sound pitch variation. Use when a feature works but feels flat, or when asked for a juice/polish pass.
---

# Juice pass

Work through the event list, one event at a time, smallest effect first. Every effect must be **tunable** (a field in `Game` or `Tuning`), **toggleable** from the debug panel, and **readable** (never hide the player or threats). Respect a future "reduced motion" option: route shake/flash amounts through one multiplier.

## 1. List the moments
From `SimEvent` flags and scene changes, list: spawn, pickup, hit/damage, death, level start/end, UI confirm/back, score milestone. For each, pick 2–3 effects below. Write the table to `docs/design/juice.md`.

## 2. Recipes (presentation layer only — never in sim.c)

| Effect | Recipe |
|---|---|
| Ease in/out | `reasings.h`: UI slides `EaseBackOut`, fades `EaseQuadOut`, bounces `EaseElasticOut`, with a 0..1 timer per element |
| Screen shake | `shake = max(shake, amount)`; offset blit by `rand(-1,1)*shake`; `shake *= 0.85f` per fixed tick (already in `GameDraw`) |
| Hit-stop | on hit, skip `SimStep` for 2–6 fixed ticks (keep drawing + particles) |
| Squash & stretch | scale sprite by `(1+k, 1-k)` along velocity, `k` eased back to 0 over ~0.15 s |
| Flash | draw entity with `WHITE` tint or a flash shader for 1–2 frames on hit |
| Particles | `EmitBurst(pos, color, count, speed)`; vary count by importance; short lives (0.2–0.6 s) |
| Camera kick | push `Camera2D.offset` opposite to the impact direction, ease back |
| Number pop | score text scale with `EaseBackOut` (already on pickup) |
| Audio | every visual event has a sound; ±10% random pitch; layered "thump + sparkle" for big moments |
| Anticipation | 0.1–0.2 s telegraph before enemy attacks (blink, scale up) — juice that is also fairness |

## 3. Verify
- `make smoke` and read the screenshots: effects must not obscure the player in any frame you look at.
- Ask the human to play twice: once with juice multiplier 0, once at 1. If they can't tell the difference, the effect is too subtle; if it's tiring after 2 minutes, it's too strong.
