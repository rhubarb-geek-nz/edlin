// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;

namespace nlstool
{
    internal class MessageCatalog : NLSGenerator
    {
        internal MessageCatalog(IntPtr hModule, UInt16 lang, string os, string ext) : base(hModule, lang, os, ext)
        {
        }

        protected override void Generate()
        {
            WriteLine("$ codeset=utf-8");
            WriteLine("$quote \"");
            WriteLine("$set 1");
            foreach (int id in new EnumEnumerator(typeof(EDLMES)))
            {
                string str = LoadString(id, lang);
                str = QuoteString(str);
                WriteLine($"{id} {str}");
            }
        }
    }
}
