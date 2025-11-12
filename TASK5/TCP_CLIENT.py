import socket 
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect(("127.0.0.1",8080))
s.sendall(b"hello from client!")
while True:
    msg = s.recv(1024).decode()
    if not msg:
        break
    print(msg, end="")

    if "Enter" in msg:
        data = input()
        s.sendall(data.encode())

s.close()

