var CTX;

var CONFIG = {
    "animation_timing_ms": 100,
    "none_color": "#333333",
    "grid_height": 12,
    "grid_length": 30,
    "led_size": 20,
    "led_spacing": 17
}

var animation; 
var playing = false;
var insertAtTheEnd = false;
var colorPicking = false;

/* pencil, eraser */
var mode = "pencil"

/* This funciton is stolen from: https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
Written by Tim Down */
function hexToRgb(hex) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}

function writeUint8(value, array) 
{
        let steps = new Uint8Array(1);
        steps[0] = value;
        array.push(steps);
}

function writeUint16(value, array) 
{
        let steps = new Uint16Array(1);
        steps[0] = value;
        array.push(steps);
}

class Animation 
{
    constructor(leds) 
    {
        this.leds = leds;
        this.stepCount = 0;
        this.currentIndex = 0;
        this.playing = false;
        this.use_color = "#ffffff"
        this.colors = [{}]
    }

    clickLed(x, y) 
    {
        for (let i = 0; i < CONFIG["grid_height"] * CONFIG["grid_length"]; i++) {
            if (this.leds[i].checkCollision(x, y)) {
                if (colorPicking) {
                    let color = this.leds[i].getColor()
                    if (color != CONFIG["none_color"]) {
                        this.setColor(color.substr(1));
                        document.getElementById("colorpicker").jscolor.fromString(color);
                        console.log("setting color", color)
                    }
                }
                else if (mode == "pencil") {
                        this.leds[i].updateColor(this.use_color);
                        this.colors[this.currentIndex][this.use_color] = 1;
                }

                else if (mode == "eraser")
                        this.leds[i].updateColor(CONFIG["none_color"]);

                return true;
            }
        }
        return false;
    }

    getLedsWithColor(index, color) 
    {
        var led_indexes = []
        this.leds.forEach(led => {
            if (led.hasColorInState(index, color)) 
            {
                led_indexes.push(led.getIndex());
            }
        });
        return led_indexes;
    }

    /* Data format:
    uint8: total animation steps
    ---
    uin16: numbers of colors in this step,
    ----
    uint8: r,
    uint8: g,
    uint8: b,
    uint16: number of leds in this step
    uint16 led_indexes[] 
    ---
    uint16: number of colors in this step
    -----
    uint8 r,
    uint8 g.... and so on
    */
    
    export() {
        let data = [];
        let totalAnimationSteps = this.stepCount + 1;

        writeUint8(totalAnimationSteps, data);
        
        for (var i = 0; i < totalAnimationSteps; i++) 
        {

            let totalColorsInCurrentStep = Object.keys(this.colors[i]).length;

            if (totalColorsInCurrentStep === 0 && totalAnimationSteps === 0)
            {
                console.log("Nothing to export.")
                return;
            }

            writeUint16(totalColorsInCurrentStep, data);

            for (const color of Object.keys(this.colors[i])) 
            {
                let rgb = hexToRgb(color)
                writeUint8(rgb.r, data)
                writeUint8(rgb.g, data)
                writeUint8(rgb.b, data)
                let ledsWithThisColor = this.getLedsWithColor(i, color);
                writeUint16(ledsWithThisColor.length, data)
                ledsWithThisColor.forEach(elem => {
                    writeUint16(elem, data);
                })
            }


        }

        var blob = new Blob(data, { type: "application/octet-stream" });
        var blobUrl = URL.createObjectURL(blob);
        window.location.replace(blobUrl);

        var fileLink = document.createElement('a');
        fileLink.href = blobUrl
        fileLink.download = "animation.bin"
        fileLink.click();

    }

    update() 
    {
        this.updateLedState();
        this.draw();
    }
    
    insertStep() 
    {
        this.leds.forEach(led => {
            led.insert();
        });
    }
    newStep(copy = false) 
    {
        this.stepCount++;
        var previousIndex = this.currentIndex;
        if (insertAtTheEnd) {
            this.currentIndex = this.stepCount;
            this.colors.push({});
        } else {
            this.currentIndex++;
            this.colors.splice(this.currentIndex, 0, {});
            this.insertStep();
        }

        this.update();

        if (copy && this.stepCount > 0) 
        {
            for (const color of Object.keys(this.colors[previousIndex])) 
            {
                this.colors[this.currentIndex][color] = 1;
            }

            this.leds.forEach(led => {
                led.copyIndex(previousIndex);
            });
        }
        this.update();
    }

    stepForward() 
    {
        if (this.currentIndex < this.stepCount) {
            this.currentIndex++;
        }
        else {
            this.currentIndex = 0;
        }
        this.update();
    }
    
    clearAll() 
    {
        this.leds.forEach(led => {
            led.updateColor(CONFIG["none_color"]);
        });
        this.update();
    }
    stepBackward() 
    {
        this.currentIndex--;
        if (this.currentIndex < 0)
            this.currentIndex = 0;
        this.update();
    }

    updateLedState() 
    {
        this.leds.forEach(led => {
            led.updateState(this.currentIndex)
        });
    }

    animationStep() 
    {

        if (this.stepCount == 0) 
        {
            this.stop();
            return;
        }
        this.currentIndex++;
        this.currentIndex = (this.currentIndex % (this.stepCount + 1))
        this.update();
    }
    
    setColor(color)
    {
        this.use_color = "#" + color;
    }

    play() 
    {
        if (!playing) {
            playing = true;
            this.playingInterval = setInterval(() => {this.animationStep()}, CONFIG["animation_timing_ms"]);
        }
    }
     
    stop() 
    {
        playing = false;
        clearTimeout(this.playingInterval);
        this.update();
    }
    
    deleteStep() 
    {
     if (this.stepCount > 0)   {
         this.leds.forEach(led => {
            led.removeStep(this.currentIndex);
            led.updateState(this.currentIndex - 1);
            
         });
         this.colors.splice(this.currentIndex, 1);
         if (this.currentIndex != 0)
            this.currentIndex--;
         this.stepCount--;
         this.update();
     }
    }
    
    draw() 
    {
        clearCanvas()
        updateCurrentStep()
        updateTotalSteps()
        this.leds.forEach(led => {
           led.draw(); 
        });
    }
}



class Led 
{
    constructor(x, y) 
    {
        this.x = x;
        this.y = y;
        this.colorState = [CONFIG["none_color"]]
        this.currentState = 0;
        this.size = CONFIG["led_size"]
        this.spacing = CONFIG["led_spacing"]
        this.posX = this.x * this.size + this.x * this.spacing;
        this.posY = this.y * this.size + this.y * this.spacing;

    }

    removeStep(idx) {
        this.colorState.splice(idx, 1)
    }
    hasColorInState(state, color) {
        return this.colorState[state] == color;
    }

    getColor() 
    {
        return this.colorState[this.currentState];
    }

    getIndex()
    {
        return this.y * CONFIG["grid_length"] + this.x;
    }

    insert() {
        this.colorState.splice(this.currentState + 1, 0,  CONFIG["none_color"]);
    }

    copyIndex(idx) 
    {
        this.colorState[this.currentState] = this.colorState[idx];
    }

    checkCollision(x, y) 
    {
        return (x >= this.posX && x <= this.posX + this.size && y >= this.posY && y <= this.posY + this.size)
    }

    updateColor(color) 
    {
        this.colorState[this.currentState] = color;
    }

    updateState(step) 
    {
        this.currentState = step;
        if (step == this.colorState.length) {
            this.colorState.push(CONFIG["none_color"])
        }
    }

    draw() 
    {
        if (this.colorState[this.currentState - 1] != CONFIG["none_color"] && this.colorState[this.currentState] == CONFIG["none_color"] && this.currentState != 0 && !playing) {
            CTX.fillStyle = this.colorState[this.currentState - 1];
            CTX.globalAlpha = 0.3;
            CTX.shadowBlur = 0;
        }
        else {

            CTX.fillStyle = this.colorState[this.currentState];
            CTX.shadowColor = this.colorState[this.currentState];
            CTX.globalAlpha = 1;
            CTX.shadowBlur = 20;
        }

        CTX.beginPath();
        CTX.fillRect(this.x * this.size + this.x * this.spacing, this.y * this.size + this.y * this.spacing, this.size, this.size);
        CTX.stroke();
    }

    drawShadow() {
        CTX.beginPath();
        CTX.fillStyle = this.colorState[this.currentState];
        CTX.shadowBlur = 20;
        CTX.shadowColor = this.colorState[this.currentState];
        CTX.fillRect(this.x * this.size + this.x * this.spacing, this.y * this.size + this.y * this.spacing, this.size, this.size);
        CTX.stroke();
    }
}


function clearCanvas() 
{
    CTX.clearRect(0, 0, canvas.width, canvas.height);
}

function updateCurrentStep() 
{
    $(".totalSteps").each(function() {
        this.innerHTML = animation.stepCount + 1;
    })
}

function updateTotalSteps() 
{
    $(".currentStep").each(function() {
        this.innerHTML = animation.currentIndex + 1;
    })
}
function startDraw()
{
    var mouse_down = false;
    var c = document.getElementById("canvas");
    CTX = c.getContext("2d");
    var height = 12;
    var leds = []

    for (i = 0; i < CONFIG["grid_height"]; i++) 
    {
        for (j = 0; j < CONFIG["grid_length"]; j++) 
        {
            leds.push(new Led(j, i))
        }
    }

    animation = new Animation(leds);
    animation.draw();


    c.onclick = function(e) 
    { 
        if (animation.clickLed(e.offsetX, e.offsetY) ) 
        {
            animation.draw();
        }
    }

    c.onmousedown = function(e)
    {
        mouse_down = true;
    }

    c.onmousemove = function(e)
    {
        if (mouse_down) 
        {
            if (animation.clickLed(e.offsetX, e.offsetY) ) 
            {
                clearCanvas()
                animation.draw();
            }
        }
    }
    c.onmouseup = function(e) 
    {
        mouse_down = false;
    }

}


function stepForward() 
{
    animation.stepForward();
}

function stepBackward() 
{
    animation.stepBackward();
}

function play() 
{
    animation.play();
}

function stop()
{
    animation.stop();
}

function setColor(color) 
{
    animation.setColor(color);
}

function setMode(m) 
{
    colorPicking = false;
    mode = m
    if (mode == "pencil") {
        $('#canvas').css({'cursor': "url('pencilsmall.png') -10 40, pointer"});
    }
    else if (mode == "eraser") {
        $('#canvas').css({'cursor': "url('erasersmall.png') -10 40, pointer"});

    }
}

function clearAll() {
    animation.clearAll()
}

function exportFormat() {
    animation.export();
}

function removeStep() {
    animation.deleteStep();
}

function newStep() {
    animation.newStep();
}

function copyStep() {
    animation.newStep(true)
}

function insertAfter(v){
    insertAtTheEnd = v;
}

function colorPick(v) {
    colorPicking = v;
    if (colorPicking) {
        $('#canvas').css({'cursor': "url('eyedropper.png') -10 40, pointer"});
    }
    else {
        setMode(mode);
    }
}