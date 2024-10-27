# opensync-backend

This is the source code for the backend API for the opensync project, made using mongoose with C.

## Building

Firstly, this server was developed for linux, obviously. If you are a developer, and are using windows, you can use WSL to compile and test it, but I strongly advise against using windows for deploying your home server. There is no reason to do that.

That said, this webserver runs in both AMD64 / x86/64 and ARM architectures, and is well suited for running in embedded systems using mini-pcs and microcontrollers, although you may need to statically link some libraries to work with different OS's, as was done with cUnity and mongoose. You may also need to change some functions of the server, such as the code for creating/managing files, if you are not running it under linux. If needed, you can sumbit an Issue, and I can try to adapt the code.

Have fun!

### Development

To build the project for development purposes, you will need:
- openssl
- cJSON
- sqlite

To install them, refer to your OS's documentation. Here are some examples:

Arch Linux:
`sudo pacman -S sqlite openssl cjson`

openSUSE Tumbleweed:
`sudo zypper install sqlite3-devel openssl-devel cJSON-devel`

Alpine Linux:
`apk add cmake cjson-dev openssl-dev sqlite-dev curl-dev`

You will also need the build essentials for your distro, such as build-base for alpine.
More specifically, you will need cmake and ninja or make.

After installing the libs, you can create a build directory, run cmake, and then ninja:

```
mkdir build; cd build
cmake ..
make
./opensync-backend migrate
.opensync-backend
```

If you want to run the tests, you can do:
```
cd test
mkdir build; cd build
cmake ..; make
./opensync-test
```

Thats it!

### Deployment

For deploying, follow the same steps as the compilation for development.

You may also change the config.json, located in the same directory as the binary, to change the defualt directory for user data storage, and change the database name for something more specific.
