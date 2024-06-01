> ⚠️ Early stage of development.

# HTTP Server in C

## pre-fork model

- Master process inits listener socket and spawns N workers (using `fork`).
- Each worker has own event loop and uses `epoll` + `fcntl` for non-blocking I/O operations.
- With these techniques I achieved a decent level of concurrency.

## TO-DO
- [x] Non-blocking architecture for handling connections
- [ ] Parse HTTP requests
- [ ] Handling keep-alive requests and timeouts

## Building and running

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


## Benchmark

> ⚠️ There is no HTTP request parsing yet.

In 1 minute test with 7000 concurrent connections I achieved 126 220 requests per second:

```
$ wrk -t10 -c7000 -d60s http://localhost:8080/

Running 1m test @ http://localhost:8080/
  10 threads and 7000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    26.96ms   18.13ms 879.95ms   90.27%
    Req/Sec    12.78k     2.63k   24.79k    69.77%
  7585409 requests in 1.00m, 368.93MB read
  Socket errors: connect 0, read 7585034, write 0, timeout 0
Requests/sec: 126219.65
Transfer/sec:      6.14MB
```

```md
# Specs:
Laptop: Acer Nitro AN515-45
CPU:  AMD Ryzen 5 5600H (12) @ 3.293GHz
GPU: NVIDIA GeForce RTX 3060 (Laptop GPU)
RAM: 32 GB
```

> ⚠️ I ran benchmark inside WSL so resources are probably more limited.

## Resources
- [Beej's Guide to Network Programming](https://www.beej.us/guide/bgnet/html/split/index.html)
- [The C10K problem](http://www.kegel.com/c10k.html)
- [Wikipedia - epoll()](https://en.wikipedia.org/wiki/Epoll)
- [Such Programming - epoll() In 3 Easy Steps!](https://suchprogramming.com/epoll-in-3-easy-steps/)
- [Trung Vuong Thien - A simple HTTP server from scratch](https://trungams.github.io/2020-08-23-a-simple-http-server-from-scratch)
- [HTTP RFC](https://datatracker.ietf.org/doc/html/rfc7230)
