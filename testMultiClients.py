import subprocess

server = subprocess.Popen(['UDPServer\\build\\UDPServer.exe'])

clients = []
for x in range(10):
    clients.append(subprocess.Popen(['UDPClient\\build\\UDPClient.exe', str(9090 + x)]))

for i, client in enumerate(clients):
    client.wait()
