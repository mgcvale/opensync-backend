# Hyprsync-backend

This is the source code for the backend API for the Hyprsync project, made using mongoose with C.

## Building

To build the project, you will need cJson and sqlite3.

To install them, refer to your OS's documentation.
In Arch Linux, you can run:
`sudo pacman -S sqlite`
 and
`sudo pacman -S cjson`

After installing the libs, you can create a build directory, run cmake, and then ninja:

```
mkdir build; cd build
cmake ..
ninja
./hyprsync-backend
```

Thats it!
