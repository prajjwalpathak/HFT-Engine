const WebSocket = require('ws');
const dgram = require('dgram');

let packetsReceived = 0;
const TRADE = 1;
const TOP_OF_BOOK = 2;
let lastSequence = 0;

let engineStats = {

    orders: 0,

    trades: 0,

    cancelled: 0,

    active: 0
};

// setInterval(() => {
//     sendControlCommand(
//         'STATS'
//     );
// }, 1000);

const wss =
    new WebSocket.Server({
        port: 8080
    });

console.log(
    'WebSocket bridge running on port 8080'
);

const udpSocket =
    dgram.createSocket('udp4');

const controlSocket =
    dgram.createSocket('udp4');

udpSocket.bind(
    5000,
    () => {

        console.log(
            'Listening for UDP market data on port 5000'
        );
    }
);

function sendControlCommand(command) {

    console.log('Sending:', command);

    controlSocket.send(
        Buffer.from(command),
        5001,
        '127.0.0.1'
    );
}

udpSocket.on('message', (msg) => {

    packetsReceived++;

    const type = msg.readUInt8(0);

    if (type === 1) {

        const price = msg.readUInt32LE(40);

        const quantity = msg.readUInt32LE(44);

        const sideRaw = msg.readUInt8(48);

        const side = sideRaw === 0 ? 'BUY' : 'SELL';

        const timestamp = Number(msg.readBigUInt64LE(56));

        wss.clients.forEach(
            (client) => {

                if (client.readyState === WebSocket.OPEN) {
                    client.send(
                        JSON.stringify({
                            type: 'trade',
                            price,
                            quantity,
                            side
                        })
                    );
                }
            }
        );
    }

    else if (type === 2) {

        const bidPrice =
            msg.readUInt32LE(16);

        const bidQty =
            msg.readUInt32LE(20);

        const askPrice =
            msg.readUInt32LE(24);

        const askQty =
            msg.readUInt32LE(28);

        wss.clients.forEach(
            (client) => {

                if (client.readyState === WebSocket.OPEN) {

                    client.send(
                        JSON.stringify({

                            type: 'orderbook',

                            bids: [
                                {
                                    price: bidPrice,
                                    quantity: bidQty
                                }
                            ],

                            asks: [
                                {
                                    price: askPrice,
                                    quantity: askQty
                                }
                            ]
                        })
                    );
                }
            }
        );
    }
}
);

wss.on('connection', (ws) => {
    console.log('Browser connected');

    ws.on('message', (message) => {

        const command = message.toString();

        console.log('Frontend command:', command);

        sendControlCommand(command);
    }
    );

    ws.on('close', () => {
        console.log('Browser disconnected');
    }
    );
}
);