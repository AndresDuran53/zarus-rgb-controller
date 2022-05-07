var rgbToHex = function (rgb) {
  var hex = Number(rgb).toString(16);
  if (hex.length < 2) {
       hex = "0" + hex;
  }
  return hex;
};

var fullColorHex = function(r,g,b) {
  var red = rgbToHex(r);
  var green = rgbToHex(g);
  var blue = rgbToHex(b);
  return "#"+red+green+blue;
};

/**
 * degreesToRadians
 *
 * @param {number} degrees
 * @returns {number}  radians
 */
function degreesToRadians(degrees) {
    return degrees * (Math.PI / 180);
}

function sendColor(values){
  $.post("/setValues",
  values,
  function(data, status){
    //alert("Data: " + data + "\nStatus: " + status);
  });
}

function generateColorWheel() {
    //Generate main canvas to return

    //Initiate variables
    var size = document.getElementById('colorDiv').clientWidth * 0.7 ;
    var halfSize = size/2;
    var angle = 0;
    var hexCode = [255, 0, 0];
    var pivotPointer = 0;
    var colorOffsetByDegree = 4.322;

    var canvas = document.createElement("canvas");
    var canvasClone = document.createElement("canvas");

    canvas.id = "colorPickerId";
    canvas.setAttribute("style","touch-action: none;");
    var ctx = canvas.getContext("2d");
    canvas.width = canvas.height = size;
    //Generate canvas clone to draw increments on

    canvasClone.width = canvasClone.height = size;
    var canvasCloneCtx = canvasClone.getContext("2d");

    //For each degree in circle, perform operation
    while (angle++ < 360) {
        //find index immediately before and after our pivot
        var pivotPointerbefore = (pivotPointer + 3 - 1) % 3;
        var pivotPointerAfter = (pivotPointer + 3 + 1) % 3;
        //Angle variables
        var degreePlus = degreesToRadians(angle + 1);
        var degreeMinus = degreesToRadians(angle - 1);
        //Modify colors
        if (hexCode[pivotPointer] < 255) {
            //If main points isn't full, add to main pointer
            hexCode[pivotPointer] = (hexCode[pivotPointer] + colorOffsetByDegree > 255 ? 255 : hexCode[pivotPointer] + colorOffsetByDegree);
        }
        else if (hexCode[pivotPointerbefore] > 0) {
            //If color before main isn't zero, subtract
            hexCode[pivotPointerbefore] = (hexCode[pivotPointerbefore] > colorOffsetByDegree ? hexCode[pivotPointerbefore] - colorOffsetByDegree : 0);
        }
        else if (hexCode[pivotPointer] >= 255) {
            //If main color is full, move pivot
            hexCode[pivotPointer] = 255;
            pivotPointer = (pivotPointer + 1) % 3;
        }
        //clear clone
        canvasCloneCtx.clearRect(0, 0, size, size);
        //Generate gradient and set as fillstyle
        var grad = canvasCloneCtx.createRadialGradient(halfSize, halfSize, 0, halfSize, halfSize, halfSize);
        grad.addColorStop(0, "white");
        grad.addColorStop(1, "rgb(" + hexCode.map(function (h) { return Math.floor(h); }).join(",") + ")");
        canvasCloneCtx.fillStyle = grad;
        //draw full circle with new gradient
        canvasCloneCtx.globalCompositeOperation = "source-over";
        canvasCloneCtx.beginPath();
        canvasCloneCtx.arc(halfSize, halfSize, halfSize, 0, Math.PI * 2);
        canvasCloneCtx.closePath();
        canvasCloneCtx.fill();
        //Switch to "Erase mode"
        canvasCloneCtx.globalCompositeOperation = "destination-out";
        //Carve out the piece of the circle we need for this angle
        canvasCloneCtx.beginPath();
        canvasCloneCtx.arc(halfSize, halfSize, 0, degreePlus, degreePlus);
        canvasCloneCtx.arc(halfSize, halfSize, halfSize + 1, degreePlus, degreePlus);
        canvasCloneCtx.arc(halfSize, halfSize, halfSize + 1, degreePlus, degreeMinus);
        canvasCloneCtx.arc(halfSize, halfSize, 0, degreePlus, degreeMinus);
        canvasCloneCtx.closePath();
        canvasCloneCtx.fill();
        //Draw carved-put piece on main canvas
        ctx.drawImage(canvasClone, 0, 0);
    }
    //ctx.drawImage(canvasClone, 0, 0);
    //return main canvas
    return canvas;
}

function createWheel() {
  //MoveSlider
  var size = document.getElementById('colorDiv').clientWidth * 0.7 ;
  var halfSize = size/2;
  var colorShowDiv = document.getElementById("colorShowDiv");
  colorShowDiv.setAttribute("style","top: "+halfSize+"px;");

  //Get color wheel canvas
  var colorWheel = generateColorWheel();

  //Add color wheel canvas to document
  document.getElementById('colorDiv').appendChild(colorWheel);

  //Add ouput field
  var p = document.getElementById("colorPickerTextId");

  var lastCoord = {
    offsetX:0,
    offsetY:0,
  };

  var initValue = {
    red:255,
    green:255,
    blue:255,
    hex:fullColorHex(255, 255, 255),
    isValid:true
  };

  function changeColorViewer(imgData){
    p.innerHTML = "RGB: " + imgData.red + "," + imgData.green + "," + imgData.blue;
    var canvasShowColor = document.getElementById("canvasShowColorId");
    var ctx = canvasShowColor.getContext("2d");
    ctx.fillStyle = imgData.hex;
    ctx.fillRect(0, 0, canvasShowColor.width, canvasShowColor.height);
    var colorBrightnessRange = document.getElementById("colorBrightnessRangeId");
    colorBrightnessRange.style.backgroundImage = 'linear-gradient(to right, #0F0F0D, rgb(' + imgData.red + ',' + imgData.green + ',' + imgData.blue + '))';
    colorBrightnessRange.background = 'rgb(0,0,0)';
  }

  changeColorViewer(initValue);

  function getColorFromEvent(offsetX,offsetY) {
    var ctx = colorWheel.getContext("2d");
    var imgData = ctx.getImageData(offsetX, offsetY, 1, 1);
    var isValid = true;
    var red = imgData.data[0];
    var green = imgData.data[1];
    var blue = imgData.data[2];
    if(red==0 && green==0 && blue==0){
      isValid=false
    }
    var rgb = {
      red:red,
      green:green,
      blue:blue,
      hex:fullColorHex(red, green, blue),
      isValid:isValid
    };
    return rgb;
  }

  function colorWheelMouse(evt) {
    var imgData = getColorFromEvent(evt.offsetX,evt.offsetY)
    if(imgData.isValid){
      lastCoord = {
        offsetX: evt.offsetX,
        offsetY: evt.offsetY
      };
      changeColorViewer(imgData);
    }
  };

  function colorWheelClick(evt) {
    var imgData = getColorFromEvent(evt.offsetX,evt.offsetY)
    if(imgData.isValid){
      var colorValues = {
        rgbh: imgData.hex
      }
      sendColor(colorValues);
    }
  };

  function colorWheelTouch(evt) {
    evt.preventDefault();
    var rect = evt.target.getBoundingClientRect();
    touchCoord = {
      offsetX: evt.targetTouches[0].clientX - rect.x,
      offsetY: evt.targetTouches[0].clientY - rect.y
    };
    var imgData = getColorFromEvent(touchCoord.offsetX,touchCoord.offsetY)
    if(imgData.isValid){
      lastCoord = {
        offsetX: evt.targetTouches[0].clientX - rect.x,
        offsetY: evt.targetTouches[0].clientY - rect.y
      };
      changeColorViewer(imgData);
    }
  };

  function colorWheelTouchStart(evt) {
    evt.preventDefault();
    var rect = evt.target.getBoundingClientRect();
    touchCoord = {
      offsetX: evt.targetTouches[0].clientX - rect.x,
      offsetY: evt.targetTouches[0].clientY - rect.y
    };
    var imgData = getColorFromEvent(touchCoord.offsetX,touchCoord.offsetY)
    if(imgData.isValid){
      lastCoord = {
        offsetX: evt.targetTouches[0].clientX - rect.x,
        offsetY: evt.targetTouches[0].clientY - rect.y
      };
    }
  };

  function colorWheelTouchEnd(evt) {
    evt.preventDefault();
    var rect = evt.target.getBoundingClientRect();
    var imgData = getColorFromEvent(lastCoord.offsetX,lastCoord.offsetY)
    if(imgData.isValid){
      changeColorViewer(imgData);
      var colorValues = {
        rgbh: imgData.hex
      }
      sendColor(colorValues);
    }
  };

  //Bind mouse event
  colorWheel.onmousemove = colorWheelMouse;
  colorWheel.onclick = colorWheelClick;
  colorWheel.ontouchmove = colorWheelTouch;
  colorWheel.ontouchend = colorWheelTouchEnd;
  colorWheel.ontouchstart = colorWheelTouchStart;
  colorWheel.addEventListener("touchcancel", function(evt){evt.preventDefault()});
  fadeOutInEffect("loadingSreenId","mainDivId");
}

// Update the current slider value (each time you drag the slider handle)
document.getElementById("colorBrightnessRangeId").onchange = function() {
  var brightnessValue = {
    lhbr: document.getElementById("colorBrightnessRangeId").value
  }
  sendColor(brightnessValue);
}
