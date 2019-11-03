# luminosus-core
This is a cross-platform framework for modular applications using C++, Qt  and QML.

It contains everything that was common in all of the apps that I built before and in addition some helpers that may be useful and don't require any further dependencies.

**Attention:** This repository is still work-in-progress and currently mostly intended for my personal use. Although all features mentioned below were already implemented in previous projects, they may not be accessible yet from the API of this framework. A good starting point to see what is already working is [luminosus-minimal](https://github.com/luminosuslight/luminosus-minimal). Nevertheless creating issues and pull requests is welcome.

Things you don't need to take care of anymore by using this framework:

* persisting data in a fast, modern and transparent way
  * separated into projects that can be exported and imported to and from files
  * remove the need for a "Save" button by saving every few seconds automatically and whenever required
  * using JSON and CBOR as a universal file format
* modularize the code and UI into so called _blocks_
  * UNIX philosophy: do one thing and do it well
  * easily testable
  * create connections between them that forward integer, floating point, matrix and string values
  * support many-to-many connections including different merge strategies
* connect multiple devices running the same app
  * show instances in the same network
  * download projects from other instances and load projects into other instances from remote
  * use a secure websocket connection to exchange data, even over the internet
* support High DPI screens and touch as well as mouse and keyboard
  * includes a High DPI mechanism that works better and more reliable than Qt's own and supports arbitrary floating point scale factors, in QML as easy to use as "width: 20*dp"
  * provides a new TouchArea QML item that improves the multi-touch support of Qt and gives it an easy to use and recognizable API
* implement common helpers and useful QML items
  * fills in some common functions that are missing in Qt
  * provides StretchRow and StretchColumn QML items to easily create resizable UIs

## Requirements

* Target Platform: Linux / macOS / Windows / Android / iOS
* C++14
* latest Qt release (as of October 2019: Qt 5.13)
* qmake-based project (or write your own build files)
* to be continued...

## Integration

Add this repository to your project using `git submodule add git@github.com:luminosuslight/luminosus-core.git core` and add `include(core/luminosus-core.pri)` to your qmake (.pro) file.

You also need to create an SSL certificate (`data/luminosus_websocket.cert` and `data/luminosus_websocket.key`). Please use the standard commands you can find on the Internet to create those.

You can find a small example how to use this framework here: [luminosus-minimal](https://github.com/luminosuslight/luminosus-minimal)

## Possible Use-Cases

Here are some examples of apps that I already built with code from this frameworks:

* node-based lighting controller
* speech assistant with modular conversation editor
* smart home controller
* personal cloud including image database server and image viewer client
* health diary
* social network app
* cellular image analysis software
