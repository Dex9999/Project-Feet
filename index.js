var robot = require("robotjs");
const { SerialPort } = require('serialport');
let x,y, curr;
// Create a port
const port = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 9600, // change later if too slow
})

// port.on('open', () => {
//     setInterval(() => port.write('active!'), 3000);
// });
//TODO fix data recieving lmao
let partial = "";
port.on('data', (data) => {
    // console.log(`Received serial data: ${data.toString()}`);
    partial += data.toString();
    // console.log(partial);
    const dataParts = partial.split("*");
    if (dataParts.length > 1) {
        const fullData = dataParts.shift();
        partial = dataParts.join("*");
        if(fullData.startsWith("{") && fullData.endsWith("}")){
            processData(fullData);
        } // else ignore, data is corrupted
    }

    // console.log(x,y)
    // curr = robot.getMousePos();
    // robot.moveMouseSmooth(curr.x+x,curr.y+y)
});
function processData(data){
    console.log(data);
}
function moveMouse(x, y){
    let curr = robot.getMousePos();
    robot.moveMouseSmooth(curr.x+x, curr.y+y);
}