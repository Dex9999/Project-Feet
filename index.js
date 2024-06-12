var robot = require("robotjs");
const { SerialPort } = require('serialport');
let x,y, curr;
// Create a port
const port = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 9600, // change later if too slow
})

let partial = "";
port.on('data', (data) => {
    partial += data.toString();
    const dataParts = partial.split("*");
    if (dataParts.length > 1) {
        const fullData = dataParts.shift();
        partial = dataParts.join("*");
        if(fullData.startsWith("{") && fullData.endsWith("}")){
            processData(fullData);
        } // else ignore, data is corrupted
    }
});

function processData(data){
    try{
        let json = JSON.parse(data);
        // console.log(json);
        if(json.type == "mouse"){
            let x = json.move.x;
            let y = json.move.y;

            if(x>100 || x < 0){
                x = 0;
            } else {
                x = mapNumber(x, 8, 20, -20, 20);
            }
            if(y>100 || y < 0){
                y = 0;
            } else {
                y = mapNumber(y, 8, 20, -20, 20);
            }

            // moveAbsolute(x,y);
            moveMouse(x,y);
        }
        // if(json.type = "key"){

        // }
    } catch(err){
        console.log("tried to parse broken data", err)
    }
}

function moveAbsolute(x,y){
    let screenSize = robot.getScreenSize();
    robot.moveMouse(mapNumber(x, -20, 20, 0, screenSize.width),mapNumber(y, -20, 20, 0, screenSize.height));
}

function moveMouse(x, y){
    console.log(x,y)
    let curr = robot.getMousePos();
    robot.moveMouse(curr.x+x, curr.y+y);
}


function mapNumber (num, in_min, in_max, out_min, out_max) {
    return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}