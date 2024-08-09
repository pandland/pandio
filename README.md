> ⚠️ Early stage of development.

# luxio

Simple async io library for C (it will be in the future, for now it's just HTTP server) - it will be used across my personal projects.

### TODO:

- [x] Timers using heap data structure (works like `setTimeout`, `setInterval`).
- [x] Non-blocking IO for sockets using epoll.
- [x] Basic networking abstraction to handle TCP.
- [ ] Thread pool and files handling.
- [ ] Support many platforms: Windows, MacOS and FreeBSD.

```sh
make && ./build/server
```

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
