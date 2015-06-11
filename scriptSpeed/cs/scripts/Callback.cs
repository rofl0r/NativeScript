using System;
using System.Runtime.CompilerServices;

class Callback
{
    public static void Main(string[] args)
    {
    }

    public static void simpleCallback(double count)
    {
        for (double i = 0; i < count; i++)
        {
            countFnc();
        }
    }

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void countFnc();

}