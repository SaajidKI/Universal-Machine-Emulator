# UM Test Binaries

This package includes several binary programs designed to run on a Universal Machine (UM) emulator. These programs vary in complexity and runtime, making them well-suited for testing input/output functionality, measuring performance, and experimenting with interactive UM behavior.

## Included Binaries
### `hello.um`
Prints a standard greeting message to the terminal.

### `cat.um`
Copies standard input to standard output.

**Example:**
```bash
echo Hello, world. | ./um cat.um
```

### `midmark.um`
A medium-scale performance benchmark. When executed, it prints a sequence of hexadecimal output lines and concludes with a benchmark complete message. Useful for testing general performance and stability.

### `sandmark.umz`
A self-decompressing program that runs a large-scale performance test. Once executed, it outputs the results to `sandmark.out`. Intended for evaluating decompression and runtime efficiency.

### `advent.umz`
A lightweight text-based adventure game. Navigate through a minimal interactive story world. `advent.txt` contains the solution to the puzzle.

### `codex.umz`
Launches a small operating system and includes a series of hidden puzzles for exploration. May take 30 seconds or more to load. Offers a fun and challenging experience for those interested in reverse engineering and problem-solving.
