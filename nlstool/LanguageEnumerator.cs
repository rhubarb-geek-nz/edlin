// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace nlstool
{
    internal class LanguageEnumerator : List<UInt16>
    {
        internal LanguageEnumerator(IntPtr hModule)
        {
            EnumResourceLanguagesW(hModule, 6, 1 + ((int)EDLMES.EOF >> 4), EnumResLangProc, 0);
        }

        private int EnumResLangProc(nint hModule, nint lpType, nint lpName, ushort wLanguage, nint lParam)
        {
            Add(wLanguage);

            return 1;
        }

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int ENUMRESLANGPROC(IntPtr hModule, IntPtr lpType, IntPtr lpName, ushort wLanguage, IntPtr lParam);

        [DllImport("kernel32.dll")]
        public static extern bool EnumResourceLanguagesW(IntPtr hModule, IntPtr lpType, IntPtr lpName, ENUMRESLANGPROC lpEnumFunc, IntPtr lParam);
    }
}
