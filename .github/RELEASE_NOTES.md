# NDS-Shop (3DS) v1.0.0

Réécriture complète de l'application 3DS : une base C neuve (libctru,
citro2d, libcurl), sans aucune dépendance à l'ancienne base de code.

## Catalogue

- Catalogue paginé servi par l'API (`/api/v1/catalog`), révision en secondes.
- Cache local du catalogue : l'application reste utilisable hors-ligne.
- Recherche par titre ou par auteur.
- Atlas d'icônes téléchargé depuis l'API et mis en cache sur la carte SD.

## Installation des jeux

- Téléchargement de la ROM sur la carte SD (`sdmc:/roms/nds/`).
- Installation du forwarder `.cia` par le service AM de la console.
- Exécution des actions fournies par le catalogue (`downloadFile`,
  `installCia`, `deleteFile`), avec repli automatique lorsque le catalogue
  n'en publie aucune.
- Progression affichée pendant le téléchargement et l'installation, annulation
  possible avec **B**.

## Interface

- Interface entièrement maison, cohérente avec le site (fond clair, accent
  bleu, cartes à coins arrondis).
- Écrans : liste des jeux, détail, recherche, réglages, aide, progression,
  erreurs.
- Thème clair ou sombre.
- Français et anglais.
- Adresse d'API configurable depuis l'application ou via
  `sdmc:/3ds/NDS-Shop/config.txt`.

## Technique

- Périmètre limité aux jeux **Nintendo DS**.
- Visuels générés depuis des sources SVG (`tools/gen-assets.sh`).
- Tests du parseur de catalogue exécutables sur machine hôte
  (`make -C tests/host`).
- Intégration continue : tests, `.3dsx`, `.cia`, publication sur tag `v*`.
