#!/usr/bin/env bash
# Génère l'atlas d'icônes de l'interface à partir des sources SVG.
#
#   tools/icons/*.svg  -> assets/gfx/ui/*.png (+ assets/gfx/ui_gfx.t3s)
#   app/icon.png       -> assets/gfx/ui/logo.png (copie : le logo affiché dans
#                         l'application est l'icône officielle de la boutique)
#
# Les visuels de la distribution (app/banner.png, app/icon.png,
# app/BannerAudio.wav, app/logo.bcma.lz) sont des fichiers du dépôt : ce script
# ne les produit pas et ne les écrase jamais.
#
# Dépendances : rsvg-convert.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ICONS_DIR="$ROOT/tools/icons"
OUT_ICONS="$ROOT/assets/gfx/ui"
T3S="$ROOT/assets/gfx/ui_gfx.t3s"

# Cadres de l'atlas, dans l'ordre attendu par source/assets.h (ICON_*).
FRAMES=(
  logo download settings search back check warning
  cartridge refresh globe moon up down help
)

# Cadres produits depuis les SVG (le logo, lui, vient de app/icon.png).
SVG_ICONS=(
  download settings search back check warning
  cartridge refresh globe moon up down help
)

command -v rsvg-convert >/dev/null || { echo "rsvg-convert est requis" >&2; exit 1; }

mkdir -p "$OUT_ICONS"

[ -f "$ROOT/app/icon.png" ] || { echo "app/icon.png manquant" >&2; exit 1; }
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
  [ -f "$src" ] || { echo "icône manquante : $src" >&2; exit 1; }
  rsvg-convert -w 48 -h 48 "$src" -o "$OUT_ICONS/$name.png"
done

echo "Atlas d'interface généré :"
echo "  $T3S"
echo "  $OUT_ICONS/"
