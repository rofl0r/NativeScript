-- some call saving precomputation
diffX = mouse:getX() - point:getX();
diffY = mouse:getY() - point:getY();

-- modify velocity based on point decision
distance = (diffX^2 + diffY^2)^0.5;
if distance > 20 then
	point:setVelocityX(point:getVelocityX() + (diffX) / distance);
	point:setVelocityY(point:getVelocityY() + (diffY) / distance);
elseif distance < 15 then
	point:setVelocityX(point:getVelocityX() - (diffX) / distance);
	point:setVelocityY(point:getVelocityY() - (diffY) / distance);
end

-- let the physics happen
stepPoint(point);