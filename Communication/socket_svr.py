from socket import *

def run():
  s = socket(AF_INET, SOCK_STREAM)
  
  s.bind(("127.0.0.1",8888))
  s.listen()
  
  conn, addr = s.accept()
  
  while True:
      data = conn.recv(1024)
      if not data:
          break
  
      msg = data.decode() + "*"
      conn.send(msg.encode())
  
  conn.close()

if __name__ == "__main__":
	run()
