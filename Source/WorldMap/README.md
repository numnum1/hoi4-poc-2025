# Modul `WorldMap`

Der Globus: die Kugel, das Einfärben der Provinzen, das Auflösen eines Klicks in eine Provinz und
die Namenszüge der Länder. Mehr nicht — **das Modul entscheidet nichts**. Es weiß nicht, wem eine
Provinz gehört, welcher Kartenmodus aktiv ist oder was ein Klick bedeutet. All das wird in `RtsUI`
entschieden und hier hereingereicht.

## Rolle in der Schichtung

```
Mass-Fragmente + UXxxEntitySubsystem
  → UXxx (Fassaden auf ARtsGameState)
    → RtsFacades
      → RtsUI  ── Farben/Nationsdaten runter ──►  WorldMap
               ◄── Provinzklick hoch ────────────

MapCamera  ── Trefferpunkt ──►  WorldMap
```

`WorldMap` hängt **nicht** an `RtsUI` — `RtsUI` hängt privat an `WorldMap`. Eine Abhängigkeit in die
Gegenrichtung wäre ein Zyklus; deshalb kommen Farben und Nationsdaten ausschließlich als Push von
oben (`UWorldMapWidget::SetProvinceVisuals` / `SetNationData`) und Klicks ausschließlich als
Broadcast nach oben (`UWorldMapWidget::OnProvinceClicked`). Dieselbe Einwegrichtung gilt zu
`MapCamera`: der Pawn ruft `ARtsWorldMap::NotifyWorldClicked`, die Karte kennt das Kameramodul nicht.

Die einzige Domänenabhängigkeit ist `Provinces` (privat, nur `.cpp`): beide Richtungen des
Provinz-Lookups laufen über `FProvinceDisplayInfo::MapId`, aufgelöst per
`FindRtsGameStateComponent<UProvinces>`.

## Der Schnitt: Pixel unten, Provinzen oben

Der Kartenactor kennt **keine `FProvinceId`**. Er malt Zeilen, die schon nach Pixel indiziert sind,
und meldet beim Klick den Pixel, den er gelesen hat. Übersetzt wird ausschließlich in
`UWorldMapSubsystem`, und zwar in der Schicht darüber:

```
URtsWorldMapViewModel ── FMapLutEntry (pixelindiziert) ──►  ARtsWorldMap
                      ◄── FMapPixelId ─────────────────────
        │
        └── UWorldMapSubsystem  (FProvinceId ↔ FMapPixelId, Adjazenz, Unit-Positionen)
```

Das ist die Trennlinie, an der das ganze Modul hängt: **Actor = Textur und Geometrie,
Subsystem = Tabelle und Id, ViewModel = was gezeigt wird, Widget = die Tür nach draußen.**

## Datenfluss

**Klick rein:**

```
Mausklick  →  ARtsMapPawn::HandleSelectStarted        (Modul MapCamera)
           →  TraceUnderCursor                        (Linientrace auf das Globus-Mesh)
           →  ARtsWorldMap::NotifyWorldClicked(FHitResult)
                → MapProjection::VectorToUV           (Weltpunkt → Karten-UV)
                → ReadLookupPixel                     (Pixel aus ProvinceIdLookupTexture, CPU-Seite)
                → OnPixelClicked.Broadcast(FMapPixelId)
           →  UWorldMapWidget::HandlePixelClicked
                → UWorldMapSubsystem::FindProvince    (Pixel → MapId → FProvinceId)
           →  UWorldMapWidget::OnProvinceClicked      →  RtsUI
```

**Farben raus:**

```
RtsUI (URtsHUDViewModel::GetProvinceVisuals über URtsMapModes)
  →  URtsHUDWidget pusht in sein Kindwidget
  →  UWorldMapWidget::SetProvinceVisuals
  →  URtsWorldMapViewModel::SetProvinceVisuals
       → UWorldMapSubsystem::BuildLutRows            (FProvinceId → FMapPixelId)
  →  ARtsWorldMap::PaintProvinces
       → WriteLutTexture auf Primary/Secondary/National-LUT (Render-Thread)
  →  Mat_Map_Province sampelt ProvID + die drei LUTs
```

**Labels raus:**

```
RtsUI (URtsHUDViewModel::GetNationData)
  →  UWorldMapWidget::SetNationData
  →  URtsWorldMapViewModel::SetNationData
       → Pixel→Nation-Tabelle  →  ARtsWorldMap::SetProvinceOwners
       → BuildLabels: Zonen (Subsystem) + Schwerpunkt + weitestes Punktepaar  →  UV-Punkte
  →  ARtsWorldMap::SpawnCountryLabels                (UV → Weltpunkt, SpawnMapName, Initialize)
  →  ACountryName + CountryName_BP zeichnen den Namenszug
```

## Klassen

### `ARtsWorldMap` (`RtsWorldMap.h`)

Der Kartenactor, funktional eine UI-Komponente: er malt, mehr nicht. Einer pro Level, gefunden über
`GetRtsWorldMap(WorldContextObject)`.

- **Mesh und Material** — `MapMesh`, `GlobeMesh`, `MapRadius`, `MapMaterials`, `MapDynamicMaterials`,
  `ConfigureMesh`, `RebuildDynamicMaterials`. Skaliert die Kugel auf `MapRadius` und hängt frische
  Dynamic-Material-Instanzen in die Mesh-Slots. Läuft bei jeder Konstruktion, damit Radius- und
  Mesh-Änderungen im Editor sofort sichtbar sind.
- **LUT-Malen** — `PrimaryLut` / `SecondaryLut` / `NationalLut` / `VfxLut`, `EnsureLuts`,
  `PaintProvinces`, `PaintProvinceVfx`. Nimmt ein **Änderungsset**, keine Weltbeschreibung: nicht
  genannte Zeilen behalten ihre Farbe, es wird nichts vorher geleert. `VfxLut` läuft bewusst über
  einen eigenen Aufruf: dort steht nur das Hover-Highlight, und das ändert sich mit der Maus, während
  die drei Farbschichten sich mit der Simulation ändern.
- **Pixel lesen** — `ProvinceIdTexture` (nur GPU, vom Material gesampelt), `ProvinceIdLookupTexture`
  (nur CPU), `ReadLookupPixel`, `IsLookupTextureReadable`, `NotifyWorldClicked`. Rot und Grün eines
  Pixels tragen die Provinz-Id, Blau und Alpha sind ungenutzt. `IsLookupTextureReadable` prüft vor
  jedem Read die Texturkonfiguration, weil ein falsch eingestelltes Asset sonst still die falsche
  Provinz liefert statt gar keine.
- **Projektion** — `VectorToUV` / `UVToLocation`, beides dünne Weiterleitungen an `MapProjection`.
  Bleiben als `BlueprintPure` bestehen, weil `CountryName_BP` sie ruft.
- **Labels spawnen** — `SpawnCountryLabels` projiziert die hereingereichten UV-Punkte und ruft
  `SpawnMapName` (BlueprintImplementableEvent, implementiert in `RtsMap_BP`).
- **Besitz-Nachschlag für das Label-BP** — `SetProvinceOwners` / `FindOwnerNationIdForProvinceColor`.
  Das ViewModel reicht eine fertige Pixel→Nation-Tabelle herunter; der Actor schlägt nur nach. Das
  ist der Grund, warum der Actor Nationen kennt, ohne Provinzen zu kennen.

`ProvinceMapData` liegt weiterhin als `EditAnywhere`-Referenz hier, weil das Level die Stelle ist, an
der ausgewählt wird, welche Welt geladen ist — gelesen wird sie aber vom Subsystem, dem der Actor sie
per `ConfigureSubsystem` überreicht.

### `UWorldMapSubsystem` (`WorldMapSubsystem.h`)

Alles, was Tabelle und Id ist: `MapIdToPixelId`, beide Richtungen von
`FProvinceId` ↔ `FMapPixelId`, der Nachbarschaftsgraph, die Unit-Positionen und `HighestPixelRow`.

- `BuildLutRows` — die Übersetzung, durch die jedes Malen läuft.
- `FindProvince` / `FindPixelId` — die beiden Richtungen einzeln.
- `FindUnitPosition` — der Verankerungspunkt einer Provinz aus der `Unit Position`-Spalte.
- `BuildNameZones` — gruppiert Provinzen in zusammenhängende Zonen, damit eine Nation auf zwei
  Landmassen zweimal beschriftet wird statt einmal quer über das Wasser.
- `HasMapData` — die Tabelle allein reicht, um die LUTs zu dimensionieren; Provinz-Ids brauchen
  zusätzlich ein geladenes Szenario.

Alle Lookups sind lazy (`Ensure*`) und werden gecacht. Die, die Provinz-Ids auflösen, bleiben leer,
bis ein Szenario geladen ist, und werden bis dahin bei jedem Zugriff neu versucht.

### `URtsWorldMapViewModel` (`RtsWorldMapViewModel.h`)

Was die Karte zeigt. Gehört `UWorldMapWidget`, das es erzeugt und befüllt. Hält `ProvinceVisuals`
und `NationData` als gepushten Zustand, übersetzt sie über das Subsystem und ruft den Actor.

Die Label-Berechnung sitzt hier: `BuildLabels` zerlegt jede Nation in Zonen und `BuildLabelForZone`
bestimmt Schwerpunkt und weitestes Punktepaar. Das Punktepaar wird auf der **Einheitskugel**
verglichen — das Skalieren aller Abstände mit dem Radius kann die Reihenfolge nicht ändern, also
kommt die Berechnung ohne `MapRadius` aus.

Es hält **nur Rendering-State**. Welche Provinz ausgewählt ist, warum ein Klick zählt, welches
Fenster er öffnet — nichts davon gehört hierher, das liegt in `RtsUI`. Es hat auch **keine
Dispatcher**: als `CreateInstance`-ViewModel hat es kein Eltern-ViewModel, das einen binden könnte
(siehe `### Callbacks gibt es nur auf Manual-ViewModels` in `CLAUDE.md`).

### `UWorldMapWidget` (`WorldMapWidget.h`)

Die Schnittstelle der Karte zum Rest der UI, damit die Karte irgendwo in den Widget-Baum des HUD
gehängt werden kann, ohne dass der Platzierende Kartenactor, Subsystem oder Pawn kennen muss.

- Erzeugt sein ViewModel in `NativeOnInitialized` und gibt es über `GetWorldMapViewModel` heraus —
  das Blueprint bindet mit Creation Type „Property Path" darauf, nie mit „Create Instance".
- Farben und Nationsdaten rein → ViewModel.
- Pixel-Klick des Actors rein, über das Subsystem übersetzt, als `OnProvinceClicked` raus.

### `ACountryName` (`CountryName.h`)

Ein Namenszug auf dem Globus. Reines Anzeigeobjekt: es hält, was es bekommen hat, und ruft `Refresh`
(BlueprintImplementableEvent) fürs Rendern. `CountryName_BP` tastet sich dabei selbst über die Kugel
und fragt pro Punkt `ARtsWorldMap::FindOwnerNationIdForProvinceColor`, ob dort noch dieselbe Nation
liegt.

### Typen

| Typ | Datei | Wofür |
|---|---|---|
| `FMapPixelId` | `MapTypes.h` | R/G eines Provinz-ID-Pixels. Zugleich der Index in die LUTs — Rot wählt die Spalte, Grün die Zeile. `FromColor` wirft Blau und Alpha weg. |
| `FMapLutEntry` | `MapTypes.h` | Eine Provinzzeile über alle drei LUTs, damit die Ebenen nie über ihr Pixel uneins werden können. |
| `FMapNameZone` | `MapTypes.h` | Ein zusammenhängender Provinzblock für genau ein Label, plus dessen Randprovinzen. |
| `FWorldMapNationData` | `MapTypes.h` | Name + gehaltene Provinzen einer Nation. Das Minimum, das die Beschriftung braucht — der Grund, warum `WorldMap` nicht am Nations-Modul hängt. |
| `FWorldMapLabel` | `MapTypes.h` | Ein fertiges Label: UV-Punkte, Text, Owner-Tags. UVs statt Weltpunkte, damit die Berechnung ohne `MapRadius` auskommt. |
| `FProvinceVisuals` | `ProvinceVisuals.h` | Die drei Farbebenen einer Provinz. `bUseSecondary/NationalColor` aus heißt „folgt der Primärfarbe", nicht „schwarz". |
| `MapProjection` | `MapProjection.h` | Kugelkoordinaten hin und zurück, als freie Funktionen. Der Radius ist ein Parameter, kein Zustand. |

## Verträge, die nicht im Code stehen

- **Die LUT-Höhe ist nicht frei wählbar.** `Mat_Map` leitet die Zeile aus der *Texturhöhe selbst* ab.
  Die LUTs sind deshalb exakt `HighestPixelRow + 1` hoch — auf 256 aufgerundet sampelt jede Provinz
  eine leere Zeile und die Karte bleibt farblos. Breite ist immer 256 (der Wertebereich von Rot).
- **`ProvinceIdLookupTexture` braucht bestimmte Importeinstellungen**: Compression
  `VectorDisplacementmap` (RGBA8), sRGB aus, keine Mipmaps, `Never Stream`. Sonst existiert die
  Bulk-Data auf CPU-Seite nicht oder wird als Blockdaten fehlinterpretiert. `IsLookupTextureReadable`
  meldet jeden dieser Fälle einmalig mit der konkret nötigen Einstellung.
- **`ProvinceIdTexture` und `ProvinceIdLookupTexture` sind dasselbe Bild in zwei Ausprägungen** und
  müssen von Hand synchron gehalten werden. Die Auflösungen dürfen abweichen (gesampelt wird über
  UVs), das Layout nicht.
- **Die Zeilennamen in `ProvinceMapData` sind die `MapId`** der Provinzen ("1893"), nicht das
  `Name`-Feld der Zeile ("Arnhem").
- **`CountryName_BP` ruft in den Actor zurück** — `FindOwnerNationIdForProvinceColor`, `VectorToUV`
  und `MapRadius`. Diese drei sind ein Blueprint-Vertrag und dürfen nicht umbenannt werden.
  `RtsMap_BP` implementiert `SpawnMapName`.

## Externe Abhängigkeiten

- **BloodAnvil-Plugin** (nicht in diesem Repository): das Zeilenformat `FProvinceMapDataGame` (vom
  Subsystem gelesen), der Pixel-Read `UBA_DataFunctions::FindColor`, sowie Mesh- und
  Material-Assets, die der Konstruktor per Pfad zieht. `ARtsWorldMap` leitete früher von der
  Plugin-Klasse ab; das wurde hierher gezogen, damit der Code unter `Source/` versioniert und
  reviewbar ist. Ablösbar, sobald das Szenario die Tabelle beim Laden in Projektdaten überführt.
- **`Provinces`** (privat): `UProvinces::FindByMapId` für beide Richtungen des Lookups.

## Bekannte Baustellen

- `SpawnCountryLabels` zerstört die vorherigen `ACountryName`-Actors nicht. `URtsHUDViewModel`
  broadcastet `GetNationData` bei jedem Provinzbesitzwechsel, also stapeln sich die Labels.
  Bewusst unverändert übernommen — die Label-Pipeline wird als Ganzes angefasst, nicht nebenbei.
- Die Label-Pipeline ist unfertig: Es gibt keine Regel dafür, welche Nationen ungenannt bleiben (zu
  klein, Subjekt eines Overlords), die Provinzen eines Subjekts werden nicht in die Zonen des
  Overlords gefaltet, und `SpawnMapName` ist ein `BlueprintImplementableEvent` — das Spawnen liegt
  damit im Binärasset statt in C++.
- `UFoundOrganizationModalWidgetViewModel` malt über `ARtsWorldMap::GetRtsWorldMap` +
  `UWorldMapSubsystem` direkt, statt über `UWorldMapWidget` — der „einzige Schnittstelle"-Vertrag
  des Widgets gilt insofern nicht mehr.
