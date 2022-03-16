import socket
from threading import Thread

HOST = "127.0.0.1"
PORT = 8000
request = "GET /index.html"

def req(tid):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(request.encode())
        response = s.recv(1024)
        print(tid)


for i in range(10):
    Thread(target=req, args=[i]).start()