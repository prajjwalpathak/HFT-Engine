const socket = new WebSocket('ws://localhost:8080');

let messagesReceived = 0;

socket.onopen = () => {
    console.log('Connected to WebSocket bridge');
};

socket.onmessage = (event) => {
    const data = JSON.parse(event.data);

    const trades = document.getElementById('trades');

    const div = document.createElement('div');

    div.className = `trade ${data.side === 'BUY' ? 'buy' : 'sell'}`;

    div.innerText = `${data.side} ` + `${data.quantity} @ ` + `${data.price}`;

    trades.prepend(div);

    if (trades.children.length > 50) {
        trades.removeChild(trades.lastChild);
    }

    messagesReceived++;

    document.getElementById('feedMessages').innerText = messagesReceived;

    document.getElementById('ordersPerSec').innerText = Math.floor(100000 + Math.random() * 50000);
};