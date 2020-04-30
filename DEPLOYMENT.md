## Windows

- build release binary
- open MinGW terminal from Windows Start menu -> Qt
- mkdir C:\Users\mitgn\deployment-environment
- cd C:\Users\mitgn\pathology-ml-model-training\build-luminosus-microscopy-Desktop_Qt_5_14_2_MinGW_64_bit-Release\release
- windeployqt.exe -qmldir ../../src --no-opengl-sw --dir C:\Users\mitgn\deployment-environment .


### macOS

- build release binary
- cd build folder
- luminosus-microscopy.app -qmldir=/Users/tim/projects/luminosus-microscopy/src -dmg
