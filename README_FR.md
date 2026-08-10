# NDS-Shop
> Un magasin alternatif pour Nintendo DS, conçu pour la famille des consoles Nintendo 3DS.

## Téléchargement

### Prérequis
- Une Nintendo 3DS (XL) / 2DS (XL) modifiée
- Une carte SD d'au moins 500 Mo pour l'installation des jeux
- La dernière version (non disponible)

---

<details><summary><strong>Compilation</strong></summary>

#### Préparer l'environnement de développement :

<details><summary><strong>Configurer votre environnement</strong></summary>

##### Pour Windows :
- Téléchargez et installez [Git](https://git-scm.com/downloads) si ce n'est pas déjà fait.
- Installez le [devkitPro installer](https://github.com/devkitPro/installer/releases) si ce n'est pas déjà fait.

Pour compiler NDS-Shop depuis le code source, vous devrez configurer un environnement avec devkitARM, libctru, et 3ds-libarchive. Suivez la [Getting Started](https://devkitpro.org/wiki/Getting_Started) de devkitPro pour installer pacman, puis exécutez la commande suivante :  
`(sudo dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive`

##### Pour Linux :
- Pour compiler NDS-Shop depuis le code source, vous devrez configurer un environnement avec devkitARM, libctru, et 3ds-libarchive. Suivez la page [Getting Started](https://devkitpro.org/wiki/Getting_Started) de devkitPro pour installer pacman, puis exécutez la commande suivante :  
`(sudo dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive`
- Téléchargez et installez [Git](https://git-scm.com/downloads) si ce n'est pas déjà fait.

<details><summary><strong>Cloner le dépôt</strong></summary>
 
##### Clonage du dépôt :

Exécutez la commande suivante pour cloner le dépôt :  
`git clone --recursive https://gitlab.com/nds-shop-team/nds-shop`

Ensuite, exécutez le script `compile.bat`. Pour effectuer un nettoyage d'une compilation, utilisez la commande `clean.bat`.
</details></details>
