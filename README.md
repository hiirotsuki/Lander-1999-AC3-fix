# Lander (1999) DVD Edition crash fix

this fix takes care of two issues:

1. the game attempts to read from the registry using write-only permissions, this causes the game to fail to start at all on Windows 11

2. background music tracks are picked at random on mission start, once the first track has finished playing, it stops playing,
and fails to cleanup the audio handler, leading to a crash on mission abort/success

## Usage
place dinput.dll into the same directory as lander.exe, both lndrd3dr (directx) and lndrgldr (glide) are supported.
