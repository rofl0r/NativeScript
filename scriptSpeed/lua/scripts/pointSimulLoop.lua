function simpleCallback(pointCount, cyclesCount, friction)
	for j=0,cyclesCount-1,1 do
		count();
	end
end

function loopCallback(pointCount, cyclesCount, friction)
	initSimul();
	points = getAllPoints(pointCount);
	for i=0,cyclesCount-1,1 do
		if processInput() then break; end
		mouse = getMouse();
		preDraw();
		drawMouse();
		for j=0,pointCount-1,1 do

			updateVelocity(points[j]);
			stepPoint(points[j]);
			drawPoint(points[j]);

		end
		postDraw();
	end
	cleanup();
end

function loopSetGetMax(pointCount, cyclesCount, friction)
	initSimul();
	points = getAllPoints(pointCount);
	for i=0,cyclesCount-1,1 do
		if processInput() then break; end
		mouse = getMouse();
		for j=0,pointCount-1,1 do

			allScript(points[j], mouse, friction);

		end
		draw();
	end
	cleanup();
end

function loopSetGetMin(pointCount, cyclesCount, friction)
	initSimul();
	points = getAllPoints(pointCount);
	for i=0,cyclesCount-1,1 do
		if processInput() then break; end
		mouse = getMouse();
		for j=0,pointCount-1,1 do

			allScriptMinCallback(points[j], mouse, friction);

		end
		draw();
	end
	cleanup();
end

function getAllPoints(pointCount)
	points = {}
    for i=0,pointCount-1 do
      points[i] = getPoint(i);
    end
    return points;
end

-- 20 set/get callbacks
-- 4 var declarations
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
		point:setVelocityX(point:getVelocityX() - point:getVelocityX() * friction / speed);
		point:setVelocityY(point:getVelocityY() - point:getVelocityY() * friction / speed);

		-- move points
		point:setX(point:getX() + point:getVelocityX());
		point:setY(point:getY() + point:getVelocityY());
	end
end


-- 10 set/get callbacks
-- 10 var declarations
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
		point:setVelocityX(xVelocity - xVelocity * friction / speed);
		point:setVelocityY(yVelocity - yVelocity * friction / speed);

		-- move points
		point:setX(x + xVelocity);
		point:setY(y + yVelocity);
	end
end
