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

## Testing the server

### Manual testing

In a terminal you can test with ipv4 the following way:

```bash
curl --http0.9 "127.0.0.1:8000/index.html"
```

To test with ipv6, you can use the following command:

```bash
curl --http0.9 -g -6 "[::1]:8000/index.html"
```

### Automatic testing

If you run the spammer.py script it will spam the server with default 100 requests.

You can also specify your own amount of requests to spam with:

```bash
python3 spammer.py -r number_of_requests -p port
```

### Exploit and safeguard

You can run the relative_path_exploit.py and see that it currently does not
work, but if you change the:

```c
if ((access(full_path, F_OK) == 0) && good_request(full_path) == 1 && is_regular_file(full_path))
```
to

```c
if (access(full_path, F_OK) == 0)
```

You will see that the server will respond with that file
