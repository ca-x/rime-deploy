# rime-deploy

A command-line tool for deploying RIME Input Method Engine configurations, written in C.

## Features

- Automatic installation of RIME input method
- Configuration backup and restore
- Rime-ice configuration deployment
- Custom configuration support
- Cross-platform support (Windows, macOS, Linux)
- Interactive configuration setup
- Detailed logging

## Prerequisites

- Git
- GCC or compatible C compiler
- Make

### Platform-specific Requirements

- **macOS**: Homebrew
- **Linux**: apt/dnf package manager
- **Windows**: Manual RIME installation required

## Building

```bash
mkdir -p build
make

## reference

- [rime-auto-deploy](https://github.com/Mark24Code/rime-auto-deploy)
- [雾凇拼音](https://github.com/iDvel/rime-ice)
