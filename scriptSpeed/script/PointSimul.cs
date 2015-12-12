using System;
using System.Runtime.CompilerServices;

class PointSimul
{
    public static void allScript(double friction, Point point, Mouse mouse)
    {
        double diffX = mouse.getX() - point.getX();
        double diffY = mouse.getY() - point.getY();
        
        double distance = Math.Pow(Math.Pow(diffX, 2) + Math.Pow(diffY, 2), 0.5);
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

        double speed = Math.Pow(Math.Pow(point.getVelocityX(), 2) + Math.Pow(point.getVelocityY(), 2), 0.5);
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

    public static void allScriptMinCallback(double friction, Point point, Mouse mouse)
    {
        // some call saving precomputation
        double x = point.getX();
        double y = point.getY();
        double velocityX = point.getVelocityX();
        double velocityY = point.getVelocityY();
        double diffX = mouse.getX() - x;
        double diffY = mouse.getY() - y;

        // modify velocity based on point decision
        double distance = Math.Pow(Math.Pow(diffX, 2) + Math.Pow(diffY, 2), 0.5);
        if (distance > 20)
        {
            velocityX += diffX / distance;
            velocityY += diffY / distance;
        }
        else if (distance < 15)
        {
            velocityX -= diffX / distance;
            velocityY -= diffY / distance;
        }

        // let the physics happen
        double speed = Math.Pow(Math.Pow(velocityX, 2) + Math.Pow(velocityY, 2), 0.5);
        if (speed <= friction)
        {
            point.setVelocityX(0);
            point.setVelocityY(0);
        }
        else
        {
            // apply friction
            point.setVelocityX((1 - friction / speed) * velocityX);
            point.setVelocityY((1 - friction / speed) * velocityY);

            // move points
            point.setX(x + velocityX);
            point.setY(y + velocityY);
        }
    }

    public static void scriptToNative(double friction, Point point, Mouse mouse)
    {
        double diffX = mouse.getX() - point.getX();
        double diffY = mouse.getY() - point.getY();

        double distance = Math.Pow(Math.Pow(diffX, 2) + Math.Pow(diffY, 2), 0.5);
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

        stepPoint(point);
    }
    
    // Interfaces and native interop
    public static void Main(string[] args)
    {
    }

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void stepPoint(Point p);
}

// Point interface implemented on native side
class Point
{
    // internal identification of this object
    private int pointIndex;

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getX();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void setX(double x);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getY();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void setY(double y);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getVelocityX();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void setVelocityX(double velocityX);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getVelocityY();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void setVelocityY(double velocityY);
}

// Mouse interface implemented on native side
class Mouse
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getX();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern double getY();
}