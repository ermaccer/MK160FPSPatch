# MK1.60FPSPatch

A 60 FPS patch for Mortal Kombat 1.

Removes 30 fps cap from:
- Main Menu
- Invasions
- Towers
- Kustomization
- Cinematics (intros/fatal blows/fatalities) **Read below**

<p align="center">Click the image to go to the YT video.</p>

[![YouTubeVideo](https://i.imgur.com/n89ajEH.jpg)](https://www.youtube.com/watch?v=3wBk2euEs0Q)


# Issues

Since the game uses 30/60hz materials for certain things, some elements might be
broken during previously capped sections such as character faces or eyes.

While all fatal blows, fatalities and intros will run at 60 fps, their animations
seem to be recorded at 30 fps without interpolation. This was also the case
with some MK11 animations (Scorpion's fire turbine intro is 30 fps for example).

60 FPS might be implemented properly by QLOC/NRS in the near future, enjoy for now.



# Download

##  60 FPS patch was tested only with the latest Steam version!

You can download binary files from [Releases](https://github.com/ermaccer/MK160FPSPatch/releases) page. Extract **mk160fps.zip**
to MK12\Binaries\Win64 folder of Mortal Kombat 1 (so that **dsound.dll** ends up near **SDL2.dll**).

If you are not sure how to find your Mortal Kombat 1 folder, search for it in your Steam library then right click on the entry and select Manage->Browse local files.

