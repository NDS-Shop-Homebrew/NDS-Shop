# NDS-Shop

Alternative shop for Nintendo DS / DSi — browse, search, and download homebrew games directly from your console.

## Supported platforms

- **Nintendo DSi** (DSi Shop clone)
- **Nintendo 3DS** (eShop clone)

## Features

- Browse and search a curated database of homebrew games
- Download `.nds` / `.cia` directly to SD card
- Multi-language support (EN, FR, NL, IT)
- Auto-update from GitHub releases

## Building

### DSi

Requires [Wonderful Toolchain](https://wonderful.asie.pl/) + BlocksDS.

```bash
# From the Wonderful Toolchain Shell
cd DSi
python build.py
```

Or use `compile-dsi.bat` / `clean-dsi.bat` on Windows.

### 3DS

Requires devkitPro + libctru.

```bash
cd 3DS
make
```

Or use `compile-3ds.bat` / `clean-3ds.bat` on Windows.

## Credits

Based on [Kekatsu DS](https://github.com/cavv-dev/Kekatsu-DS) by cavv.
