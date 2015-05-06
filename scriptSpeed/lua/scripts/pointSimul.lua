---------------------------------------------
-- all computation for one point
--
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

---------------------------------------------
-- half of the computation for one point, rest delegated to native call
--
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

---------------------------------------------
-- all computation for one point with minimal number of native calls
--
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

---------------------------------------------
-- lua script controls the simulation loop
--
function loopControl(pointCount, cyclesCount, friction)
	-- initialize the simulation
	initSimul();

	-- start main loop
	for i=0,cyclesCount-1,1 do

		-- process input and if the app should quit, break
		if processInput() then
			break;
		end

		for j=0,pointCount-1,1 do
			allScript(getPoint(j), getMouse(), friction);
		end

		draw();

	end

	-- clean resources
	cleanup();
end


---------------------------------------------
-- lua script controls the simulation loop and caches all point objects
--
function loopPointsCached(pointCount, cyclesCount, friction)
	-- initialize the simulation
	initSimul();

	points = {}
    for i=0,pointCount-1 do
      points[i] = getPoint(i);
    end

	-- start main loop
	for i=0,cyclesCount-1,1 do

		-- process input and if the app should quit, break
		if processInput() then
			break;
		end

		mouse = getMouse();

		for j=0,pointCount-1,1 do
			allScript(points[j], mouse, friction);
		end

		draw();

	end

	-- clean resources
	cleanup();
end
