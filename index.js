// Type "Hello World" then press enter.
var robot = require("robotjs");
const { SerialPort } = require('serialport');

// Create a port
const port = new SerialPort({
    path: '/dev/ttyACM1',
    baudRate: 57600, // change later if too slow
})

port.on('open', () => {
    setInterval(() => port.write('active!'), 3000);
});

port.on('data', (data) => {
    console.log(`Received serial data: ${data.toString()}`);
});