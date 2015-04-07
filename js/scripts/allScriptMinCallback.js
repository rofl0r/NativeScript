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
if (distance > 20 ) {
    velocityX += diffX / distance;
    velocityY += diffY / distance;
} else if (distance < 15) {
    velocityX -= diffX / distance;
    velocityY -= diffY / distance;
}

// let the physics happen
var friction = 0.2;
var speed = Math.pow(Math.pow(velocityX,2) + Math.pow(velocityY,2), 0.5);
if (speed <= friction) {
    point.velocityX = 0;
    point.velocityY = 0;
} else {
    // apply friction
    point.velocityX = ((1 - friction / speed) * velocityX);
    point.velocityY = ((1 - friction / speed) * velocityY);

    // move points
    point.x = x + velocityX;
    point.y = y + velocityY;
}