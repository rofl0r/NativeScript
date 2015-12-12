using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class PointSimulStruct
{
    public static void allScript(double friction, ref PointStruct point, MouseStruct mouse)
    {
        double diffX = mouse.x - point.x;
        double diffY = mouse.y - point.y;

        double distance = Math.Pow(Math.Pow(diffX, 2) + Math.Pow(diffY, 2), 0.5);
        if (distance > 20)
        {
            point.velocityX += diffX / distance;
            point.velocityY += diffY / distance;
        }
        else if (distance < 15)
        {
            point.velocityX -= diffX / distance;
            point.velocityY -= diffY / distance;
        }

        double speed = Math.Pow(Math.Pow(point.velocityX, 2) + Math.Pow(point.velocityY, 2), 0.5);
        if (speed <= friction)
        {
            point.velocityX = 0;
            point.velocityY = 0;
        }
        else
        {
            point.velocityX *= 1 - friction / speed;
            point.velocityY *= 1 - friction / speed;

            point.x += point.velocityX;
            point.y += point.velocityY;
        }
    }

    public static void allScriptMinCallback(double friction, ref PointStruct point, MouseStruct mouse)
    {
        // some call saving precomputation
        double x = point.x;
        double y = point.y;
        double velocityX = point.velocityX;
        double velocityY = point.velocityY;
        double diffX = mouse.x - x;
        double diffY = mouse.y - y;

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
            point.velocityX = 0;
            point.velocityY = 0;
        }
        else
        {
            // apply friction
            point.velocityX = ((1 - friction / speed) * velocityX);
            point.velocityY = ((1 - friction / speed) * velocityY);

            // move points
            point.x = x + velocityX;
            point.y = y + velocityY;
        }
    }

    public static void scriptToNative(double friction, ref PointStruct point, MouseStruct mouse)
    {
        double diffX = mouse.x - point.x;
        double diffY = mouse.y - point.y;

        double distance = Math.Pow(Math.Pow(diffX, 2) + Math.Pow(diffY, 2), 0.5);
        if (distance > 20)
        {
            point.velocityX += diffX / distance;
            point.velocityY += diffY / distance;
        }
        else if (distance < 15)
        {
            point.velocityX -= diffX / distance;
            point.velocityY -= diffY / distance;
        }

        stepPoint(ref point);
    }

    // Interfaces and native interop
    public static void Main(string[] args)
    {
    }

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void stepPoint(ref PointStruct p);
}

[StructLayout(LayoutKind.Sequential)]
struct PointStruct
{
    public double x;
    public double y;
    public double velocityX;
    public double velocityY;
}

[StructLayout(LayoutKind.Sequential)]
struct MouseStruct
{
    public double x;
    public double y;
}
