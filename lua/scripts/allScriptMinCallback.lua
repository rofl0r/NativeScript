-- some call saving precomputation
x = point:getX();
y = point:getY();
xVelocity = point:getVelocityX();
yVelocity = point:getVelocityY();
mouseX = mouse:getX();
mouseY = mouse:getY();
diffX = mouseX - x;
diffY = mouseY - y;

-- modify velocity based on point decision
distance = (diffX^2 + diffY^2)^0.5;
if distance > 20 then
	xVelocity = xVelocity + diffX / distance;
	yVelocity = yVelocity + diffY / distance;
elseif distance < 15 then
	xVelocity = xVelocity - diffX / distance;
	yVelocity = yVelocity - diffY / distance;
end

-- let the physics happen
friction = 0.2;
speed = (xVelocity^2 + yVelocity^2)^0.5;
if speed <= friction then
	point:setVelocityX(0);
	point:setVelocityY(0);
else
	-- apply friction
	point:setVelocityX((1 - friction / speed) * xVelocity);
	point:setVelocityY((1 - friction / speed) * yVelocity);

	-- move points
	point:setX(x + xVelocity);
	point:setY(y + yVelocity);
end