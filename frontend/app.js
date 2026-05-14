const socket = new WebSocket('ws://localhost:8080');

let totalOrders = 0;
let ordersPerSecond = 0;
let sessionSeconds = 0;
let currentMode = 'NORMAL';
let frontendRunning = false;
let renderCounter = 0;

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

    ++totalOrders;
    ++ordersPerSecond;

    const data = JSON.parse(event.data);

    if (data.type === 'trade') {
        renderCounter++;
        renderCounterData(renderTrade, data);
    }

    if (data.type === 'orderbook') {
        renderCounterData(renderOrderbook, data);
    }
};

const renderCounterData = (renFun, data) => {
    let sampleRate = 80;

    if (currentMode === 'HFT') {
        sampleRate = 100;
    }
    else if (currentMode === 'STRESS') {
        sampleRate = 120;
    }

    if (renderCounter % sampleRate === 0) {
        renFun(data);
    }
}

const sessionTimeInterval = setInterval(() => {
    if (frontendRunning) {
        sessionSeconds++;
        document.getElementById('totalOrders').innerText = totalOrders;
        document.getElementById('elapsedTime').innerText = sessionSeconds + 's';
        document.getElementById('ordersPerSec').innerText = ordersPerSecond;
        ordersPerSecond = 0;
    }
}, 1000);

let running = false;
let modeElement = document.getElementById('currentMode');

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
        currentMode = 'NORMAL';
        modeElement.innerText = currentMode;
        modeElement.classList.add('blue');
        modeElement.classList.remove('red');
        modeElement.classList.remove('green');
        socket.send(
            'NORMAL'
        );
    };

document
    .getElementById(
        'hftBtn'
    )
    .onclick = () => {
        currentMode = 'HFT';
        modeElement.innerText = currentMode;
        modeElement.classList.add('green');
        modeElement.classList.remove('blue');
        modeElement.classList.remove('red');
        socket.send(
            'HFT'
        );
    };

document
    .getElementById(
        'stressBtn'
    )
    .onclick = () => {
        currentMode = 'STRESS';
        modeElement.innerText = currentMode;
        modeElement.classList.add('red');
        modeElement.classList.remove('blue');
        modeElement.classList.remove('green');
        socket.send(
            'STRESS'
        );
    };

document
    .getElementById(
        'resetBtn'
    )
    .onclick = () => {
        totalOrders = 0;
        ordersPerSecond = 0;
        sessionSeconds = 0;
        currentMode = 'NORMAL';
        running = false;
        frontendRunning = false;
        renderCounter = 0;

        socket.send('RESET');
        socket.send('NORMAL');
        document
            .getElementById(
                'toggleBtn'
            )
            .innerText = '▶ Start';


        modeElement.innerText = currentMode;
        modeElement.classList.add('blue');
        modeElement.classList.remove('red');
        modeElement.classList.remove('green');


        document
            .getElementById(
                'ordersPerSec'
            )
            .innerText = '0';

        document
            .getElementById(
                'totalOrders'
            )
            .innerText = '0';

        document
            .getElementById(
                'elapsedTime'
            )
            .innerText = '0s';

        document
            .getElementById(
                'trades'
            )
            .innerHTML = '';

        document
            .getElementById(
                'bids'
            )
            .innerHTML = '';

        document
            .getElementById(
                'asks'
            )
            .innerHTML = '';
    };

window.addEventListener('beforeunload', (event) => {
    socket.send('NORMAL');
});



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
        `trade ${data.side === 'BUY'
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