# NLTK - New Leaf ToolKit [![Build status](https://ci.appveyor.com/api/projects/status/dvqruc40q9fyes5r/branch/master?svg=true)](https://ci.appveyor.com/project/Slattz/nltk/branch/master)

NLTK is a WIP ACNL Toolkit without the hassle of taking out your SD card. Only Homebrew **or** CFW is needed to use it. \
The main developers of this project are [Slattz](https://github.com/Slattz) and [Cuyler](https://github.com/Cuyler36).

[![Discord](https://discordapp.com/api/guilds/437592048057450508/widget.png?style=banner2)](https://discord.gg/j2VUBBv)

## Features

### Save Editor

* Auto Updater - Easily get the Latest Update
* Custom Keyboard with ACNL's Custom Symbols
* Basic Player Editing
  * Player Name
  * Player Wallet, Bank, Medals & MEOW Coupons
* Acre Editing
* Basic Villager Editing
  * Change the Villager in each slot

## Installation

### For Original 'Hax' Homebrew Users:

Old 'Hax' Homebrew Entrypoints (such as steelhax) are not supported. \
Please [install CFW](https://3ds.hacks.guide) (**It's free on the latest firmware**) and use NLTK with Rosalina Homebrew or install NLTK directly to your Home Menu.

### For CFW Users:

#### NLTK on Home Menu

1. Install `NLTK.cia` from the [latest release](https://github.com/Slattz/NLTK/releases/latest) using [FBI](https://github.com/Steveice10/FBI/releases/latest).
2. You're done, it's that easy. It'll now be on your Home Menu. If you choose to enable the Auto Updater, the app will automatically update on start-up, hassle-free.

#### NLTK for Rosalina Homebrew

1. Copy `NLTK.3dsx` to `SD:/3ds/NLTK/`.
2. Launch the HBL, then open NLTK.
3. You're done, it's that easy. If you choose to enable the Auto Updater, the app will automatically update on start-up, hassle-free.

## Compiling

### Requirements

1. Firstly, make sure devkitPro is properly installed, up to date and added to your PATH.
2. Using [devkitPro's pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702), install the `3ds-dev` package if you haven't already. If you have already, ensure everything is up to date.
3. Using [devkitPro's pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702), install the following dkP packages: `3ds-curl`, `3ds-mbedtls`, `3ds-libjpeg-turbo`, `3ds-zlib`, `3ds-freetype`, `3ds-jansson`.
4. [makerom](http://3dbrew.org/wiki/Makerom) and [bannertool](https://github.com/Steveice10/bannertool) somewhere in your `PATH` environment.

Enter the NLTK directory and run `make` in your command line/terminal.

Provided you have all the requirements, `NLTK.cia` and `NLTK.3dsx` should be outputted to `/out`.
