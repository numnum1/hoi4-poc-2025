# Modul `MapCamera`

Die Kamera auf dem Globus und die Eingabe, die sie bewegt. Eine Klasse: `ARtsMapPawn`.

Das Modul **löst nichts auf und entscheidet nichts**. Es weiß nicht, was eine Provinz ist, wem sie
gehört oder was ein Klick bedeutet — es tract, und reicht den Treffer weiter.

## Rolle in der Schichtung

```
MapCamera  ── Trefferpunkt ──►  WorldMap  ── Provinzklick ──►  RtsUI
```

Die Abhängigkeit läuft **nur in eine Richtung**: `MapCamera` hängt privat an `WorldMap`, `WorldMap`
kennt dieses Modul nicht. Das ist kein Stilgeschmack, sondern die Bedingung dafür, dass es zwei
Module sein können — `UWorldMapWidget` hielt früher einen `ARtsMapPawn` und band dessen Delegate,
und genau das wäre ein Zyklus.

Daraus folgt die Aufteilung des Klicks:

- Der Pawn tract (`TraceUnderCursor`) und ruft `ARtsWorldMap::NotifyWorldClicked(FHitResult)`.
- Die **Karte** rechnet den Weltpunkt in UV um, liest den Provinzpixel und broadcastet
  `OnProvinceClicked`.
- Der Pawn fasst deshalb nie eine `FProvinceId` an und hängt nicht an `RtsCore`.

Ein Fehlschuss wird mitgereicht, nicht verschluckt: das leere `FHitResult` geht denselben Weg und
die Karte meldet eine ungültige Id. Neben den Globus klicken ist Abwählen, und Abwählen ist ein
Ergebnis.

## `ARtsMapPawn`

Der Pawn hält **keine eigenen Kartendaten**. Was er von `ARtsWorldMap` braucht, ist `MapRadius`:
Kamerahöhen sind Abstände zur Kugeloberfläche, nicht zum Weltursprung, und werden deshalb überall
gegen den Radius verrechnet (`FrameCamera`, `AddMovement`).

- **Bewegung ist Rotation *um* die Kugel**, nicht Translation — `AddControllerYawInput` /
  `AddControllerPitchInput`, nicht `AddActorWorldOffset`.
- **Die Bewegungsskalierung hängt an der Kamerahöhe**, damit die Kamera herangezoomt kriecht und
  herausgezoomt fegt (`MinHeightRelativeMovementScale` … `MaxHeightRelativeMovementDistance`).
- **Der Tick kommt aus dem Blueprint.** `AddMovement` und `ComputeMovementScale` sind
  `BlueprintCallable` und werden von `Content/RTS/Player/PlayerMovementPawn_BP` pro Frame gerufen;
  `PrimaryActorTick.bCanEverTick` ist aus. Deshalb behalten beide Funktionen ihre Namen.
- **Der Klick ist nativ gebunden**, nicht im Graphen: `SetupPlayerInputComponent` hängt
  `InputSelect` auf `HandleSelectStarted`.

`FrameCamera` läuft absichtlich aus `SetupPlayerInputComponent` und nicht aus `PossessedBy` —
`AController::OnPossess` überschreibt die Control Rotation direkt nach `PossessedBy` mit der
Actor-Rotation des Pawns und würde alles dort Gesetzte wieder wegwerfen.

## Externe Abhängigkeiten

- **BloodAnvil-Plugin** (nicht in diesem Repository): die Input-Assets, die der Konstruktor per Pfad
  zieht (`IMC_DefaultPlayer`, `Move`, `Select`). `ARtsMapPawn` leitete früher von `ABA_MapPawn` ab;
  der Code wurde nach `Source/` gezogen, weil `Plugins/` hier nicht versioniert ist und das Plugin
  seinen Klick nur als `BlueprintImplementableEvent` anbot.
- **`EnhancedInput`** (privat): nur die `.cpp` braucht die Input-Typen.
- **`WorldMap`** (privat): `ARtsWorldMap` für `MapRadius` und `NotifyWorldClicked`.

## Bekannte Baustellen

- `ComputeMovementScale` rechnet mit `ArmLength`, `AddMovement` mit `ArmLength - MapRadius`. Zwei
  Skalierungen, die dasselbe heißen und Verschiedenes ergeben; das Blueprint ruft beide.
- Die Kamerabewegung im Blueprint-Tick gehört nach C++ — Logik in einem Binärasset lässt sich hier
  weder lesen noch reviewen.
