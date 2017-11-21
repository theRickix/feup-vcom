# feup-vcom
Repo for VCOM (Computer Vision) class in FEUP (MIEIC).

# Steps for using conan

1. Install Conan from https://www.conan.io/downloads

2. Generate dependencies/files 

```ShellSession
conan install  --build missing
```

3. Generate cmake files

```ShellSession
//Eclipse
cmake  -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
```
```ShellSession
//Visual Studio
cmake -G "Visual Studio 14 Win64" --build . --config Release
```
