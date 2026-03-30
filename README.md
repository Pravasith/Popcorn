# Popcorn Engine

A small artist friendly stylized 3d graphics engine. Uses Vulkan on the backend
to draw sweet sweet graphics on the screen.

## Development setup

#### Friendly Advice

If you want to use Popcorn for your Game, you don't need to go through this,
just download it for Linux or Windows from the website (there's no website but
there will be in the future...)

But if you wanna build it from source for whatever reason, maybe to contribute,
follow these steps

- Clone the repo obviously. If you don't know how to do it then you should
  probably take my friendly advice above.

- These steps are for Linux machines only. You can cross-compile for Windows
  though.

- Install these dependencies (they're needed for Vulkan and GLFW build step)

```
sudo apt-get install git build-essential libx11-xcb-dev \
libxkbcommon-dev libwayland-dev libxrandr-dev xorg-dev
```

- Install the cross compiler (needed for cross-compiling your game to windows)

```
sudo apt install mingw-w64
```

- Then run the bash script `/tools/linux-setup.sh`. Again, if you need help with
  this, you should take my friendly advice above.
