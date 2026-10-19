# On-console test (Nintendo 3DS)

Acceptance sheet for the application on real hardware. Off-console tests
(`make -C tests/host`) cover catalog parsing and configuration; this sheet
covers what can only be verified on the device: network access through the SOC
service, title installation through the AM service, SD card writes, on-screen
rendering and game shortcut behavior.

## Requirements

- Nintendo 3DS or 2DS with a recent custom firmware (Luma3DS) allowing the
  installation of unsigned titles.
- A title manager (FBI or equivalent) to install the `.cia`.
- An SD card with roughly 100 MB free per game tested.
- A server reachable from the console exposing the catalog's `/api/v1` API
  (local or online instance).

## Getting the binary

| Method | How |
| --- | --- |
| CI artifact | **Actions** tab → "Build NDS-Shop (3DS)" workflow → latest run → `nds-shop-3ds` artifact (contains `NDS-Shop.cia` and `NDS-Shop.3dsx`). The workflow can also be triggered manually with *Run workflow*. |
| Local build | `make` produces `NDS-Shop.3dsx`; `make cia` produces `NDS-Shop.cia` (requires `bannertool` and `makerom`). |

CI artifacts expire after 90 days; rebuild if unavailable.

## API address

On first launch the app points at `https://api.db-nds-shop.fr`. To target
another instance, open **Settings** (**Y** button) and enter the address.

Enter the **origin only**, without trailing slash and without `/api` (the app
appends `/api/v1/...` itself).

| Input | Address actually called |
| --- | --- |
| `https://example.fr` | `https://example.fr/api/v1/catalog` |
| `https://example.fr/` | same (trailing slash ignored) |
| `https://example.fr/api` | same (redundant `/api` suffix ignored) |
| `http://local-host:3000` | `http://local-host:3000/api/v1/catalog` |

## SD card preparation

Nothing to create by hand: the app makes its own folders. To start from a
clean state, delete:

- `sdmc:/3ds/NDS-Shop/`: configuration, catalog cache, icon atlas;
- `sdmc:/roms/nds/`: downloaded Nintendo DS ROMs.

## Scenarios

| # | Action | Expected result |
| --- | --- | --- |
| 1 | First launch, Wi-Fi off | **Error** screen with an explicit message; `A` retries. No hang. Header shows the disconnected/red Wi-Fi icon. |
| 2 | Wi-Fi back on, `A` | Loading screen, then the game list: thumbnails visible, footer with store title and game count. Header shows battery percentage and Wi-Fi strength bars. |
| 3 | Inspect the SD card | `sdmc:/3ds/NDS-Shop/` contains `config.txt` and `catalog.json`; after icons load, `atlas.t3x` and `atlas.url`. |
| 4 | `X`, type a title then an author | The list filters, the counter follows, `B` clears the search. |
| 5 | `Y`: change language, theme, then restart | Settings are preserved (check `config.txt`). |
| 6 | `Y`: enter a wrong address (`https://example.invalid`) | Readable **Error** screen, no crash; a correct address restores the catalog. |
| 7 | `A` on a game, let the install finish | Progress shown (percentage), live speed (MB/s) and ETA while downloading, then a completion message. |
| 8 | Inspect the SD card after install | The ROM is in `sdmc:/roms/nds/`; **no leftover `.cia`** on the card. |
| 9 | Return to the console HOME menu | The game shortcut is present and launches (Nintendo DS game in retrocompatible mode). |
| 10 | Restart an install then press `B` mid-download | The task stops, the partial file is deleted, back to the list. |
| 11 | Wi-Fi off, then relaunch the app | **Offline** mention; the list comes from `catalog.json`. |
| 12 | Plug the charger during any screen | Battery indicator switches to the charging icon with an amber fill. |
| 13 | `START` | Clean return to the console HOME menu. |
| 14 | (Optional) Reinstall an already-present game | Installation runs without error, no leftover temporary files. |

## On failure

Record, in this order:

1. the screen shown and the **exact error message** (photo);
2. the contents of `sdmc:/3ds/NDS-Shop/config.txt`;
3. the list and sizes of the files in `sdmc:/3ds/NDS-Shop/` and
   `sdmc:/roms/nds/`;
4. the scenario number and the binary used (artifact or local build).

## Known issues

- **TLS**: server certificate verification is disabled in the app (no
  certificate authority is embedded). Traffic stays encrypted but is not
  authenticated.
- **Offline cache**: only the first page of the catalog is kept (up to 200
  games).
- **Without custom firmware**: only the `.3dsx` is usable; installing a
  shortcut will fail.
- **Missing atlas**: if the icon atlas cannot be downloaded, generic tiles
  replace the thumbnails; the rest of the app keeps working.
