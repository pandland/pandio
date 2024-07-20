> ⚠️ Early stage of development.

# luxio

Simple async io library for C (it will be in the future, for now it's just HTTP server) - it will be used across my personal projects.

### TODO:

- [x] Timers using heap data structure (works like `setTimeout`).
- [x] Non-blocking IO for sockets using epoll.
- [x] Basic networking abstraction to handle TCP.
- [ ] HTTP module.
- [ ] WebSockets module.
- [ ] TLS support.
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

## Resources
- [Beej's Guide to Network Programming](https://www.beej.us/guide/bgnet/html/split/index.html)
- [The C10K problem](http://www.kegel.com/c10k.html)
- [Wikipedia - epoll()](https://en.wikipedia.org/wiki/Epoll)
- [Such Programming - epoll() In 3 Easy Steps!](https://suchprogramming.com/epoll-in-3-easy-steps/)
- [Trung Vuong Thien - A simple HTTP server from scratch](https://trungams.github.io/2020-08-23-a-simple-http-server-from-scratch)
- [HTTP RFC](https://datatracker.ietf.org/doc/html/rfc7230)
