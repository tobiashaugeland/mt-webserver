# mt-webserver
Multithreaded webserver in C. Practical exercise 2 in TDT4186.

## Compilation
```bash
make
```

## Running the server

```bash
./mtwwwd
```
If you want to specify another folder to serve, you can do so by passing the
arguments in the following way:

```bash
./mtwwwd myOtherFolder 8001 4 10
```

The other arguments are:
* port
* number of threads
* ring buffer size
