# HTTP Server in C

Goals:
- [ ] parse HTTP request
- [ ] multi-threaded
- [ ] handle multiple concurrent connections in non-blocking way (Chrome blocks event loop, because it opens multiple TCP connections even for single HTTP request)

## Resources
- [Beej's Guide to Network Programming](https://www.beej.us/guide/bgnet/html/split/index.html)
