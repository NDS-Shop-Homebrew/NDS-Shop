# NDS-Shop

> Un magasin alternatif pour Nintendo DS, conçu pour les consoles Nintendo DS, DSi et 3DS.

## Fonctionnalités

- Parcourir et rechercher une base de données de jeux NDS
- Télécharger les `.nds` / `.cia` directement sur la carte SD
- Téléchargement multi-plateformes (DSi + 3DS)
- Extraction de fichiers ZIP
- Chargement de bases de données via URL ou fichier
- Support de plusieurs bases de données
- Vérification automatique des mises à jour
- Schéma de couleurs personnalisable
- Support multi-langues (EN, FR, NL, IT)

## Téléchargement

### DSi

- Téléchargez `NDS-Shop.nds` depuis la [dernière release](https://github.com/NDS-Shop-Homebrew/NDS-Shop/releases/latest).
- Placez-le n'importe où sur votre carte SD.
- Lancez via TWiLight Menu++ ou Unlaunch.

### 3DS

#### Prérequis
- Une Nintendo 3DS (XL) / 2DS (XL) modifiée
- Une carte SD d'au moins 500 Mo pour l'installation des jeux

#### Dernière release
- [dernière release](https://github.com/NDS-Shop-Homebrew/NDS-Shop/releases/latest) — récupérez le fichier `.cia` ou `.3dsx`

## Compilation

### DSi

Nécessite [Wonderful Toolchain](https://wonderful.asie.pl/) + BlocksDS.

#### Installer les dépendances

Depuis le dépôt pacman BlocksDS :
```
wf-pacman -S blocksds-core toolchain-gcc-arm-none-eabi-zlib toolchain-gcc-arm-none-eabi-libpng16 blocksds-mbedtls blocksds-libcurl
```

#### Compilation

```bash
git clone --recursive https://github.com/NDS-Shop-Homebrew/NDS-Shop.git
cd NDS-Shop
python3 build.py
```

Utilisez `python3 build.py release` pour output dans `DSi/release/`.
Ou utilisez `compile-dsi.bat` / `clean-dsi.bat` sous Windows.

### 3DS

Nécessite devkitPro + libctru + 3ds-curl + 3ds-libarchive.

```bash
# Installer les dépendances
(dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive

# Cloner et compiler
git clone --recursive https://github.com/NDS-Shop-Homebrew/NDS-Shop.git
cd NDS-Shop
make
```

Ou utilisez `compile-3ds.bat` / `clean-3ds.bat` sous Windows.

## Crédits

Basé sur [Kekatsu DS](https://github.com/cavv-dev/Kekatsu-DS) par cavv.
Base de données : [UDB-Kekatsu-DS](https://github.com/cavv-dev/UDB-Kekatsu-DS).
Concept inspiré par [pkgi-psp](https://github.com/bucanero/pkgi-psp) et [Universal-Updater](https://github.com/Universal-Team/Universal-Updater).
