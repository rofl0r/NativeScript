-- define various test functions
function bodyNative()
	updatePoints();
end

function bodyLoopToNative(pointCnt)
	for i=0,pointCnt-1,1 do
		getPoint(i);
		updateVelocity(point);
		stepPoint(point);
	end
end

function bodyAllScript(pointCnt)
	for i=0,pointCnt-1,1 do
		getPoint(i);

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
		friction = 0.2;
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
end

-- initialize game
initGame();
cyclesCount = getCyclesCount();
pointCount = getPointCount();

-- start game loop
i = 0;
step = 1;
if cyclesCount < 0 then
	step = 0;
	cyclesCount = 1;
end

while i < cyclesCount-1 do
i = i + step;

	-- process input and if the app should quit, break
	if processInput() then
		break;
	end

	--bodyNative();
	bodyLoopToNative(pointCount);
	--bodyAllScript(pointCount);

	draw();

end

-- clean resources
cleanup();

