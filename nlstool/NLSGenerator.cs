// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace nlstool
{
    internal enum EDLMES
    {
        EOF = 1000,
        PROMPT = 1001,
        QMES = 1002,
        BADCOM = 1003,
        ESCAPE = 1004,
        CTRLC = 1005,
        NDNAME = 1006,
        NOSUCH = 1007,
        ASK = 1008,
        MRGERR = 1009,
        MEMFUL = 1010,
        TOOLNG = 1011,
        NEWFIL = 1012,
        NN = 1013,
        YY = 1014
    }

    internal abstract class NLSGenerator
    {
        protected readonly IntPtr hModule;
        protected readonly UInt16 lang;
        internal readonly string os;
        internal readonly string ext;
        private StreamWriter outputFile;
        protected NLSGenerator(IntPtr hModule, UInt16 lang, string os, string ext)
        {
            this.hModule = hModule;
            this.lang = lang;
            this.os = os;
            this.ext = ext;
        }

        protected virtual string GetFilePath(string dir)
        {
            string langName = GetLanguageName(lang).Split('-')[0];
            string path = Path.Combine(dir, os);
            path = Path.Combine(path, "nls");
            return Path.Combine(path, $"edlin-{langName}.{ext}");
        }

        internal void GenerateFile(string dir)
        {
            outputFile = File.CreateText(GetFilePath(dir));

            try
            {
                Generate();
            }
            finally
            {
                outputFile.Close();
                outputFile = null;
            }
        }

        protected abstract void Generate();


        protected void WriteLine(string s)
        {
            outputFile.WriteLine(s);
        }

        protected string LoadString(int id, UInt16 lang)
        {
            int resId = 1 + (id >> 4);
            id &= 0xf;
            IntPtr hRes = FindResourceEx(hModule, (IntPtr)6, (IntPtr)resId, lang);
            int dwSize = SizeofResource(hModule, hRes);
            IntPtr ptr = LoadResource(hModule, hRes);
            dwSize >>= 1;
            char[] ch = new char[dwSize];
            Marshal.Copy(ptr, ch, 0, dwSize);
            int offset = 0;

            while (offset < dwSize)
            {
                int len = ch[offset++];

                if (0 == id--)
                {
                    if (len == 0)
                    {
                        return String.Empty;
                    }

                    return new string(ch, offset, len);
                }

                offset += len;
            }

            return null;
        }

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern IntPtr LoadLibraryExW(string lpszLibName, IntPtr handle, UInt32 flags);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern int FreeLibrary(IntPtr hModule);

        [DllImport("kernel32.dll")]
        static extern IntPtr FindResourceEx(IntPtr hModule, IntPtr lpType, IntPtr lpName, UInt16 wLanguage);

        [DllImport("kernel32.dll")]
        static extern int SizeofResource(IntPtr hModule, IntPtr hResource);

        [DllImport("kernel32.dll")]
        static extern IntPtr LoadResource(IntPtr hModule, IntPtr hResource);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern int GetLocaleInfoW(IntPtr lpLocaleName, UInt32 code, StringBuilder lpLCData, int cchData);

        static internal IntPtr LoadLibrary(string lpszLibName)
        {
            return LoadLibraryExW(lpszLibName, 0, 2);
        }
        internal string GetLanguageName(UInt16 lang)
        {
            StringBuilder sb = new StringBuilder(25);

            GetLocaleInfoW(lang, 0x5C, sb, 25);

            return sb.ToString();
        }

        protected string QuoteString(string str)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append('\"');

            foreach (char c in str)
            {
                switch (c)
                {
                    case '\\':
                        sb.Append("\\\\");
                        break;

                    case '\r':
                        sb.Append("\\r");
                        break;

                    case '\n':
                        sb.Append("\\n");
                        break;

                    case '\t':
                        sb.Append("\\t");
                        break;

                    case '\b':
                        sb.Append("\\b");
                        break;

                    default:
                        sb.Append(c); break;
                }
            }

            sb.Append('\"');

            return sb.ToString();
        }
    }
}
