function updateVelocity(point,mouse)
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
end

function stepPoint(point,friction)
	speed = (point:getVelocityX()^2 + point:getVelocityY()^2)^0.5;
	if speed <= friction then
		point:setVelocityX(0);
		point:setVelocityY(0);
	else
		-- apply friction
		point:setVelocityX((1 - friction / speed) * point:getVelocityX());
		point:setVelocityY((1 - friction / speed) * point:getVelocityY());

		-- move points
		point:setX(point:getX() + point:getVelocityX());
		point:setY(point:getY() + point:getVelocityY());
	end
end

function updatePoint(point,mouse,friction)
	updateVelocity(point,mouse);
	stepPoint(point,friction);
end

