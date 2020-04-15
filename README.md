# STM32H7 dual-core inter-CPU communication

Example project for inter-processor communication between 2 cores, ARM Cortex-M7 and ARM-Cortex-M4 cores.

## How it works

This example demonstrates how to implement communication between both cores to exchange data.
It uses shared RAM and `2` separate ring buffers, acting like pipe (single input, single output)
in both direction. First buffer from CPU1 to CPU2, second buffer from CPU2 to CPU1.

## Used hardware

Example runs on official ST Nucleo boards for dual-core STM32H7 series, listed below.

* [Nucleo-H745ZI-Q](https://www.st.com/en/evaluation-tools/nucleo-h745zi-q.html)
* [Nucleo-H755ZI-Q](https://www.st.com/en/evaluation-tools/nucleo-h755zi-q.html)

## STM32H7 option bytes configuration

STM32H7 dual-core CPU must have some option bytes configured to run the example properly.
Example is configured for this configuration:

* CPU1 (Cortex-M7) flash memory address is `0x08000000` and its size is `1MB`
* CPU2 (Cortex-M4) flash memory address is `0x08100000` and its size is `1MB`
* CPU2 is by default disabled and is active with software instruction executed by CPU1

> Option bytes can be configured using [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) software

## Run examples

Run examples with [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) toolchain.
