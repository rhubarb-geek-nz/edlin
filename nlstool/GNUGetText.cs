// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;

namespace nlstool
{
    internal class GNUGetText : NLSGenerator
    {
        internal GNUGetText(IntPtr hModule, UInt16 lang, string os, string ext) : base(hModule, lang, os, ext)
        {
        }

        protected override void Generate()
        {
            WritePrompt("msgid", string.Empty);
            WritePrompt("msgstr", string.Empty);
            WriteLine(QuoteString("Content-Type: text/plain; charset=UTF-8\n"));
            for (int id = 1000; id <= 1014; id++)
            {
                EDLMES e = (EDLMES)id;
                WriteLine($"# {id}, {e.GetType().Name}_{e}");
                WritePrompt("msgid", LoadString(id, 9));
                WritePrompt("msgstr", LoadString(id, lang));
            }
        }

        void WritePrompt(string prompt, string str)
        {
            str = QuoteString(str);
            WriteLine($"{prompt} {str}");
        }
    }
}
