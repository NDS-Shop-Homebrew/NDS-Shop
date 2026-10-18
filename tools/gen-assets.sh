#!/usr/bin/env bash
# Génère les visuels de l'application NDS-Shop à partir des sources SVG.
#
#   tools/icons/*.svg  -> assets/gfx/ui/*.png  (+ assets/gfx/ui_gfx.t3s)
#   tools/brand/*.svg  -> app/banner.png, app/icon.png
#   app/BannerAudio.wav -> généré par tools/gen-banner-audio.py
#
# Dépendances : rsvg-convert, python3.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ICONS_DIR="$ROOT/tools/icons"
BRAND_DIR="$ROOT/tools/brand"
OUT_ICONS="$ROOT/assets/gfx/ui"
T3S="$ROOT/assets/gfx/ui_gfx.t3s"

# Ordre des cadres : il doit correspondre aux constantes de source/assets.h.
ICONS=(
  logo download settings search back check warning
  cartridge refresh globe moon up down help
)

command -v rsvg-convert >/dev/null || { echo "rsvg-convert est requis" >&2; exit 1; }

mkdir -p "$OUT_ICONS"

{
  echo "--atlas -f rgba -z auto"
  echo
  for name in "${ICONS[@]}"; do
    src="$ICONS_DIR/$name.svg"
    [ -f "$src" ] || { echo "icône manquante : $src" >&2; exit 1; }
    rsvg-convert -w 48 -h 48 "$src" -o "$OUT_ICONS/$name.png"
    echo "ui/$name.png"
  done
} > "$T3S"

rsvg-convert -w 256 -h 128 "$BRAND_DIR/banner.svg" -o "$ROOT/app/banner.png"
rsvg-convert -w 48 -h 48 "$BRAND_DIR/icon.svg" -o "$ROOT/app/icon.png"

python3 "$ROOT/tools/gen-banner-audio.py" "$ROOT/app/BannerAudio.wav"

echo "Assets générés :"
echo "  $T3S"
echo "  $OUT_ICONS/"
echo "  $ROOT/app/banner.png, $ROOT/app/icon.png, $ROOT/app/BannerAudio.wav"
