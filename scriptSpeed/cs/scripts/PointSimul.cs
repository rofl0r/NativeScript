using System;
using System.Runtime.CompilerServices;

class PointSimul
{
    public static void allScript(Point point, Mouse mouse, double friction)
    {
	    double diffX = mouse.getX() - point.getX();
	    double diffY = mouse.getY() - point.getY();

	    double distance = Math.pow(Math.pow(diffX, 2) + Math.pow(diffY, 2), 0.5);
        if (distance > 20)
        {
            point.setVelocityX(point.getVelocityX() + diffX / distance);
            point.setVelocityY(point.getVelocityY() + diffY / distance);
        }
        else if (distance < 15)
        {
            point.setVelocityX(point.getVelocityX() - diffX / distance);
            point.setVelocityY(point.getVelocityY() - diffY / distance);
        }
	    
	    double speed = Math.pow(Math.pow(point.getVelocityX(), 2) + Math.pow(point.getVelocityY(), 2), 0.5);
        if (speed <= friction)
        {
            point.setVelocityX(0);
            point.setVelocityY(0);
        }
        else
        {
    		point.setVelocityX((1 - friction / speed) * point.getVelocityX());
		    point.setVelocityY((1 - friction / speed) * point.getVelocityY());

		    point.setX(point.getX() + point.getVelocityX());
		    point.setY(point.getY() + point.getVelocityY());
        }
    }


    // Interfaces and native interop
    public static void Main(string[] args) { }

    // Point interface implemented on native side
    class Point
    {
        // internal identification of this object
        private long user_data;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getX();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public void setX(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getY();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public void setY(double y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getVelocityX();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public void setVelocityX(double velocityX);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getVelocityY();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public void setVelocityY(double velocityY);
    }

    // Mouse interface implemented on native side
    class Mouse
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getX();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public double getY();
    }

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static Point getPoint(int i);
}
