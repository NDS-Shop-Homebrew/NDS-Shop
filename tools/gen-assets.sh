#!/usr/bin/env bash
# Generates the UI icon atlas from SVG sources.
#
#   tools/icons/*.svg          -> assets/gfx/ui/*.png
#   app/icon.png               -> assets/gfx/ui/logo.png (store icon)
#   assets/gfx/thirdparty/     -> copied as-is into the atlas frames
#                                (battery and Wi-Fi indicators)
#
# Distribution visuals (app/banner.png, app/icon.png, app/BannerAudio.wav,
# app/logo.bcma.lz) are committed files: this script never produces or
# overwrites them.
#
# Dependencies: rsvg-convert.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ICONS_DIR="$ROOT/tools/icons"
THIRDPARTY_DIR="$ROOT/assets/gfx/thirdparty"
OUT_ICONS="$ROOT/assets/gfx/ui"
T3S="$ROOT/assets/gfx/ui_gfx.t3s"

# Atlas frames, in the order expected by include/ui/assets.h (ICON_*).
FRAMES=(
  logo download settings search back check warning
  cartridge refresh globe moon up down help
  battery battery_charging net_0 net_1 net_2 net_3 net_discon
)

# Frames rendered from SVG sources.
SVG_ICONS=(
  download settings search back check warning
  cartridge refresh globe moon up down help
)

# Frames copied verbatim from third-party assets.
COPY_ICONS=(
  battery battery_charging net_0 net_1 net_2 net_3 net_discon
)

command -v rsvg-convert >/dev/null || { echo "rsvg-convert is required" >&2; exit 1; }

mkdir -p "$OUT_ICONS"

[ -f "$ROOT/app/icon.png" ] || { echo "missing app/icon.png" >&2; exit 1; }
cp "$ROOT/app/icon.png" "$OUT_ICONS/logo.png"

{
  echo "--atlas -f rgba -z auto"
  echo
  for name in "${FRAMES[@]}"; do
    echo "ui/$name.png"
  done
} > "$T3S"

for name in "${SVG_ICONS[@]}"; do
  src="$ICONS_DIR/$name.svg"
  [ -f "$src" ] || { echo "missing icon: $src" >&2; exit 1; }
  rsvg-convert -w 48 -h 48 "$src" -o "$OUT_ICONS/$name.png"
done

for name in "${COPY_ICONS[@]}"; do
  src="$THIRDPARTY_DIR/$name.png"
  [ -f "$src" ] || { echo "missing asset: $src" >&2; exit 1; }
  cp "$src" "$OUT_ICONS/$name.png"
done

echo "UI atlas generated:"
echo "  $T3S"
echo "  $OUT_ICONS/"
