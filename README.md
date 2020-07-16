WaveFunctionCollapse implementation in C 
========================================

*The project was based on the [Maxim Gumin's wave function algorithm](https://github.com/mxgmn/WaveFunctionCollapse).*

The program determines a bitmap based on the input consisting of smaller tiles (.png) as well as a set of constraints (.xml). The purpose is to create an image that is [*locally similair*](#Local-Similarity) to the input images.

<p align="center">
  <img src="wfc_demo.gif" width="600">
</p>

## Table of contents

- [Introduction](#introduction)
- [Local similarity](#local-similarity)
- [Algorithm](#algorithm)
- [Example inputs and outputs](#example-inputs-and-outpus)
- [Possible extensions](#possible-extensions)
- [References](#references)
- [Credits and licences](#credits-and-licences)
- [Documentation](#documentation)
----------------------------------------

## Introduction

The name of the algorithm stems from the Quantum Mechanics, where it refers to a [behaviour](https://en.wikipedia.org/wiki/Wave_function_collapse) of miniscule particles which gradaully determies their position in space. Similair in a broad sense, WaveFunctionCollapse (WFC) determies the orientation and coordinates of input tiles based on initial constraints. Let us consider a simplified example:

1. The bitmap consist of two spaces for tiles, which can be occupied by any of the four tiles with equal propabilities. The *entropy* of each space is 4 now. In our model, the algothim will always choose the minium *non-zero* entropy space and then choose randomly.

2. Let us say that the right space was chosen with a head occupying it. Now only straigt and tail tiles stisfy the neighbourhood rules. The choice of the head tile *propagates* itself throughout the grid.

3. Tail is chosen at random. Valid image is determined, collapse terminates.

<p align="center">
  <img src="CollapseOfWF.gif" width="600">
</p>



## Local similarity

Authors: [Łukasz Jakubowski](https://github.com/woocashh), [Maciej Kaszlewicz](https://github.com/NaraS91), [Paweł Kroll](https://github.com/skrroll), [Stefan Radziuk](https://github.com/stefanradziuk)

Tile art in graphics/rpg_map is Overworld rpg tileset by Tayoko (licensed under [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/)) available at [opengameart.org/content/overworld-rpg-tileset](https://opengameart.org/content/overworld-rpg-tileset)
