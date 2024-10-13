// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;
using System.IO;

namespace nlstool
{
    internal class ValueGenerator : NLSGenerator
    {
        readonly string langName;

        internal ValueGenerator(IntPtr hModule, UInt16 lang, string os, string ext) : base(hModule, lang, os, ext)
        {
            langName = GetLanguageName(lang).Split('-')[0];
        }

        protected override string GetFilePath(string dir)
        {
            string path = Path.Combine(dir, os);
            return Path.Combine(path, $"edlmes{langName}.{ext}");
        }

        protected override void Generate()
        {
            foreach (int id in new EnumEnumerator(typeof(EDLMES)))
            {
                EDLMES key = (EDLMES)id;
                string str = LoadString(id, lang);
                str = QuoteString(str);

                WriteLine($"#define {key.GetType().Name}{langName.ToUpper()}_{key} {str}");
            }
        }
    }
}
