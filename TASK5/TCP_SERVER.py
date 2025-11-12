import socket 

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind(("0.0.0.0", 8080))
server.listen(1)

print("Server listening...")

conn, addr = server.accept()
print("Connected by", addr)

data = conn.recv(1024)
print("Received:", data.decode())

conn.sendall(b"Hello from server!\n")

# Ask for first number
conn.sendall(b"Enter first number: ")
num1 = conn.recv(1024).decode().strip()

# Ask for second number
conn.sendall(b"Enter second number: ")
num2 = conn.recv(1024).decode().strip()

# Ask for operation
conn.sendall(b"Enter operation (+, -, *, /): ")
op = conn.recv(1024).decode().strip()

# Perform calculation safely
try:
    a = float(num1)
    b = float(num2)

    if op == "+":
        result = a + b
    elif op == "-":
        result = a - b
    elif op == "*":
        result = a * b
    elif op == "/":
        if b == 0:
            result = "Error: Division by zero"
        else:
            result = a / b
    else:
        result = "Invalid operation"
except ValueError:
    result = "Invalid number input"

# Send result back to client
conn.sendall(f"Result: {result}\n".encode())

conn.close()
server.close()

