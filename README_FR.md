# NDS-Shop

> Un magasin alternatif pour Nintendo DS, conçu pour les consoles Nintendo DSi et 3DS.

![Screenshot 1](https://github.com/cavv-dev/Kekatsu-DS/raw/main/resources/screenshots/Kekatsu-DS_1.png) ![Screenshot 2](https://github.com/cavv-dev/Kekatsu-DS/raw/main/resources/screenshots/Kekatsu-DS_2.png) ![Screenshot 3](https://github.com/cavv-dev/Kekatsu-DS/raw/main/resources/screenshots/Kekatsu-DS_3.png)

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
- [v1.0.0](https://github.com/NDS-Shop-Homebrew/NDS-Shop/releases/latest) — récupérez l'asset `.cia` ou `.3dsx`

## Compilation

### DSi

Nécessite [Wonderful Toolchain](https://wonderful.asie.pl/) + BlocksDS.

```bash
# Depuis le Wonderful Toolchain Shell
cd DSi
python build.py
```

Utilisez `python build.py release` pour output dans `DSi/release/`.
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
