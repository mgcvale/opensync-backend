# opensync-backend

This is the source code for the backend API for the opensync project, made using mongoose with C.

## Building

To build the project, you will need:
- openssl
- cJSON
- sqlite

To install them, refer to your OS's documentation.
In Arch Linux, you can run:

`sudo pacman -S sqlite openssl cjson`

And on openSUSE, you can get them via:
`sudo zypper install sqlite3-devel openssl-devel cJSON-devel`

You will also need ninja and CMake, of course.

After installing the libs, you can create a build directory, run cmake, and then ninja:

```
mkdir build; cd build
cmake ..
ninja
./hyprsync-backend
```

Thats it!
