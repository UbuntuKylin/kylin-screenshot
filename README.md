<div align="center">
  <p>
    <h1>
      Kylin-Screenshot
    </h1>
    <h4>Powerful yet simple to use screenshot software.</h4>
  </p>
</div>


## Index

- [Features](#features)
- [Usage](#usage)
  - [CLI configuration](#cli-configuration)
- [Keyboard Shortcuts](#keyboard-shortcuts)
  - [Local](#local)
  - [Global](#global)
    - [On KDE Plasma desktop](#on-kde-plasma-desktop)
- [Considerations](#considerations)
- [Installation](#installation)
- [Compilation](#compilation)
  - [Dependencies](#dependencies)
    - [Debian](#debian)
    - [Fedora](#fedora)
  - [Build](#build)
  - [Install](#install)
- [Packaging](#packaging)
- [License](#license)
- [Contribute](#contribute)
- [Acknowledgment](#acknowledgment)

## Features

- Customizable appearance.
- Easy to use.
- In-app screenshot edition.
- DBus interface.
- Upload to Imgur.

## Usage

Example commands:

- Capture with GUI:

    ```shell
    kylin-screenshot gui
    ```

- Capture with GUI with custom save path:

    ```shell
    kylin-screenshot gui -p ~/myStuff/captures
    ```

- Capture with GUI after 2 seconds delay (can be useful to take screenshots of mouse hover tooltips, etc.):

    ```shell
    kylin-screenshot gui -d 2000
    ```

- Fullscreen capture with custom save path (no GUI) and delayed:

    ```shell
    kylin-screenshot full -p ~/myStuff/captures -d 5000
    ```

- Fullscreen capture with custom save path copying to clipboard:

    ```shell
    kylin-screenshot full -c -p ~/myStuff/captures
    ```

- Capture the screen containing the mouse and print the image (bytes) in PNG format:

    ```shell
    kylin-screenshot screen -r
    ```

- Capture the screen number 1 and copy it to the clipboard:

    ```shell
    kylin-screenshot screen -n 1 -c
    ```

In case of doubt choose the first or the second command as shortcut in your favorite desktop environment.

A systray icon will be in your system's panel while Kylin-ScreenShot is running.
Do a right click on the tray icon and you'll see some menu items to open the configuration window and the information window.
Check out the information window to see all the available shortcuts in the graphical capture mode.

### CLI configuration

You can use the graphical menu to configure Kylin-ScreenShot, but alternatively you can use your terminal or scripts to do so.

- Open the configuration menu:

    ```shell
    kylin-screenshot config
    ```

- Show the initial help message in the capture mode:

    ```shell
    kylin-screenshot config --showhelp true
    ```

- For more information about the available options use the help flag:

    ```shell
    kylin-screenshot config -h
    ```

## Keyboard shortcuts

### Local

These shortcuts are available in GUI mode:

|  Keys                                                                     |  Description                                                   |
|---                                                                        |---                                                             |
| <kbd>←</kbd>, <kbd>↓</kbd>, <kbd>↑</kbd>, <kbd>→</kbd>                    | Move selection 1px                                             |
| <kbd>Shift</kbd> + <kbd>←</kbd>, <kbd>↓</kbd>, <kbd>↑</kbd>, <kbd>→</kbd> | Resize selection 1px                                           |
| <kbd>Esc</kbd>                                                            | Quit capture                                                   |
| <kbd>Ctrl</kbd> + <kbd>C</kbd>                                            | Copy to clipboard                                              |
| <kbd>Ctrl</kbd> + <kbd>S</kbd>                                            | Save selection as a file                                       |
| <kbd>Ctrl</kbd> + <kbd>Z</kbd>                                            | Undo the last modification                                     |
| <kbd>Spacebar</kbd>                                                       | Toggle visibility of sidebar with options of the selected tool, color picker for the drawing color and history menu |
| Mouse Wheel                                                               | Change the tool's thickness                                    |

<kbd>Shift</kbd> + drag a handler of the selection area: mirror redimension in the opposite handler.

### Global

If you want use Kylin-Screenshot as a default screenshot utility, chances are you want to launch it using the <kbd>Prt Sc</kbd> key.Kylin-ScreenShot  doesn't yet offer a fully-automated option to do so, but you can configure your system to do so.

#### On KDE Plasma desktop

To make configuration easier, there's a [file](docs/shortcuts-config/kylin-screenshot-shortcuts-kde) in the repository that more or less automates this process. This file will assign the following keys to the following actions by default:

|  Keys                                                  |  Description                                                                       |
|---                                                     |---                                                                                 |
| <kbd>Prt Sc</kbd>                                      | Start the Kylin-screenShot screenshot tool and take a screenshot                          |
| <kbd>Ctrl</kbd> + <kbd>Prt Sc</kbd>                    | Wait for 3 seconds, then start the Kylin-Screenshot screenshot tool and take a screenshot |
| <kbd>Shift</kbd> + <kbd>Prt Sc</kbd>                   | Take a full-screen (all monitors) screenshot and save it                           |
| <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>Prt Sc</kbd> | Take a full-screen (all monitors) screenshot and copy it to the clipboard          |

If you don't like the defaults, you can change them manually later.

Steps for using the configuration:

1. The configuration file configures shortcuts so that Kylin-Screenshot automatically saves (without opening the save dialog) screenshots to `~/Pictures/Screenshots` folder. Make sure you have that folder by running the following command:

    ```shell
    mkdir -p ~/Pictures/Screenshots
    ```

    (If you don't like the default location, you can skip this step and configure your preferred directory later.)
2. Download the configuration file:

    ```shell
    cd ~/Desktop
    ```
3. Go to _System Settings_ → _Shortcuts_ → _Custom Shortcuts_.
4. If there's one, you'll need to disable an entry for Spectacle, the default KDE screenshot utility first because its shortcuts might collide with Kylin-Screenshot's ones; so, just uncheck the _Spectacle_ entry.
5. Click _Edit_ → _Import..._, navigate to the Desktop folder (or wherever you saved the configuration file) and open the configuration file.
6. Now the Kylin-ScreenShot entry should appear in the list. Click _Apply_ to apply the changes.
7. If you want to change the defaults, you can expand the entry, select the appropriate action and modify it as you wish; the process is pretty mush self-explanatory.

## Considerations

- Experimental Gnome Wayland and Plasma Wayland support.

- If you are using Gnome you need to install the [Gnome Shell Extension Appindicator](https://github.com/Ubuntu/gnome-shell-extension-appindicator) extension in order to see the systemtray icon.

- In order to speed up the first launch of Kylin-ScreenShot (DBus init of the app can be slow), consider starting the application automatically on boot.

- Press <kbd>Enter</kbd> or <kbd>Ctrl</kbd> + <kbd>C</kbd> when you are in a capture mode and you don't have an active selection and the whole desktop will be copied to your clipboard! Pressing <kbd>Ctrl</kbd> + <kbd>S</kbd> will save your capture in a file! Check the [Shortcuts](#shortcuts) for more information.

- Execute the command `kylin-screenshot` without parameters to launch a running instance of the program without taking actions.

## Installation

There are packages available for a few distros:

- [Arch]: `pacman -S kylin-screenshot`
- [Debian 10+]: `apt install kylin-screenshot`
  + Package for Debian 9 ("Stretch") also [available via stretch-backports](https://backports.debian.org/).
- [Ubuntu 18.04+]: `apt install kylin-screenshot`
- [Void Linux] (`xbps-install kylin-screenshot`)
- Fedora: `dnf install kylin-screenshot`

## Compilation

To build the application in your system, you'll need to install the dependencies needed for it and Package names might be different for each distribution, see [Dependencies](#dependencies) below for more information.

### Dependencies

#### Build dependencies

- Qt >= 5.3
  + Development tools
- GCC >= 4.9.2
- cmake
- pkg-config
- Qt4 (4.8 or newer) or Qt5 (5.7 or newer)
- ffmpeg or libav (libavformat, libavcodec, libavutil, libswscale)
- ALSA library
- PulseAudio library (optional, disable with -DWITH_PULSEAUDIO=FALSE)
- JACK library (optional, disable with -DWITH_JACK=FALSE)
- libGL (32 and 64 bit)
- libGLU (32 and 64 bit)
- libX11 (32 and 64 bit)
- libXfixes (32 and 64 bit)
- libXext
- libXi
- libxinerama

#### Debian

```shell
apt install g++ build-essential qt5-default qt5-qmake qttools5-dev-tools cmake pkg-config desktop-file-utils \
    libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libasound2-dev libpulse-dev libjack-jackd2-dev \
    libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxfixes-dev libxext-dev libxi-dev libxinerama-dev
Extra dependencies for 32-bit GLInject on 64-bit systems:

    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install g++-multilib libgl1-mesa-dev:i386 libglu1-mesa-dev:i386 libx11-dev:i386 libxfixes-dev:i386

Note: Debian <= 7 and Ubuntu <= 14.04 are affected by Debian bug #689088 (https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=689088), which makes it impossible to install the 32-bit and 64-bit libGL/libGLU development packages at the same time. You should use the following command instead:

    sudo apt-get install g++-multilib libglu1-mesa:i386 libx11-dev:i386 libxfixes-dev:i386

Next you should apply the workarounds described in the 'compiling' section.
```

#### Fedora

```shell
dnf install gcc-c++ qt5-devel qt5-qtbase-devel qt5-linguist  ffmpeg-devel alsa-lib-devel pulseaudio-libs-devel \
    jack-audio-connection-kit-devel 
make gcc gcc-c++ mesa-libGL-devel mesa-libGLU-devel libX11-devel libXext-devel libXfixes-devel
```

### Build

After installing all the dependencies, finally run the following commands in the sources root directory:

```shell
mkdir build
cd build
qmake ../
make
```

### Install

Simply use `make install` with privileges.

## Packaging

Having `git` installed is required if you're building Kylin-ScreenShot from a snapshot to have precise version information.

In order to generate the instructions in the `Makefile` to install the application in `/usr` instead of in `/usr/local` you can pass the `packaging` option to `qmake` (`qmake CONFIG+=packaging`).

If you want to install in a custom directory you can use the `INSTALL_ROOT` variable.

**Example**:

If you want to install Kylin-ScreenShot in `~/myBuilds/test`, you can execute the following to do so:

```shell
qmake CONFIG+=packaging
make INSTALL_ROOT=~/myBuilds/test install
```

## License
- The main code is licensed under [GPLv3](LICENSE)
- The button icons are licensed under Apache License 2.0. See: https://github.com/google/material-design-icons
- The code at capture/capturewidget.cpp is based on https://github.com/ckaiser/Lightscreen/blob/master/dialogs/areadialog.cpp (GPLv2)
- The code at capture/capturewidget.h is based on https://github.com/ckaiser/Lightscreen/blob/master/dialogs/areadialog.h (GPLv2)
- I copied a few lines of code from KSnapshot regiongrabber.cpp revision `796531` (LGPL)
- Qt-Color-Widgets taken and modified from https://github.com/mbasaglia/Qt-Color-Widgets (see their license and exceptions in the project) (LGPL/GPL)

Info: If I take code from your project and that implies a relicense to GPLv3, you can reuse my changes with the original previous license of your project applied.

## Contribute

If you want to contribute check the [CONTRIBUTING.md](CONTRIBUTING.md)

## Acknowledgment
I really appreciate those who have shown interest in the early development process:
- [Cosmo](https://github.com/philpem)
- [XerTheSquirrel](https://github.com/XerTheSquirrel)
- [The members of Sugus GNU/Linux](https://github.com/SUGUS-GNULinux)
- ismatori
