import asyncio
import websockets

PIPE_PATH = "/tmp/ws_pipe"
clients = set()

async def producer():
    """Lectura de líneas del pipe."""
    print(f"Abriendo Pipe")
    while True:
        try:
            with open(PIPE_PATH, "r") as pipe:
                while True:
                    line = pipe.readline()
                    if line:
                        data = line.strip()
                        await broadcast(data)
                    await asyncio.sleep(0.001)  # avoid CPU hog
        except Exception as e:
            print("Error en pipe:", e)
            await asyncio.sleep(1)

async def broadcast(message):
    """Envio de los datos en el pipe a clientes."""
    disconnected = set()
    for ws in clients:
        try:
            await ws.send(message)
        except:
            disconnected.add(ws)
    clients.difference_update(disconnected)

async def handler(websocket, path):
    print("Conectado con el cliente")
    clients.add(websocket)
    try:
        await websocket.wait_closed()
    finally:
        print("Desconexión del cliente")
        clients.remove(websocket)

async def main():
    server = await websockets.serve(handler, "0.0.0.0", 8765)
    print("puerto 8765")
    await producer()

asyncio.run(main())
