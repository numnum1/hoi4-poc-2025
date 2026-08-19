# Modul `WorldMap`

Der Globus: die Kugel, die Kamera darauf, das Auflösen eines Klicks in eine Provinz und das Einfärben
der Provinzen. Mehr nicht — **das Modul entscheidet nichts**. Es weiß nicht, wem eine Provinz gehört,
welcher Kartenmodus aktiv ist oder was ein Klick bedeutet. All das wird in `RtsUI` entschieden und
hier hereingereicht.

## Rolle in der Schichtung

```
Mass-Fragmente + UXxxEntitySubsystem
  → UXxx (Fassaden auf ARtsGameState)
    → RtsFacades
      → RtsUI  ── Farben/Nationsdaten runter ──►  WorldMap
               ◄── Provinzklick hoch ────────────
```

`WorldMap` hängt **nicht** an `RtsUI` — `RtsUI` hängt privat an `WorldMap`. Eine Abhängigkeit in die
Gegenrichtung wäre ein Zyklus; deshalb kommen Farben und Nationsdaten ausschließlich als Push von
oben (`UWorldMapWidget::SetProvinceVisuals` / `SetNationData`) und Klicks ausschließlich als
Broadcast nach oben (`UWorldMapWidget::OnProvinceClicked`).

Die einzige Domänenabhängigkeit ist `Provinces` (privat, nur `.cpp`): beide Richtungen des
Provinz-Lookups laufen über `FProvinceDisplayInfo::MapId`, aufgelöst per
`FindRtsGameStateComponent<UProvinces>`.

## Datenfluss

**Klick rein:**

```
Mausklick  →  ARtsMapPawn::HandleSelectStarted
           →  TraceUnderCursor          (Linientrace auf das Globus-Mesh)
           →  ARtsWorldMap::VectorToUV  (Weltpunkt -> Karten-UV)
           →  ARtsWorldMap::ResolveProvinceAt
                → ReadLookupPixel       (Pixel aus ProvinceIdLookupTexture, CPU-Seite)
                → FindProvinceByColor   (R/G -> MapId -> UProvinces::FindByMapId)
           →  ARtsMapPawn::OnProvinceClicked
           →  UWorldMapWidget::OnProvinceClicked  →  RtsUI
```

**Farben raus:**

```
RtsUI (URtsHUDViewModel::GetProvinceVisuals)
  →  UWorldMapWidget::SetProvinceVisuals
  →  ARtsWorldMap::SetProvinceVisuals
       → FProvinceId -> FMapPixelId (ProvinceIdToPixelId)
       → WriteLutTexture auf Primary/Secondary/National-LUT (Render-Thread)
  →  Mat_Map_Province sampelt ProvID + die drei LUTs
```

## Klassen

### `ARtsWorldMap` (`RtsWorldMap.h`)

Der Kartenactor. Einer pro Level, gefunden über `GetRtsWorldMap(WorldContextObject)`. Acht
Zuständigkeiten liegen aktuell in dieser Klasse:

1. **Mesh- und Material-Setup** — `MapMesh`, `GlobeMesh`, `MapRadius`, `MapMaterials`,
   `MapDynamicMaterials`, `ConfigureMesh`, `RebuildDynamicMaterials`. Skaliert die Kugel auf
   `MapRadius` und hängt frische Dynamic-Material-Instanzen in die Mesh-Slots. Läuft bei jeder
   Konstruktion, damit Radius-/Mesh-Änderungen im Editor sofort sichtbar sind.

2. **UV-Projektion** — `VectorToUV` / `UVToLocation`. Kugelkoordinaten hin und zurück; die
   Umrechnung zwischen einem Trefferpunkt auf dem Globus und der Position in den Kartentexturen.

3. **Provinz-Lookup (Pixel -> Id)** — `ProvinceIdTexture` (nur GPU, vom Material gesampelt),
   `ProvinceIdLookupTexture` (nur CPU, für `ReadLookupPixel`), `ProvinceMapData` (die DataTable des
   BloodAnvil-Map-Editors). Rot und Grün eines Pixels tragen die Provinz-Id, Blau und Alpha sind
   ungenutzt. `IsLookupTextureReadable` prüft vor jedem Read die Texturkonfiguration, weil ein
   falsch eingestelltes Asset sonst still die falsche Provinz liefert statt gar keine.

4. **LUT-Malen** — `PrimaryLut` / `SecondaryLut` / `NationalLut` / `VfxLut` (`VfxLut` wird von
   niemandem beschrieben, ist aber gebunden, weil das Material es sampelt). `SetProvinceVisuals`
   nimmt ein **Änderungsset**, keine Weltbeschreibung: nicht genannte Provinzen behalten ihre Farbe,
   es wird nichts vorher geleert.

5. **Nachbarschaftsgraph** — `ProvinceAdjacencies`, `ProvincesAdjacentToUnknown`,
   `GenerateMapNameZones`. Gruppiert Provinzen in zusammenhängende Zonen, damit eine Nation auf zwei
   Landmassen zweimal beschriftet wird statt einmal quer über das Wasser.

6. **Unit-Positionen** — `FindUnitPositonOfProvince`, aus der `Unit Position`-Spalte der Map-Tabelle.
   Verankerungspunkte für alles, was auf einer Provinz sitzt; heute nur von der Label-Platzierung
   benutzt.

7. **Nationen-Besitz** — `NationNames` (per `SetNationData` hereingereicht),
   `FindOwnerNationIdForProvinceColor`. Nur damit `WorldMap` für ein Id-zu-Name-Lookup nicht am
   Nations-Modul hängen muss.

8. **Länder-Labels** — `GenerateCountryNames` / `GenerateCountryName` / `GenerateCountryNameForZone`.
   Zerlegt jede Nation in Zonen, bestimmt pro Zone Schwerpunkt und weitestes Punktepaar und spawnt
   darüber eine `ACountryName`.

Alle Lookups sind lazy (`Ensure*`) und `Transient`: sie werden aus `ProvinceMapData` neu aufgebaut,
weil das die einzige Stelle ist, an der diese Daten autorisiert werden. Die, die Provinz-Ids
auflösen, brauchen ein geladenes Szenario und bleiben bis dahin leer.

### `ARtsMapPawn` (`RtsMapPawn.h`)

Die Kamera auf dem Globus, und die Stelle, an der ein Klick zu einer Provinz wird. Bewegung ist
Rotation *um* die Kugel, nicht Translation; die Bewegungsskalierung hängt an der Kamerahöhe, damit
sie herangezoomt kriecht und herausgezoomt fegt.

Der Pawn hält **keine eigenen Kartendaten** — er fragt `ARtsWorldMap`. Die Kamerabewegung wird noch
vom Pawn-Blueprint aus getickt (`AddMovement`), der Klick dagegen ist nativ gebunden und verlässt
die Klasse über `OnProvinceClicked`.

### `UWorldMapWidget` (`WorldMapWidget.h`)

Die Schnittstelle der Karte zum Rest der UI, damit die Karte irgendwo in den Widget-Baum des HUD
gehängt werden kann, ohne dass der Platzierende den Kartenactor oder den Pawn kennen muss. Klicks
raus über `OnProvinceClicked`, Farben und Nationsdaten rein über `SetProvinceVisuals` /
`SetNationData`. Bindet sich in `NativeConstruct` an den Pawn, weil der vorher noch nicht besessen
sein muss.

### `ACountryName` (`CountryName.h`)

Ein Namenszug auf dem Globus. Reines Anzeigeobjekt: `ARtsWorldMap` entscheidet, wo ein Name
hingehört, spawnt eine dieser Instanzen und ruft `Initialize`; die Klasse hält nur, was sie bekommen
hat, und ruft `Refresh` (BlueprintImplementableEvent) fürs Rendern auf.

### Typen

| Typ | Datei | Wofür |
|---|---|---|
| `FMapPixelId` | `MapTypes.h` | R/G eines Provinz-ID-Pixels. Zugleich der Index in die LUTs — Rot wählt die Spalte, Grün die Zeile. |
| `FMapLutEntry` | `MapTypes.h` | Eine Provinzzeile über alle drei LUTs, damit die Ebenen nie über ihr Pixel uneins werden können. |
| `FMapNameZone` | `MapTypes.h` | Ein zusammenhängender Provinzblock für genau ein Label, plus dessen Randprovinzen. |
| `FWorldMapNationData` | `MapTypes.h` | Name + gehaltene Provinzen einer Nation. Das Minimum, das die Beschriftung braucht — der Grund, warum `WorldMap` nicht am Nations-Modul hängt. |
| `FProvinceVisuals` | `ProvinceVisuals.h` | Die drei Farbebenen einer Provinz. `bUseSecondary/NationalColor` aus heißt "folgt der Primärfarbe", nicht "schwarz". |

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

## Externe Abhängigkeiten

- **BloodAnvil-Plugin** (nicht in diesem Repository): das Zeilenformat `FProvinceMapDataGame`, der
  Pixel-Read `UBA_DataFunctions::FindColor`, sowie Mesh-/Material-/Input-Assets, die der Konstruktor
  per Pfad zieht. `ARtsWorldMap` und `ARtsMapPawn` leiteten früher von den Plugin-Klassen ab; das
  wurde hierher gezogen, damit der Code unter `Source/` versioniert und reviewbar ist. Ablösbar,
  sobald das Szenario die Tabelle beim Laden in Projektdaten überführt.
- **`Provinces`** (privat): `UProvinces::FindByMapId` für beide Richtungen des Lookups.

## Bekannte Baustellen

- `SetNationData` leert `ProvinceOwnerNationId` nicht, obwohl `EnsureNationOwnershipLookup` auf
  `IsEmpty()` guardet — nach dem ersten Besitzerwechsel liefert `FindOwnerNationIdForProvinceColor`
  dauerhaft veraltete Nationen.
- `SetNationData` ruft `GenerateCountryNames`, das `ACountryName`-Actors spawnt, ohne die vorherigen
  zu zerstören oder sich zu merken. `URtsHUDViewModel` broadcastet `GetNationData` bei jedem
  Provinzbesitzwechsel, also stapeln sich die Labels.
- Die Label-Pipeline (Punkt 8) ist unfertig: `GenerateCountryName` enthält einen `if (!true)`-
  Platzhalter für "diese Nation nicht beschriften", die Subject-Behandlung fehlt, und `SpawnMapName`
  ist ein `BlueprintImplementableEvent` — das Spawnen liegt damit im Binärasset statt in C++.
- Punkte 5-8 sind Ableitungen, keine Darstellung, und sitzen nur deshalb hier, weil ihre Quelle die
  Plugin-Tabelle ist. Der Klassenkommentar von `ARtsWorldMap` beschreibt noch den Stand davor.
- `UFoundOrganizationModalWidgetViewModel` malt über `ARtsWorldMap::GetRtsWorldMap` direkt, statt
  über `UWorldMapWidget` — der "einzige Schnittstelle"-Vertrag des Widgets gilt insofern nicht mehr.
