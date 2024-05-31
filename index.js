var robot = require("robotjs");
const { SerialPort } = require('serialport');
let x,y, curr;
// Create a port
const port = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 57600, // change later if too slow
})

port.on('open', () => {
    setInterval(() => port.write('active!'), 3000);
});
//TODO fix data recieving lmao
port.on('data', (data) => {
    console.log(`Received serial data: ${data.toString()}`);
    x = parseInt(data.toString().split(" ")[0]);
    y = parseInt(data.toString().split(" ")[1]);
    console.log(x,y)
    curr = robot.getMousePos();
    robot.moveMouseSmooth(curr.x+x,curr.y+y)
});