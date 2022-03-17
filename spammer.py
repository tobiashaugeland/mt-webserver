import socket
from threading import Thread
import time
import sys
import argparse

HOST = "127.0.0.1"
request = "GET /index.html"
parser = argparse.ArgumentParser()
parser.add_argument("-r", "--requests", type=int, default=100)
parser.add_argument("-p", "--ports", type=int, default=8000)

PORT = parser.parse_args().ports
number_of_requests = parser.parse_args().requests
threads = []


def req(id):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(request.encode())
        response = s.recv(1024)

        # Uncomment print statement to see the the current thread number
        # This will help to see which response is being received
        # print(id)


start = time.time()
for i in range(number_of_requests):
    thread = Thread(target=req, args=(i,))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()

end = time.time()

print("Finished!")
print(f"\n{number_of_requests} requests took {(end - start)*1000:.0f} ms")