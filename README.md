> ⚠️ Early stage of development.

<img src="https://github.com/user-attachments/assets/cf70d4ac-9cef-4c66-8295-04946abdafd2" alt="banner" />

# Pandio

Simple library for non-blocking I/O operations. Developed mainly for my PandJS project (JavaScript runtime).

### TODO:

- [x] Timers using heap data structure (works like `setTimeout`, `setInterval`).
- [x] Non-blocking IO for sockets using epoll.
- [x] Basic networking abstraction to handle TCP.
- [x] Thread pool
- [x] Support Windows
- [ ] Async files operations
- [ ] Support FreeBSD/MacOS (`kqueue` + posix threads).
- [ ] Better error handling and error reporting to the end-user.

### Build static library

Make sure you have `make` utility installed. Library uses just system calls, so it does not have any external dependencies.

```sh
make
```

> This command will create `libpandio.a` file inside `build` directory.

### Running tests

Project uses [Criterion](https://github.com/Snaipe/Criterion) for unit testing:

```sh
# Make sure you have installed criterion on your system

# Ubuntu
apt-get install libcriterion-dev
# Arch Linux (AUR)
pacaur -S criterion

# Running tests:
make test
```

## License

Distributed under the MIT License. See `LICENSE` for more information.
