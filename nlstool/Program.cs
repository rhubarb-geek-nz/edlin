﻿// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using nlstool;
using System;
using System.IO;

#if DEBUG
string configuration = "Debug";
#else
string configuration = "Release";
#endif

string arch = System.Runtime.InteropServices.RuntimeInformation.ProcessArchitecture.ToString();
string dir = "..\\..\\..\\..";
string ExePath = dir + "\\" + arch + "\\" + configuration + "\\edlin.exe";
string pwd = Environment.CurrentDirectory;
var file = File.GetAttributes(ExePath);

IntPtr hModule = NLSGenerator.LoadLibrary(ExePath);

if (hModule == IntPtr.Zero)
{
    throw new FileNotFoundException(ExePath);
}

try
{
    foreach (UInt16 lang in new UInt16[] { 7, 9, 10, 12, 16 })
    {
        new OS2MessageFile(hModule, lang, "dos", "txt").GenerateFile(dir);
        new GNUGetText(hModule, lang, "posix", "po").GenerateFile(dir);
        new MessageCatalog(hModule, lang, "posix", "msg").GenerateFile(dir);
    }
}
finally
{
    NLSGenerator.FreeLibrary(hModule);
}
