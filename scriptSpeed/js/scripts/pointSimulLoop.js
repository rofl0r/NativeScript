function loopCallback(pointCount, cyclesCount, friction) {
	initSimul();
	points = getAllPoints(pointCount);
    for (var i = 0; i < cyclesCount; i++) {
        if (processInput()) break;
        mouse = getMouse();
        preDraw();
        drawMouse();
        for (var j = 0; i < pointCount; j++) {

            updateVelocity(points[j]);
            stepPoint(points[j]);
            drawPoint(points[j]);

        }
        postDraw();
    }
    cleanup();
}

function loopSetGetMax(pointCount, cyclesCount, friction) {
    initSimul();
    points = getAllPoints(pointCount);
    for (var i = 0; i < cyclesCount; i++) {
        if (processInput()) break;
        mouse = getMouse();
        for (var j = 0; i < pointCount; j++) {

            allScript(points[j], mouse, friction);

        }
        draw();
    }
    cleanup();
}

function loopSetGetMin(pointCount, cyclesCount, friction) {
    initSimul();
    points = getAllPoints(pointCount);
    for (var i = 0; i < cyclesCount; i++) {
        if (processInput()) break;
        mouse = getMouse();
        for (var j = 0; i < pointCount; j++) {

            allScriptMinCallback(points[j], mouse, friction);

        }
        draw();
    }
    cleanup();
}

function getAllPoints(pointCount) {
    var points = [];
    for (var i = 0; i < pointCount; i++) {
        points[i] = getPoint(i);
    }
    return points;
}

// 20 set/get callbacks
// 4 var declarations
function allScript(point, mouse, friction) {
	// some call saving precomputation
    var diffX = mouse.x - point.x;
    var diffY = mouse.y - point.y;

	    // modify velocity based on point decision
    var distance = Math.pow(Math.pow(diffX, 2) + Math.pow(diffY, 2), 0.5);
    if (distance > 20) {
        point.velocityX += diffX / distance;
        point.velocityY += diffY / distance;
    } else if (distance < 15) {
        point.velocityX -= diffX / distance;
        point.velocityY -= diffY / distance;
    }

	// let the physics happen
    var speed = Math.pow(Math.pow(point.velocityX, 2) + Math.pow(point.velocityY, 2), 0.5);
    if (speed <= friction) {
        point.velocityX = 0;
        point.velocityY = 0;
    } else {
	    // apply friction
        point.velocityX = (point.velocityX - point.velocityX * friction / speed);
        point.velocityY = (point.velocityY - point.velocityY * friction / speed);

	    // move points
        point.x += point.velocityX;
        point.y += point.velocityY;
    }
}

// 10 set/get callbacks
// 10 var declarations
function allScriptMinCallback(point, mouse, friction) {
	    // some call saving precomputation
    var x = point.x;
    var y = point.y;
    var velocityX = point.velocityX;
    var velocityY = point.velocityY;
    var mouseX = mouse.x;
    var mouseY = mouse.y;
    var diffX = mouse.x - point.x;
    var diffY = mouse.y - point.y;

	// modify velocity based on point decision
    var distance = Math.pow(Math.pow(diffX, 2) + Math.pow(diffY, 2), 0.5);
    if (distance > 20) {
        velocityX += diffX / distance;
        velocityY += diffY / distance;
    } else if (distance < 15) {
        velocityX -= diffX / distance;
        velocityY -= diffY / distance;
    }

	// let the physics happen
    var speed = Math.pow(Math.pow(velocityX, 2) + Math.pow(velocityY, 2), 0.5);
    if (speed <= friction) {
        point.velocityX = 0;
        point.velocityY = 0;
    } else {
	    // apply friction
        point.velocityX = (velocityX - velocityX * friction / speed);
        point.velocityY = (velocityY - velocityY * friction / speed);

	    // move points
        point.x = x + velocityX;
        point.y = y + velocityY;
    }
}

