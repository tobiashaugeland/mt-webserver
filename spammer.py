import socket
from threading import Thread
import time
import sys
import argparse

HOST = "127.0.0.1"
PORT = 8000
request = "GET /index.html"
parser = argparse.ArgumentParser()
parser.add_argument("-t", "--threads", type=int, default=1000)
number_of_requests = parser.parse_args().threads

def req(tid):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(request.encode())
        response = s.recv(1024)
        print(tid)

threads = []

start = time.time()
for i in range(number_of_requests):
    thread = Thread(target=req, args=(i,))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()

end = time.time()

print("Finished!")
print(f"\n{number_of_requests} requests took {end - start} seconds")