using System;
using System.IO;
using System.Runtime.InteropServices;

namespace game;

internal partial class Native
{
  public static partial class WinAPI
  {
    private const string libraryName = "kernel32.dll";

    [LibraryImport(libraryName, SetLastError = true, StringMarshalling = StringMarshalling.Utf16)]
    public static partial IntPtr LoadLibraryW(string fileName);

    [LibraryImport(libraryName, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static partial bool FreeLibrary(IntPtr hModule);
  }

  public static partial class Core
  {
    private const string libraryName = "core.dll";

    [LibraryImport(libraryName)]
    public static partial Status coreInitialize();

    [LibraryImport(libraryName)]
    public static partial void coreDestroy();

    [LibraryImport(libraryName, StringMarshalling = StringMarshalling.Utf8)]
    public static partial string coreGetErrorDetails();

    public enum Status
    {
      Ok,
      SystemError,
    }
  }
}

public class NativeLibraryWrapper : IDisposable
{
  protected IntPtr handle = IntPtr.Zero;

  public static NativeLibraryWrapper Load(string path)
  {
    var handle = LoadDLL(path);
    return new NativeLibraryWrapper { handle = handle };
  }

  public void Dispose()
  {
    Native.WinAPI.FreeLibrary(handle);
  }

  private static IntPtr LoadDLL(string dllName)
  {
    if (!File.Exists(dllName))
      throw new FileNotFoundException(dllName);

    var handle = Native.WinAPI.LoadLibraryW(dllName);
    if (handle == IntPtr.Zero)
    {
      int hr = Marshal.GetHRForLastWin32Error();
      Marshal.ThrowExceptionForHR(hr);
    }

    return handle;
  }
}


public class Core : IDisposable
{
  private readonly NativeLibraryWrapper library;

  private Core(NativeLibraryWrapper library)
  {
    this.library = library;
  }

  public static Core Load()
  {
    var library = NativeLibraryWrapper.Load("core.dll");
    return new Core(library);
  }

  public void Dispose()
  {
    library.Dispose();
  }
}