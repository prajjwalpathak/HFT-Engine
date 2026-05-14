const socket = new WebSocket('ws://localhost:8080');

let messagesReceived = 0;
let frontendRunning = false;

socket.onopen = () => {
    socket.onerror = (err) => {
        console.error('WebSocket error:', err);
    };

    socket.onclose = () => {
        console.log('WebSocket closed');
    };

    console.log('Connected to WebSocket bridge');
};

socket.onmessage = (event) => {

    if (!frontendRunning) {
        return;
    }

    messagesReceived++;

    document.getElementById('feedMessages').innerText = messagesReceived;

    const data = JSON.parse(event.data);

    if (data.type === 'trade') {
        renderTrade(data);
    }

    if (data.type === 'orderbook') {
        renderOrderbook(data);
    }
};

let running = false;

document
    .getElementById(
        'toggleBtn'
    )
    .onclick = () => {

        running = !running;
        frontendRunning = running;

        socket.send(
            running
                ? 'START'
                : 'STOP'
        );

        document
            .getElementById(
                'toggleBtn'
            )
            .innerText =

            running
                ? '⏸ Stop '
                : '▶ Start';
    };

document
    .getElementById(
        'normalBtn'
    )
    .onclick = () => {
        socket.send(
            'NORMAL'
        );
    };

document
    .getElementById(
        'hftBtn'
    )
    .onclick = () => {
        socket.send(
            'HFT'
        );
    };

document
    .getElementById(
        'stressBtn'
    )
    .onclick = () => {
        socket.send(
            'STRESS'
        );
    };

function renderTrade(data) {

    const trades =
        document.getElementById(
            'trades'
        );

    const div =
        document.createElement(
            'div'
        );

    div.className =
        `trade ${
            data.side === 'BUY'
            ? 'buy'
            : 'sell'
        }`;

    div.innerText =
        `${data.side} ` +
        `${data.quantity} @ ` +
        `${data.price}`;

    trades.prepend(div);

    if (
        trades.children.length > 50
    ) {

        trades.removeChild(
            trades.lastChild
        );
    }

    document.getElementById(
        'ordersPerSec'
    ).innerText =
        messagesReceived;
}

function renderOrderbook(data) {

    const bids =
        document.getElementById(
            'bids'
        );

    const asks =
        document.getElementById(
            'asks'
        );

    bids.innerHTML = '';

    asks.innerHTML = '';

    data.bids.forEach(
        (bid) => {

            const row =
                document.createElement(
                    'div'
                );

            row.className =
                'book-row bid-row';

            row.innerHTML =
                `<span>${bid.price}</span>
                 <span>${bid.quantity}</span>`;

            bids.appendChild(
                row
            );
        }
    );

    data.asks.forEach(
        (ask) => {

            const row =
                document.createElement(
                    'div'
                );

            row.className =
                'book-row ask-row';

            row.innerHTML =
                `<span>${ask.price}</span>
                 <span>${ask.quantity}</span>`;

            asks.appendChild(
                row
            );
        }
    );
}