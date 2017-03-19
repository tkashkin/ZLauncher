# ZLauncher
The Legend of Zelda: Breath of the Wild custom launcher

## Features
* Launch game (both with user and admin priveleges)
* Launch speedhack (CheatEngine standalone trainer)
* Supports launching from Steam library as Non-Steam game
* Launch game through Steam with admin priveleges

## Configuration
All configurations are done by `zlauncher.ini` config file.
ZLauncher can use another config file if specified with `-config` command line parameter.

This is an example config file:
```ini
[cemu]
path=D:\Programs\Cemu                                          ; Path to Cemu
fullscreen=true                                                ; Fullscreen
cmdline=                                                       ; Additional Cemu command line parameters

[game]
path=D:\Games\WiiU\The Legend of Zelda - Breath of the Wild    ; Path to the game
elevate=true                                                   ; Run game with admin priveleges

[speedhack]
path=.\speedhack.exe                                           ; Path to speedhack trainer
enabled=false                                                  ; Is speedhack enabled

[steam]
enabled=true                                                   ; Is Steam support enabled
elevate=true                                                   ; Run Steam with admin priveleges
gameid=12927812811798609920                                    ; Game id in Steam library
```

You can launch game through Steam to get Steam overlay and Steam controller working:
* Add ZLauncher into your Steam library as a Non-Steam game
* Create desktop shortcut for this game
* Find new shortcut on desktop and open its properties
* Copy id number from URL attribute. It will be something like **steam://rungameid/`12927812811798609920`**
* Paste id to the `gameid` parameter of the `[steam]` section
* Delete shortcut if you want
* Set the `[steam].enabled` parameter to `true`
* Set the `[steam].elevate` and `[game].elevate` parameters to `true` if you want to run game as admin
