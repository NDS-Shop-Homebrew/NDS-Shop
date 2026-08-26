# NDS-Shop

> An alternative Nintendo DS shop for the Nintendo DS, DSi and 3DS family of systems.

## Features

- Browse and search a curated database of NDS games
- Download `.nds` / `.cia` directly to SD card
- Multi-platform content download (DSi + 3DS)
- ZIP file extraction
- Database loading via URL or file
- Multiple database support
- Automatic update check
- Customizable color scheme
- Multi-language support (EN, FR, NL, IT)

## Download

### DSi

- Download `NDS-Shop.nds` from the [latest release](https://github.com/NDS-Shop-Homebrew/NDS-Shop/releases/latest).
- Place it anywhere on your SD card.
- Launch via TWiLight Menu++ or Unlaunch.

### 3DS

#### Requirements
- A modded Nintendo 3DS (XL) / 2DS (XL)
- An SD card with at least 500 MB for game installation

#### Latest release
- [latest release](https://github.com/NDS-Shop-Homebrew/NDS-Shop/releases/latest) — grab the `.cia` or `.3dsx` asset

## Building

### DSi

Requires [Wonderful Toolchain](https://wonderful.asie.pl/) + BlocksDS.

#### Install requirements

Follow the official BlocksDS [Windows setup guide](https://blocksds.skylyrac.net/docs/setup/windows/):

```bash
# Add BlocksDS repository
wf-pacman -Syu wf-tools
wf-config repo enable blocksds
wf-pacman -Syu

# Install ARM toolchain and BlocksDS
wf-pacman -S blocksds-toolchain

# Optional: docs and examples
wf-pacman -S blocksds-docs
```

#### Build

```bash
git clone --recursive https://github.com/NDS-Shop-Homebrew/NDS-Shop.git
cd NDS-Shop
python build.py
```

Use `python build.py release` to output into `DSi/release/`.
Or use `compile-dsi.bat` / `clean-dsi.bat` on Windows.

### 3DS

Requires devkitPro + libctru + 3ds-curl + 3ds-libarchive.

```bash
# Install dependencies
(dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive

# Clone and build
git clone --recursive https://github.com/NDS-Shop-Homebrew/NDS-Shop.git
cd NDS-Shop
make
```

Or use `compile-3ds.bat` / `clean-3ds.bat` on Windows.

## Credits

Based on [Kekatsu DS](https://github.com/cavv-dev/Kekatsu-DS) by cavv.
Database: [UDB-Kekatsu-DS](https://github.com/cavv-dev/UDB-Kekatsu-DS).
Concept inspired by [pkgi-psp](https://github.com/bucanero/pkgi-psp) and [Universal-Updater](https://github.com/Universal-Team/Universal-Updater).
