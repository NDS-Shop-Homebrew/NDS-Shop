# NDS-Shop (Nintendo 3DS)

Application homebrew pour Nintendo 3DS qui installe des jeux **Nintendo DS**
depuis le catalogue de [NDS-Shop](https://db-nds-shop.fr).

L'application télécharge la ROM sur la carte SD, puis installe un **raccourci**
(forwarder `.cia`) sur le menu d'accueil : le jeu se lance ensuite comme
n'importe quel titre installé.

Le périmètre est volontairement limité aux **jeux Nintendo DS**. Aucun contenu
3DS, GBA ou DSi n'est distribué.

## Fonctionnalités

- Catalogue paginé récupéré depuis l'API (`/api/v1/catalog`), avec cache local
  pour un usage hors-ligne.
- Recherche par titre ou auteur.
- Installation en un geste : téléchargement de la ROM, installation du
  forwarder, nettoyage des fichiers temporaires.
- Atlas d'icônes téléchargé depuis l'API (aucune icône de jeu embarquée).
- Thème clair/sombre et interface en français ou en anglais.
- Adresse d'API configurable, pour pointer vers une autre instance du
  catalogue.

## Installation

1. Télécharger `NDS-Shop.cia` depuis les [versions](../../releases) et
   l'installer avec un gestionnaire de titres.
2. Ou bien copier `NDS-Shop.3dsx` dans `sdmc:/3ds/` et le lancer depuis
   l'explorateur homebrew.

Un custom firmware récent est nécessaire (installation de titres non signés).

## Utilisation

| Commande | Action |
| --- | --- |
| Croix directionnelle | Parcourir la liste |
| A | Installer le jeu sélectionné |
| X | Rechercher |
| Y | Réglages |
| SELECT | Aide |
| L / R | Faire défiler la description |
| B | Retour / annuler une installation |
| START | Quitter |

## Configuration

Le fichier `sdmc:/3ds/NDS-Shop/config.txt` est créé au premier lancement :

```ini
# Configuration NDS-Shop
api=https://api.db-nds-shop.fr
lang=fr
theme=light
```

L'adresse de l'API se modifie aussi depuis l'écran Réglages (bouton **Y**).
Les fichiers téléchargés sont rangés dans :

- `sdmc:/3ds/NDS-Shop/` : configuration, cache du catalogue, atlas d'icônes ;
- `sdmc:/roms/nds/` : ROM Nintendo DS.

## Compilation

Prérequis : [devkitPro](https://devkitpro.org/) avec `devkitARM`, `libctru`,
`citro2d` **et les portlibs 3DS**. Sans ces dernières, la compilation s'arrête
sur `fatal error: curl/curl.h: No such file or directory`.

Avec le dépôt `dkp-libs` configuré (Arch Linux) :

```sh
sudo pacman -S --needed devkitARM libctru citro2d citro3d \
                        3ds-curl 3ds-mbedtls 3ds-zlib
```

Avec l'installeur devkitPro (`dkp-pacman`) :

```sh
sudo dkp-pacman -S 3ds-dev 3ds-portlibs
```

Sans rien installer, l'image officielle `devkitpro/devkitarm` contient déjà tout
(c'est celle de la CI) :

```sh
docker run --rm -v "$PWD:/app" -w /app devkitpro/devkitarm make
```

Ensuite :

```sh
make        # NDS-Shop.3dsx
make cia    # NDS-Shop.cia (nécessite bannertool et makerom)
make clean
```

La version intégrée à l'application provient du dernier tag git (`vX.Y.Z`) ;
à défaut, `1.0.0` est utilisé.

### Visuels

Les visuels de la distribution (bannière, icône de titre, logo d'en-tête,
audio du bandeau) sont les fichiers officiels conservés dans `app/` :
`banner.png`, `icon.png`, `logo.bcma.lz` et `BannerAudio.wav`. Ils ne sont
jamais regénérés, seulement repris tels quels par `make cia`.

L'atlas d'icônes de l'interface se régénère depuis les sources SVG
(`tools/icons/*.svg`) ; le cadre du logo est une copie de `app/icon.png`, afin
que le logo affiché dans l'application soit exactement l'icône de la boutique :

```sh
tools/gen-assets.sh   # rsvg-convert
```

### Tests

Le parseur de catalogue et la configuration se testent sans console :

```sh
make -C tests/host
```

La recette sur console (installation, catalogue, raccourcis de jeux) est décrite
dans [docs/TEST-3DS.md](docs/TEST-3DS.md).

## Organisation du code

| Fichier | Rôle |
| --- | --- |
| `source/main.c` | Boucle principale, entrées, actions |
| `source/ui.c` | Rendu citro2d (thème, écrans) |
| `source/net.c` | Requêtes HTTP (libcurl) |
| `source/catalog.c` | Chargement et analyse du catalogue |
| `source/json.c` | Analyseur JSON minimal |
| `source/atlas.c` | Atlas d'icônes de la boutique (cache SD) |
| `source/cia.c` | Installation d'un titre via le service AM |
| `source/scripts.c` | Exécution des actions du catalogue |
| `source/fsutil.c` | Carte SD et configuration |

## Périmètre et retrait de contenu

L'application ne publie aucun contenu : elle consomme un catalogue administré
par l'équipe de NDS-Shop. Les demandes de retrait sont traitées via la page
DMCA du site.

## Licence

GNU General Public License v3.0 — voir [LICENSE](LICENSE).
