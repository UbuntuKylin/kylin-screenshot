<div align="center">
  <p>
    <h1>
      Kylin-Screenshot
    </h1>
    <h4>Powerful yet simple to use screenshot software forked from flameshot.</h4>
  </p>
</div>
  kylin-screenshot is a powerful softweare to screencut and screenrerecorder , which is  developed with Qt5.The software can add circular marquee, rectangular marquee, blur, add marker, add text and other functions to the screenshot when taking screenshots.and you can set whether to record sound, display the recording mouse, set the recording frame rate and other settings when recording screen.


## Index

- [Features](#features)
- [Usage](#usage)
  - [Dependencies](#dependencies)
  - [Installations](installations)
  - [Terminal Commond](terminal commond)
  - [Keyboard Shortcuts](#keyboard-shortcuts)

## Features

- Customizable appearance.
- Easy to use.
- In-app screenshot  and  screenrecorder edition.
- DBus interface.

## Usage
### Dependencies
You will need the following packages to compile kylin-screenshot:
-	        qt5-qmake,
-               qtbase5-dev,
-               qttools5-dev-tools,
-               libqt5svg5-dev,
-               libavformat-dev,
-               libavcodec-dev,
-               libavutil-dev,
-               libswscale-dev,
-               libpulse-dev,
-               libqt5x11extras5,
-               libqt5x11extras5-dev,
-               libxfixes3,
-               libxfixes-dev,
-               libx11-dev,
-               libxinerama1,
-               libxinerama-dev,
You can Build and Install by apt command
```shell
sudo apt install kylin-screenshot-build-deps_1.0.0-1_all.deb
```
###Installations
After installing all the dependencies, finally run the following commands in the sources root directory:
```shell
git clone https://github.com/ubuntukylin/kylin-screenshot.git
cd kylin-screenshot
debuild
cd ..
dpkg -i kylin-screenshot_1.0.0-1_amd64.build
```

### Terminal Command
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
### Keyboard Shortcut
These shortcuts are available in GUI mode:

|  Keys                                                                     |  Description                                                   |
|---                                                                        |---                                                             |
| <kbd>←</kbd>, <kbd>↓</kbd>, <kbd>↑</kbd>, <kbd>→</kbd>                    | Move selection 1px                                             |
| <kbd>Shift</kbd> + <kbd>←</kbd>, <kbd>↓</kbd>, <kbd>↑</kbd>, <kbd>→</kbd> | Resize selection 1px                                           |
| <kbd>Esc</kbd>                                                            | Quit capture                                                   |
| <kbd>Ctrl</kbd> + <kbd>C</kbd>                                            | Copy to clipboard                                              |
| <kbd>Ctrl</kbd> + <kbd>S</kbd>                                            | Save selection as a file                                       |
| <kbd>Ctrl</kbd> + <kbd>Z</kbd>                                            | Undo the last modification                                     |
| <kbd>Spacebar</kbd>                                                       | Toggle visibility of sidebar with options of the selected tool |

 
