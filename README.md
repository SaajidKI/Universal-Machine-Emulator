# Universal Machine Emulator

This project is a complete implementation of a Turing-complete Universal Machine (UM) designed to interpret and execute binary programs written in a custom 32-bit instruction set architecture. It simulates a low-level computing environment by handling memory segmentation, instruction decoding, and register-based execution.

## Overview

The machine supports a range of fundamental operations, including conditional moves, arithmetic, input and output, dynamic memory allocation, and program loading and unloading. By replicating these essential features, the UM serves as a simplified model of a physical computer system. Building it involved careful attention to bit-level manipulation, efficient memory handling, and adherence to architectural constraints. 

Through developing this project, I gained experience with:

- Bit-level data manipulation and instruction decoding

- Dynamic memory and segment management

- Designing and optimizing register-based virtual machines

- Performance tuning and debugging using profiling tools

This emulator demonstrates a deep understanding of system-level programming, performance tradeoffs, and how hardware-level design influences the behavior of software systems. It is capable of running full-scale applications, including a basic operating system, benchmark programs, and interactive games.

## Usage

To compile the UM:

```bash
make um
```

To run a binary program:

```bash
./um [program.um]
```
