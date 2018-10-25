# osu!auto+

[**Этот файл на русском.**](https://github.com/YaLTeR/osu-auto-plus/blob/master/README.ru.md)

Pre-built ready to use osu!auto+ is linked in the description of [this YouTube video](https://youtu.be/mO3EGy17YoQ).

This project contains osu!auto+ patches to the 2016 leaked osu! source code. To apply them, make sure you're in a git repository with the clean osu! code and run `git am --keep-cr path/to/this/repo/*.patch`, where `path/to/this/repo/` is the path to the folder with `.patch` files from this repository.

After building the client (I used either the `Debug` or the `PublicNoUpdate` configuration), make sure to put `Spline.dll` into the resulting osu! folder.

## What's osu!auto+?

osu!auto+ is a rewrite of the built-in Auto mod which attempts to obtain the highest possible scores on 2B maps, which the standard Auto does really poorly on. Additionally, osu!auto+ includes somewhat customizable cursor dancing and cursor trail tweaks. You can watch it in action in [this YouTube playlist](https://www.youtube.com/playlist?list=PLZnZoQzA8hx4xrr6yJ1nA5gpqVwqBLHun).

## Spline

The `Spline` folder contains the source code for `Spline.dll`. It's separated out because when I was adding spline interpolation I had this homework assignment with some necessary linear algebra laying around, so I just re-used the code. Thus the code quality is probably sub-ideal. Sorry for inconvenience.
