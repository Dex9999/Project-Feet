// TODO servo alternate power source battery so less jiggle!
var robot = require("robotjs");
// try to fix regular tracking?
const { SerialPort } = require('serialport');
let x,y, curr;
const port = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 9600, // change later if too slow
})
let setup = true;
let partial = "";
port.on('data', (data) => {
    if (setup) {
        console.log(data.toString()); 
    } 

    partial += data.toString();
    const dataParts = partial.split("*");
    if (dataParts.length > 1) {
        
        const fullData = dataParts.shift();
        partial = dataParts.join("*");
        if(fullData.startsWith("{") && fullData.endsWith("}")){
            setup = false;
            processData(fullData);
        } // else ignore, data is corrupted
    }
});

let zeroCounter = 0;
let mode = 'mouse'; // modes: 'mouse', 'dpad', 'pedal'

let lastMode;
function processData(data){
    try{
        let json = JSON.parse(data);

        let x = json.move.x || 0;
        let y = json.move.y || 0;
        mode = json["type"];
        if(lastMode !== mode){
            robot.keyToggle(lastKey, "up");
        }
        lastMode = mode;
    
        if (mode === 'mouse') {
            mouseMode(x, y);
        } else if (mode === 'key') {
            keyMode(json.key);
        } else if (mode === 'pedal') {
            pedalMode(y);
        }
        
    } catch(err){
        console.log("tried to parse broken data", err)
    }
}
let lastKey = "w";
function keyMode(key) {
    robot.keyToggle(key, "down");
    robot.keyToggle(lastKey, "up");
    lastKey = key;
    console.log(key);
}

// https://ledwan.itch.io/wasd
// https://shellshock.io/
function dpadMode(x, y) {
    // atan2 to figure out coordinates and therefore quadrant (direction)
    // from the x and y distances given (and then convert back to degrees)
    // + 180 to convert -180-180 to 0-360, /45 to split into the 8 fragments
    // then round will give 0-8, % 8 to ensure the unlikely 8 is turned into a 0
    const angle = Math.round((Math.atan2(y, x) * 180 / Math.PI + 180) / 45) % 8;
    // keys starting at right going clockwise
    const directions = [
        ['d'],        // R
        ['s', 'd'],   // DR
        ['s'],        // D
        ['a', 's'],   // DL
        ['a'],        // L
        ['w', 'a'],   // UL
        ['w'],        // U
        ['w', 'd']    // UR
    ];

    const activeDirections = directions[angle];

    ['w', 'a', 's', 'd'].forEach(key => {
        if (activeDirections.includes(key) && !dpadKeys[key]) {
            console.log(key + " down");
            robot.keyToggle(key, 'down');
            dpadKeys[key] = true;
        } else if (!activeDirections.includes(key) && dpadKeys[key]) {
            console.log(key + " down");
            robot.keyToggle(key, 'up');
            dpadKeys[key] = false;
        }
    });
}


// https://kermel.itch.io/drumsss
// https://kodub.itch.io/polytrack
function pedalMode(y){
    console.log(y);
    let threshold = 10;
    if(y < threshold-5){
        console.log("w half");
        robot.keyToggle("w", "down");
    } else if (y < threshold){
        console.log("w")
        robot.keyTap("w");// should alternate between pressed and not, therefore half speed
    } else {
        robot.keyToggle("w", "up")
        console.log("up");
    }
}

function mouseMode(x,y){
    // console.log(x,y);
    // if(x>100 || x < 0){
    //     x = 0;
    // } else {
    //     // x = mapNumber(x, 8, 20, -20, 20);
    // }
    // if(y>100 || y < 0){
    //     y = 0;
    // } else {
    //     // y = mapNumber(y, 8, 20, -20, 20);
    // }

    // if (zeroCounter >= 3 && x != 0 || y != 0) {
    //     zeroCounter = 0; // intentional "click"
    //     // robot.mouseClick(); // rn clicks almost all the time :sob:
    // }

    // if (x === 0.0 && y === 0.0) {
    //     zeroCounter++;
    // } else {
    //     zeroCounter = 0;
    // }

    // moveAbsolute(x,y);
    moveMouse(x,y);
}
// deprecated because it wasn't useful
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
