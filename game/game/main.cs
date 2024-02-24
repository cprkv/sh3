using System;
using game;

Console.WriteLine("Hello, World!");

var status = Native.Core.coreInitialize();
if (status != Native.Core.Status.Ok)
{
  Console.WriteLine($"core initialization error: {Native.Core.coreGetErrorDetails()}");
  return;
}

Native.Core.coreDestroy();
