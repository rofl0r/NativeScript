-- Point definition in Lua
PointLua = {}
PointLua.__index = PointLua
PointLua.__class = 'PointLua'

function PointLua:new(x, y)
	obj = {x = x, y = y, velocityX = 0, velocityY = 0}
	setmetatable(obj, self) -- This makes obj inherit from HUD
	return obj
end

function PointLua:setX(x)
	self.x = x;
end

function PointLua:getX()
	return self.x;
end

function PointLua:setY(y)
	self.y = y;
end

function PointLua:getY()
	return self.y;
end

function PointLua:setVelocityX(velocityX)
	self.velocityX = velocityX;
end

function PointLua:getVelocityX()
	return self.velocityX;
end

function PointLua:setVelocityY(velocityY)
	self.velocityY = velocityY;
end

function PointLua:getVelocityY()
	return self.velocityY;
end

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
-- lua script controls the simulation loop and uses its own point objects
--
function loopPointsScripted(pointCount, cyclesCount, friction)
	-- initialize the simulation
	initSimul();

	points = {}
    for i=0,pointCount-1 do
      points[i] = PointLua:new(i,i);
    end

	-- start main loop
	for i=0,cyclesCount-1,1 do

		-- process input and if the app should quit, break
		if processInput() then
			break;
		end

		mouse = getMouse();
		preDraw();
		drawMouse();

		for j=0,pointCount-1,1 do
			allScript(points[j], mouse, friction);
			drawPoint(points[j]:getX(), points[j]:getY());
		end

		postDraw();

	end

	-- clean resources
	cleanup();
end
