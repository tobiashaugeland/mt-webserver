from requests_futures.sessions import FuturesSession
import requests
URL = "http://127.0.0.1:8000/index.html"
PORT = 8000

session = FuturesSession()
# req = requests.get(URL)
# print(req)

for i in range(10):
    r = session.get(URL)
    print(i)
