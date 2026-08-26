# NDS-Shop

Shop alternative pour Nintendo DS / DSi — parcourez, recherchez et téléchargez des jeux NDS directement depuis votre console.

## Plateformes supportées

- **Nintendo DSi** (clone du DSi Shop)
- **Nintendo 3DS** (clone du eShop)

## Fonctionnalités

- Parcourir et rechercher une base de données de jeux NDS
- Télécharger les `.nds` / `.cia` directement sur la carte SD
- Support multi-langues (EN, FR, NL, IT)
- Mise à jour automatique depuis les releases GitHub

## Compilation

### DSi

Nécessite [Wonderful Toolchain](https://wonderful.asie.pl/) + BlocksDS.

```bash
# Depuis le Wonderful Toolchain Shell
cd DSi
python build.py
```

Ou utiliser `compile-dsi.bat` / `clean-dsi.bat` sous Windows.

### 3DS

Nécessite devkitPro + libctru.

```bash
cd 3DS
make
```

Ou utiliser `compile-3ds.bat` / `clean-3ds.bat` sous Windows.

## Crédits

Basé sur [Kekatsu DS](https://github.com/cavv-dev/Kekatsu-DS) par cavv.
