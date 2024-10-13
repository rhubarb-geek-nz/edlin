// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;
using System.IO;

namespace nlstool
{
    internal class HeaderGenerator
    {
        internal readonly string os;
        internal readonly string ext;
        private StreamWriter outputFile;

        internal HeaderGenerator(string os, string ext)
        {
            this.os = os;
            this.ext = ext;
        }

        internal void GenerateFile(string dir)
        {
            string path = Path.Combine(dir, os);
            path = Path.Combine(path, $"edlmes.{ext}");

            outputFile = File.CreateText(path);

            try
            {
                WriteLine("/************************************");
                WriteLine(" * Copyright (c) 2024 Roger Brown.");
                WriteLine(" * Licensed under the MIT License.");
                WriteLine(" */");
                WriteLine("");

                Type type = typeof(EDLMES);

                foreach (int value in new EnumEnumerator(type))
                {
                    string define = $"#define {type.Name}_{type.GetEnumName(value)} ";
                    int len = define.Length;
                    len = 22 - len;

                    if (len > 0)
                    {
                        char[] buf = new char[len];
                        Array.Fill(buf, ' ');
                        define += new String(buf);
                    }

                    WriteLine($"{define}{value}");
                }
            }
            finally
            {
                outputFile.Close();
                outputFile = null;
            }
        }

        protected void WriteLine(string s)
        {
            outputFile.WriteLine(s);
        }
    }
}
