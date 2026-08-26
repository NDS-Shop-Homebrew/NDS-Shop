# NDS-Shop v1.2.0

## Nouvelles fonctionnalités

- **Mise à jour tout** — met à jour tous les jeux installés en une action.
- **Sauvegarde et restauration des réglages** — exporte/importe ta configuration et tes favoris.
- **Détection automatique des jeux installés** — les jeux déjà présents sur la carte SD sont détectés et marqués.
- **Tri par taille et par date de mise à jour** + filtre des jeux installés.
- **QR Code dynamique** — lien direct vers le téléchargement.
- **Versioning automatique** — version extraite des tags git à la compilation (3DS + DSi).
- **Screenshots** — captures d'écran DSi/3DS ajoutées dans le README.
- **Support FR** — accents corrigés (Latin-1), langue française fonctionnelle.

## Corrections

- **Crash au démarrage sur console réelle corrigé** : la vérification de mise à jour au lancement est désactivée pour l'instant (le parsing de la réponse GitHub dépassait la pile de la console).
- **Chargement des thèmes** : validation des couleurs réécrite sans regex (stabilité accrue au boot).
- **Installation CIA fiabilisée** : correction d'un bug qui écrivait trop de données en fin de fichier (CIA corrompus / installs qui échouaient).
- **Espace SD insuffisant** : l'installation affiche maintenant une erreur au lieu d'un faux succès.
- **File de téléchargement** : une installation échouée est signalée comme telle (au lieu d'être marquée « terminée »).
- **Scanner QR** : correction d'un plantage possible à la fermeture (threads non joints).
- **Extraction d'archives** : blocage des chemins malveillants (`../`) pouvant écrire n'importe où sur la SD.
- **Screenshots** : plantage évité lorsqu'une image est corrompue.
- **Stabilité JSON** : plantage évité si la configuration ou les favoris sont édités avec de mauvais types.
- **DSi : WiFi non-fatal** — l'init WiFi ne crashe plus l'app sans réseau.
- **DSi : buffer download** — réduit 1MiB → 32KiB pour la RAM DS (4MB).
- **DSi : FAT fallbacks** — chdir robuste pour melonDS.

## DSi spécifique

- 7 thèmes de couleurs (défaut NDS-Shop + Dark/Blue/Amber/Red/Green/Purple)
- Langue française (fr.lang) encodage Latin-1 corrigé
- Auto-création `databases.txt` avec URL par défaut
- Scripts de build Windows (`compile-dsi.bat`, `clean-dsi.bat`)

## Crédits

- Crédits mis à jour : **Team NDS-Shop-Homebrew** (Rinzler, LoannMKW) — basé sur Universal-Updater (Universal-Team).
- DSi : basé sur Kekatsu DS (cavv-dev) — MIT licensed.

## CI / Release

- Build automatisé 3DS + DSi sur chaque tag `v*`.
- Publication automatique de la release (`.cia` + `.3dsx` + `.nds` + `version.txt`).