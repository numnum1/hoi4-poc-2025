# Grand Strategy Prototype — Unreal Engine 5

A character-driven grand strategy prototype in the spirit of *Hearts of Iron* and *Crusader Kings*,
written in C++ on Unreal Engine 5.8. You play a single character on a 3D globe: click a province to
see who is present, found or join an organization, recruit other characters, build, and keep the
books balanced while the world simulates hour by hour.

This started as a proof of concept and UX test. The focus is on the architecture underneath the
gameplay, not on content or visuals.

## Features

- **Interactive globe.** Provinces are colored on the GPU from a lookup texture. Map modes (political,
  (political, population, …) and click/hover resolution from pixel to province.
- **Simulated world.** Characters follow hourly schedules in their local time zone, populations grow,
  and bank accounts and daily economic settlement run on a speed-adjustable game clock.
- **Organizations.** Found, join and recruit through context-sensitive actions. Permissions come from
  the roles a character holds.
- **Buildings.** Province build menu. Organizations are anchored by a headquarters building.
- **Focus trees.** HOI4-style organization focus trees with prerequisites, mutually exclusive
  branches and data-driven requirements/effects.
- **Data-driven scenarios.** Nations, provinces, characters, races, schedules, banks and
  organizations are authored as data assets and loaded into the simulation at game start.

## Architecture

```
Mass Entity fragments + processors        simulation state, one entity per character/province/…
  └─ UXxxEntitySubsystem                  typed-id API over the entities, C++-only change events
       └─ UXxx facade                     ActorComponent on ARtsGameState: Blueprint-safe API + delegates
            └─ ViewModels (RtsUI)         MVVM view models, FieldNotify properties, composed as a tree
                 └─ UMG widgets           C++ base classes, Blueprints for layout only
```

Design decisions worth pointing out:

- **ECS for the simulation.** All per-entity state lives in [Mass Entity](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine)
  fragments. Processors are not ticked every frame. They run on the game clock
  (`UTime::OnHourElapsed`), so simulation cost scales with game time, not frame rate.
- **Isolated domain modules.** `Characters`, `Provinces`, `Nations`, `Organizations`, `Buildings`,
  `Banking`, `Populations`, `Races`, `Schedules`, `Time` and `Focuses` don't know each other. The
  few allowed dependencies are documented in the respective `*.Build.cs`. Only `RtsUI` sees every
  domain.
- **Facades hide the ECS.** Blueprints and UI only ever talk to the facade components on the game
  state. Mass types never leak past the subsystem layer.
- **MVVM like React components.** View models are small, composable C++ classes. Data flows down via
  setters and FieldNotify properties. User events flow up through the owning widget into the parent
  view model, which re-validates before acting. Derived values are computed, not synchronized by
  hand.
- **Typed, stable ids.** `FCharacterId`, `FProvinceId`, … instead of raw integers or entity handles.
  An id is a slot that is never reused or compacted, which keeps references valid for save/load.
- **Gameplay as `enum` + `switch`.** Actions, map modes and focus effects are closed sets, so they
  are modeled as enums with small switch functions (`EAction` + `URtsActions`). Data assets stay
  pure data.

## Repository layout

| Module | Responsibility |
|---|---|
| `Rts` | Game mode, game state, scenario loading |
| `RtsCore` | Typed ids and shared helpers |
| `Characters`, `Provinces`, `Nations`, `Organizations`, `Buildings`, `Banking`, `Populations`, `Races`, `Schedules`, `Time`, `Focuses` | Domain modules: Mass fragments, entity subsystem, facade |
| `Ecology`, `Ideologies` | Placeholder modules for planned features |
| `Scenario` | Data assets that describe a starting scenario |
| `RtsPlayer` | Player controller and the controlled character |
| `RtsUI` | View models, actions, economy rules and widget base classes |
| `WorldMap` | Globe rendering, province coloring and picking (see [`Source/WorldMap/README.md`](Source/WorldMap/README.md)) |
| `MapCamera` | Globe camera pawn and input |

## Tests

Unreal automation tests live next to the code they cover (`Source/Rts/Tests`). Run them from
**Tools → Session Frontend → Automation** with the filter `Rts.`, or headless:

```
UnrealEditor-Cmd.exe Rts.uproject -ExecCmds="Automation RunTests Rts.; Quit" -unattended -nullrhi -nosplash
```

## Building

- Unreal Engine 5.8, Visual Studio 2022 (Win64)
- The globe uses meshes, materials and the province map from the *BloodAnvil* map plugin, which is
  not part of this repository.
- Only source code and config are tracked. Content (maps, materials, widget Blueprints, scenario
  data) is not, so a fresh clone compiles but isn't playable.

## License

[MIT](LICENSE)
