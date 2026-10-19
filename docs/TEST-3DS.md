# Test sur console (Nintendo 3DS)

Fiche de recette de l'application sur une console réelle. Les tests hors console
(`make -C tests/host`) couvrent l'analyse du catalogue et la configuration ;
cette fiche couvre ce qui ne se vérifie que sur le matériel : accès réseau via
le service SOC, installation d'un titre via le service AM, écriture sur la carte
SD, rendu à l'écran, comportement des raccourcis de jeux.

## Prérequis

- Nintendo 3DS ou 2DS avec un custom firmware récent (Luma3DS) autorisant
  l'installation de titres non signés.
- Un gestionnaire de titres (FBI ou équivalent) pour installer le `.cia`.
- Une carte SD avec environ 100 Mo libres par jeu testé.
- Un serveur joignable depuis la console exposant l'API `/api/v1` du catalogue
  (instance locale ou en ligne).

## Récupérer le binaire

| Méthode | Comment |
| --- | --- |
| Artefact d'intégration continue | Onglet **Actions** → workflow « Build NDS-Shop (3DS) » → dernier passage → artefact `nds-shop-3ds` (contient `NDS-Shop.cia` et `NDS-Shop.3dsx`). Le workflow peut être relancé à la main avec *Run workflow*. |
| Compilation locale | `make` produit `NDS-Shop.3dsx` ; `make cia` produit `NDS-Shop.cia` (nécessite `bannertool` et `makerom`). |

Les artefacts de l'intégration continue expirent au bout de 90 jours ; à défaut,
recompiler.

## Adresse de l'API

Au premier lancement, l'application pointe vers `https://api.db-nds-shop.fr`.
Pour viser une autre instance, ouvrir **Réglages** (bouton **Y**) puis saisir
l'adresse.

À saisir : **l'origine seule**, sans barre oblique finale et sans `/api`
(l'application ajoute elle-même le chemin `/api/v1/...`).

| Saisie | Adresse réellement appelée |
| --- | --- |
| `https://exemple.fr` | `https://exemple.fr/api/v1/catalog` |
| `https://exemple.fr/` | idem (barre oblique finale ignorée) |
| `https://exemple.fr/api` | idem (suffixe `/api` redondant ignoré) |
| `http://hote-local:3000` | `http://hote-local:3000/api/v1/catalog` |

## Préparation de la carte SD

Rien à créer à la main : l'application crée ses dossiers. Pour repartir d'un
état propre, supprimer :

- `sdmc:/3ds/NDS-Shop/` : configuration, cache du catalogue, atlas d'icônes ;
- `sdmc:/roms/nds/` : ROM Nintendo DS téléchargées.

## Scénarios

| # | Manipulation | Résultat attendu |
| --- | --- | --- |
| 1 | Premier lancement, Wi-Fi coupé | Écran **Erreur** avec un message explicite ; `A` réessaie. Aucun blocage. |
| 2 | Wi-Fi rétabli, `A` | Écran de chargement, puis liste des jeux : vignettes visibles, pied de page avec le titre de la boutique et le nombre de jeux. |
| 3 | Inspecter la carte SD | `sdmc:/3ds/NDS-Shop/` contient `config.txt` et `catalog.json` ; après le chargement des icônes, `atlas.t3x` et `atlas.url`. |
| 4 | `X`, saisir un titre puis un auteur | La liste se filtre, le compteur suit, `B` efface la recherche. |
| 5 | `Y` : changer la langue, le thème, puis redémarrer | Les réglages sont conservés (relire `config.txt`). |
| 6 | `Y` : saisir une adresse erronée (`https://exemple.invalid`) | Écran **Erreur** lisible, sans plantage ; une adresse correcte rétablit le catalogue. |
| 7 | `A` sur un jeu, laisser l'installation finir | Progression affichée (pourcentage), puis message de fin. |
| 8 | Inspecter la carte SD après installation | La ROM est dans `sdmc:/roms/nds/` ; **aucun `.cia` résiduel** sur la carte. |
| 9 | Revenir au menu d'accueil de la console | Le raccourci du jeu est présent et se lance (jeu Nintendo DS en mode rétrocompatible). |
| 10 | Relancer une installation puis `B` en cours de téléchargement | La tâche s'arrête, le fichier partiel est supprimé, retour à la liste. |
| 11 | Wi-Fi coupé, puis relancer l'application | Mention **hors-ligne** ; la liste provient de `catalog.json`. |
| 12 | `START` | Retour propre au menu d'accueil de la console. |
| 13 | (Optionnel) Réinstaller un jeu déjà présent | L'installation se déroule sans erreur, sans fichier temporaire résiduel. |

## En cas d'échec

Relever, dans cet ordre :

1. l'écran affiché et le **message d'erreur exact** (photo) ;
2. le contenu de `sdmc:/3ds/NDS-Shop/config.txt` ;
3. la liste et la taille des fichiers de `sdmc:/3ds/NDS-Shop/` et
   `sdmc:/roms/nds/` ;
4. le numéro du scénario et le binaire utilisé (artefact ou compilation locale).

## Points connus

- **TLS** : la vérification du certificat serveur est désactivée dans
  l'application (aucune autorité de certification n'est embarquée). Le trafic
  reste chiffré mais n'est pas authentifié.
- **Cache hors-ligne** : seule la première page du catalogue est conservée
  (jusqu'à 200 jeux).
- **Sans custom firmware** : seul le `.3dsx` est utilisable ; l'installation
  d'un raccourci échouera.
- **Atlas absent** : si l'atlas d'icônes ne peut être téléchargé, des pastilles
  génériques remplacent les vignettes ; le reste de l'application fonctionne.
