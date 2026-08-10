# NDS-Shop
> An alternative Nintendo DS shop, designed for the Nintendo 3DS family of systems.

## Download

### Requirements
- A modded Nintendo 3DS (XL) / 2DS (XL)
- An SD card with at least 500 MB for game installation
- The latest version (not yet available)

---

<details><summary><strong>Compilation</strong></summary>

#### Setting up the development environment:

<details><summary><strong>Setting up your environment</strong></summary>

##### For Windows:
- Download and install [Git](https://git-scm.com/downloads) if you haven't already.
- Install [devkitPro installer](https://github.com/devkitPro/installer/releases) if you haven't already.

To build NDS-Shop from source, you will need to set up a system with devkitARM, libctru, and 3ds-libarchive. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run the following command:  
`(sudo dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive`

##### For Linux:
- To build NDS-Shop from source, you will need to set up a system with devkitARM, libctru, and 3ds-libarchive. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run the following command:  
`(sudo dkp-)pacman -S devkitARM libctru 3ds-curl 3ds-libarchive`
- Download and install [Git](https://git-scm.com/downloads) if you haven't already.

<details><summary><strong>Cloning the repository</strong></summary>
 
##### Cloning the repo:

Run the following command to clone the repository:  
`git clone --recursive https://gitlab.com/nds-shop-team/nds-shop`

Then, run the `compile.bat` script. To perform a clean build, use the `clean.bat` command.
</details></details>
