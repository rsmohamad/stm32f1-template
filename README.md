# STM32F1 Project Template

This is a project template for developing STM32F1 using the Standard Peripheral library. This project uses CMake so that it is compatible with CLion, but can also be built without an IDE by using the provided Makefile.

## Dependencies

- gcc-arm compilers
    ```bash
    sudo apt-get install gcc-arm-none-eabi
    ```

- stlink
    Install the latest version by running `./install_stlink.sh`

## Building

Do not invoke `cmake` directly, use the top-level Makefile.

- `make`: compile the project
- `make flash`: flash the generated hex file using stlink
- `make clean`: delete the build files

The project name can be changed in the top-level Makefile.
