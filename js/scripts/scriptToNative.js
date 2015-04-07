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
stepPoint(point);
