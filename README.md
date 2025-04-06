
# Wish Shell

`wish` is a simple Unix shell implemented in C. It supports basic command execution, path management, built-in commands, and output redirection.

## Features

- Execute commands from the system PATH
- Built-in commands:
  - `exit` — exits the shell
  - `cd <dir>` — changes the current working directory
  - `path <dir1> <dir2> ...` — sets directories to search for executables
- Output redirection using `>`
- Concurrent command execution using `&`
- Script mode (reads from a file)

## Build

Compile the code using `gcc`:

```bash
gcc -o wish wish.c
```

## Usage

### Interactive Mode

```bash
./wish
```

You'll see a prompt like:

```bash
wish>
```

Then you can enter commands like:

```bash
wish> ls -l
wish> cd /tmp
wish> path /bin /usr/bin
wish> ls > out.txt
```

### Script Mode

You can also run commands from a file:

```bash
./wish script.txt
```

## Notes

- Commands are split by `&` to run in parallel.
- Redirection (`>`) is supported, but only one redirection per command is allowed.
- Error messages are printed to `stderr` with: `An error has occurred`.

## Limitations

- No support for piping (`|`).
- Only standard output redirection (`>`) is supported; no input redirection (`<`) or append redirection (`>>`).
