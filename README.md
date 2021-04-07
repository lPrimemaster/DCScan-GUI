# DCScan-GUI
This repository contains all the code responsible for handling the DCS GUI Application and the server console CLI.

If you landed here in search for the backend library see [here](https://github.com/lPrimemaster/DCScan-ModulesAPI).


## Documentation
The present code base is not documented. If you wish it to be, open an issue, or contribute.

## Building

Installing the GUI application from source is as simple as:

Assuming you have compiled and installed the DCSModules-API libraries (refer to [this](https://github.com/lPrimemaster/DCScan-ModulesAPI/blob/master/README.md)).

> :warning: Qt 5.15 or higher is required for the client target to compile.

```shell
git clone https://github.com/lPrimemaster/DCScan-GUI.git
cd DCScan-GUI
mkdir build && cd build
cmake ..
cmake --build . --config Release --target ALL_BUILD /maxcpucount:14
```

Copy and paste the generated API library `*.dll` files into the executable directory (both server and client).

If you just want the binaries and dont want to build from source. Check [here](https://github.com/lPrimemaster/DCScan-GUI/releases).
