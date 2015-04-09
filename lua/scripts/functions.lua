function allScript(point,mouse,friction)
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

function scriptToNative(point,mouse,friction)
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

	stepPoint(point);
end

function allScriptMinCallback(point,mouse,friction)
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
end

