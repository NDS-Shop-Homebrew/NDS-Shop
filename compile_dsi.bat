@echo off
cd /d "%~dp0DSi"
set "MSYS2_PATH_TYPE=inherit"
set "MSYSTEM=UCRT64"
set "BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core"
set "BLOCKSDSEXT=/opt/wonderful/thirdparty/blocksds/external"
set "WONDERFUL_TOOLCHAIN=/opt/wonderful"
set "PATH=C:\msys64\opt\wonderful\toolchain\gcc-arm-none-eabi\bin;C:\msys64\usr\bin;C:\msys64\opt\wonderful\bin;%PATH%"
C:\msys64\usr\bin\bash.exe --login -i -c "cd /d/Projets/NDS-Shop-all-project/NDS-Shop/DSi && python build.py release; echo '--- DONE ---'; exec bash"
