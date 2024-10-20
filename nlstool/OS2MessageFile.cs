// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;

namespace nlstool
{
    internal class OS2MessageFile : NLSGenerator
    {
        internal OS2MessageFile(IntPtr hModule, UInt16 lang, string os, string ext) : base(hModule, lang, os, ext)
        {
        }

        char MessageType(EDLMES e)
        {
            switch (e)
            {
                case EDLMES.PROMPT:
                case EDLMES.QMES:
                case EDLMES.ASK:
                case EDLMES.NN:
                case EDLMES.YY:
                case EDLMES.ESCAPE:
                case EDLMES.CTRLC:
                    return 'P';

                case EDLMES.NDNAME:
                case EDLMES.BADCOM:
                case EDLMES.MEMFUL:
                case EDLMES.MRGERR:
                case EDLMES.TOOLNG:
                case EDLMES.DEST:
                case EDLMES.NOBAK:
                    return 'E';
            }

            return 'I';
        }

        protected override void Generate()
        {
            WriteLine("; Copyright (c) 2024 Roger Brown.");
            WriteLine("; Licensed under the MIT License.");
            WriteLine("EDL");
            foreach (int id in new EnumEnumerator(typeof(EDLMES)))
            {
                char c = MessageType((EDLMES)id);
                string s = LoadString(id, lang);
                int cr = s.IndexOf('\r');
                if (cr == -1)
                {
                    s = s + "%0";
                }
                else
                {
                    s = s.Substring(0, cr);
                }
                WriteLine($"EDL{id}{c}: {s}");
            }
        }
    }
}
