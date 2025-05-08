from socket import *

def run():
  HOST="127.0.0.1"
  PORT=8888
  s = socket(AF_INET, SOCK_STREAM)
  s.connect((HOST, PORT))
  msg = "Hello World"
  s.send(msg.encode())
  data = s.recv(1024)
  print(data.decode())
  s.close()

if __main__ == "__main__":
  run()
