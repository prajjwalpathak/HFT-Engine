const WebSocket = require('ws');
const dgram = require('dgram');

let packetsReceived = 0;
const wss = new WebSocket.Server({ port: 8080 });

console.log('WebSocket bridge running on port 8080');

const udpSocket = dgram.createSocket('udp4');

udpSocket.bind(5000, () => {
    console.log('Listening for UDP market data on port 5000');
}
);

udpSocket.on('message', (msg) => {

    packetsReceived++;

    if (packetsReceived % 10000 === 0) {

        console.log(
            'UDP packets:',
            packetsReceived
        );
    }

    const tradeEvent = {

        type: 'trade',

        price:
            10000 +
            Math.floor(
                Math.random() * 100
            ),

        quantity:
            1 +
            Math.floor(
                Math.random() * 500
            ),

        side:
            Math.random() > 0.5
                ? 'BUY'
                : 'SELL'
    };

    wss.clients.forEach((client) => {

        if (
            client.readyState ===
            WebSocket.OPEN
        ) {

            client.send(
                JSON.stringify(
                    tradeEvent
                )
            );
        }
    });
});

wss.on('connection', (ws) => {
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