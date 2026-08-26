# NDS-Shop (3DS) v1.2.0

## Nouvelles fonctionnalités

- **Mise à jour tout** — met à jour tous les jeux installés en une action.
- **Sauvegarde et restauration des réglages** — exporte/importe ta configuration et tes favoris.
- **Détection automatique des jeux installés** — les jeux déjà présents sur la carte SD sont détectés et marqués.
- **Tri par taille et par date de mise à jour** + filtre des jeux installés.
- **QR Code dynamique** — lien direct vers le téléchargement.
- **Versioning automatique** — version extraite des tags git à la compilation.
- **Screenshots** — captures d'écran 3DS ajoutées dans le README.
- **Support FR** — accents corrigés, langue française fonctionnelle.

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

## CI / Release

- Build automatisé 3DS sur chaque tag `v*`.
- Publication automatique de la release (`.cia` + `.3dsx`).

## Crédits

Basé sur [Universal-Updater](https://github.com/Universal-Team/Universal-Updater) par Universal-Team.
Base de données : [UDB-Kekatsu-DS](https://github.com/cavv-dev/UDB-Kekatsu-DS).