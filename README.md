# NDS-Shop (Nintendo 3DS)

Homebrew application for Nintendo 3DS that installs **Nintendo DS** games from
the [NDS-Shop](https://db-nds-shop.fr) catalog.

The app downloads the ROM to the SD card, then installs a **shortcut**
(forwarder `.cia`) on the HOME menu: the game then launches like any installed
title.

Scope is deliberately limited to **Nintendo DS games**. No 3DS, GBA or DSi
content is distributed.

## Features

- Paginated catalog fetched from the API (`/api/v1/catalog`), with a local
  cache for offline use.
- Search by title or author.
- One-press install: ROM download, then an optional prompt to install the HOME
  menu shortcut (forwarder), followed by temporary file cleanup.
- Icon atlas downloaded from the API (no game icons embedded).
- Live battery and Wi-Fi signal indicators in the header.
- Real-time download telemetry (speed and ETA) during installation.
- Dark theme and an English or French interface.
- Configurable API address, to point at another instance of the catalog.

## Installation

1. Download `NDS-Shop.cia` from the [releases](../../releases) page and
   install it with a title manager.
2. Or copy `NDS-Shop.3dsx` to `sdmc:/3ds/` and launch it from the homebrew
   launcher.

A recent custom firmware is required (installing unsigned titles).

## Usage

| Button | Action |
| --- | --- |
| D-pad | Browse the list |
| A | Install the selected game |
| X | Search |
| Y | Settings |
| SELECT | Help |
| L / R | Scroll the description |
| B | Back / cancel an install |
| START | Quit |

## Configuration

The file `sdmc:/3ds/NDS-Shop/config.txt` is created on first launch:

```ini
# NDS-Shop configuration
api=https://api.db-nds-shop.fr
lang=en
```

The API address can also be changed from the Settings screen (**Y** button).
Downloaded files are stored in:

- `sdmc:/3ds/NDS-Shop/`: configuration, catalog cache, icon atlas;
- `sdmc:/roms/nds/`: Nintendo DS ROMs.

## Building

Requirements: [devkitPro](https://devkitpro.org/) with `devkitARM`, `libctru`,
`citro2d` **and the 3DS portlibs**. Without the portlibs, the build stops with
`fatal error: curl/curl.h: No such file or directory`.

With the `dkp-libs` repository configured (Arch Linux):

```sh
sudo pacman -S --needed devkitARM libctru citro2d citro3d \
                        3ds-curl 3ds-mbedtls 3ds-zlib
```

With the devkitPro installer (`dkp-pacman`):

```sh
sudo dkp-pacman -S 3ds-dev 3ds-portlibs
```

Without installing anything, the official `devkitpro/devkitarm` image already
contains everything (it is the one used by CI):

```sh
docker run --rm -v "$PWD:/app" -w /app devkitpro/devkitarm make
```

Then:

```sh
make        # NDS-Shop.3dsx
make cia    # NDS-Shop.cia (requires bannertool and makerom)
make clean
```

The version embedded in the app comes from the latest git tag (`vX.Y.Z`);
otherwise `1.0.0` is used.

### Visuals

Distribution visuals (banner, title icon, header logo, banner audio) are the
official files kept in `app/`: `banner.png`, `icon.png`, `logo.bcma.lz` and
`BannerAudio.wav`. They are never regenerated, only reused as-is by `make cia`.

The UI icon atlas is regenerated from SVG sources (`tools/icons/*.svg`); the
logo frame is a copy of `app/icon.png`, so the logo shown in the application is
exactly the store icon. The battery and Wi-Fi indicator frames live in
`assets/gfx/thirdparty/`:

```sh
tools/gen-assets.sh   # rsvg-convert
```

### Tests

The catalog parser and the configuration layer are testable without a console:

```sh
make -C tests/host
```

The on-console recipe (installation, catalog, game shortcuts) is described in
[docs/TEST-3DS.md](docs/TEST-3DS.md).

## Code organization

| Folder | Role |
| --- | --- |
| `source/core/` | Main loop, SD card/config, translations, battery/Wi-Fi |
| `source/ui/` | citro2d rendering (theme, screens) |
| `source/catalog/` | Catalog, JSON, icon atlas (SD cache) |
| `source/net/` | HTTP requests (httpc for files, libcurl for JSON), download speed |
| `source/tasks/` | .cia installation, catalog action runner |

Matching headers live in `include/` with the same layout.

## Scope and content removal

The application publishes no content: it consumes a catalog administered by
the NDS-Shop team. Takedown requests are handled through the site's DMCA page.

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).
