# Universal Machine Emulator

This project is a full implementation of a Turing-complete Universal Machine (UM) capable of interpreting and executing programs written in a custom 32-bit instruction set architecture. The UM emulates low-level memory management, instruction decoding, and register-based execution, mimicking the core principles of real-world computer architecture.

## Overview

The UM loads a binary file into memory and executes it instruction-by-instruction, supporting operations such as conditional moves, arithmetic, I/O, dynamic memory allocation, and program loading. It also incorporates its own segmented memory system and a suite of unit tests to ensure functional correctness.

Through developing this project, I gained experience with:

- Bit-level data manipulation and instruction decoding

- Dynamic memory and segment management

- Designing and optimizing register-based virtual machines

- Performance tuning and debugging using profiling tools

This emulator demonstrates a deep understanding of systems programming and low-level computation. It is capable of running full-scale applications, including a basic operating system, text-based games, and benchmark programs.

## Usage

To compile the UM:

```bash
make um
```

To run a binary program:

```bash
./um [program.um]
```
