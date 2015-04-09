function updateVelocity(point, mouse) {
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
}

function stepPoint(point, friction) {
    // let the physics happen
    var speed = Math.pow(Math.pow(point.velocityX, 2) + Math.pow(point.velocityY, 2), 0.5);
    if (speed <= friction) {
        point.velocityX = 0;
        point.velocityY = 0;
    } else {
        // apply friction
        point.velocityX = ((1 - friction / speed) * point.velocityX);
        point.velocityY = ((1 - friction / speed) * point.velocityY);

        // move points
        point.x += point.velocityX;
        point.y += point.velocityY;
    }
}

function updatePoint(point, mouse, friction) {
    updateVelocity(point, mouse);
    stepPoint(point, friction);
}
