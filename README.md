# feup-vcom
Repo for VCOM (Computer Vision) class in FEUP (MIEIC).

# Steps for using conan

1. Install Conan from https://www.conan.io/downloads

2. Generate dependencies/files 

```
conan install  --build missing
```

3. Generate cmake files

```
//Eclipse
cmake  -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
```
```
//Visual Studio
cmake -G "Visual Studio 14 Win64" --build . --config Release
```
