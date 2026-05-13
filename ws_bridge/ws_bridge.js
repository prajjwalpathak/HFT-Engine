const WebSocket = require('ws');

const wss = new WebSocket.Server({port: 8080});

console.log('WebSocket bridge running on port 8080');

wss.on('connection',(ws) => {
        console.log('Browser connected');

        ws.on('close', () => {
                console.log('Browser disconnected');
            }
        );

        ws.on('message', (message) => {
                console.log('Received:', message.toString());
            }
        );
    }
);