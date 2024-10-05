// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System.Runtime.InteropServices;
using System.Text;

namespace UnitTests
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

    [TestClass]
    public class StringTests : BaseTests
    {
        IntPtr hModule;

        [TestInitialize]
        public void Initialize()
        {
            hModule = LoadLibraryExW(ExePath, 0, 2);
        }

        [TestCleanup]
        public void Cleanup()
        {
            FreeLibrary(hModule);
        }

        [TestMethod]
        public void TestSingleString()
        {
            string str = LoadString(hModule, 1001, 0x409);
            Assert.AreEqual("*", str);
        }

        [TestMethod]
        public void TestEnglishStrings()
        {
            TestStrings(new Dictionary<EDLMES, string>(){
                { EDLMES.EOF, "End of input file\r\n"},
                { EDLMES.PROMPT, "*"},
                { EDLMES.QMES, "Abort edit (Y/N)? "},
                { EDLMES.BADCOM, "Entry error\r\n"},
                { EDLMES.ESCAPE, "\\\r\n"},
                { EDLMES.CTRLC, "^C\r\n\r\n"},
                { EDLMES.NDNAME, "File name must be specified\r\n"},
                { EDLMES.NOSUCH, "Not found.\r\n"},
                { EDLMES.ASK, "O.K.? "},
                { EDLMES.MRGERR, "Not enough room to merge the entire file\r\n"},
                { EDLMES.MEMFUL, "Insufficient memory\r\n"},
                { EDLMES.TOOLNG, "Line too long\r\n"},
                { EDLMES.NEWFIL, "New file\r\n"},
                { EDLMES.NN, "Nn"},
                { EDLMES.YY, "Yy"}
            },
            0x409, "en-US");
        }

        [TestMethod]
        public void TestGermanStrings()
        {
            TestStrings(new Dictionary<EDLMES, string>(){
                { EDLMES.EOF, "Ende der Eingabedatei\r\n"},
                { EDLMES.PROMPT, "*"},
                { EDLMES.QMES, "Abbrechen? (J/N)? "},
                { EDLMES.BADCOM, "Eingabefehler\r\n"},
                { EDLMES.ESCAPE, "\\\r\n"},
                { EDLMES.CTRLC, "^C\r\n\r\n"},
                { EDLMES.NDNAME, "Dateiname fehlt\r\n"},
                { EDLMES.NOSUCH, "Nicht gefunden\r\n"},
                { EDLMES.ASK, "Okay? "},
                { EDLMES.MRGERR, "Nicht gen\x00FCgend Speicher f\x00FCr die gesamte Datei\r\n"},
                { EDLMES.MEMFUL, "Kein Speicher mehr frei\r\n"},
                { EDLMES.TOOLNG, "Zeile ist zu lang\r\n"},
                { EDLMES.NEWFIL, "Neue Datei\r\n"},
                { EDLMES.NN, "Nn"},
                { EDLMES.YY, "Jj"}
            },
            0x407, "de-DE");
        }

        [TestMethod]
        public void TestFrenchStrings()
        {
            TestStrings(new Dictionary<EDLMES, string>(){
                { EDLMES.EOF, "Fin du fichier d'entr\x00E9e\r\n"},
                { EDLMES.PROMPT, "*"},
                { EDLMES.QMES, "Annuler? (O/N)? "},
                { EDLMES.BADCOM, "Erreur d'entr\x00E9e\r\n"},
                { EDLMES.ESCAPE, "\\\r\n"},
                { EDLMES.CTRLC, "^C\r\n\r\n"},
                { EDLMES.NDNAME, "Aucun nom de fichier\r\n"},
                { EDLMES.NOSUCH, "Non trouv\r\n"},
                { EDLMES.ASK, "O.K? "},
                { EDLMES.MRGERR, "Pas assez de m\x00E9moire pour le fichier complet\r\n"},
                { EDLMES.MEMFUL, "Pas assez de m\x00E9moire disponible\r\n"},
                { EDLMES.TOOLNG, "Erreur de longueur de cha\x00EEne\r\n"},
                { EDLMES.NEWFIL, "Nouveau fichier\r\n"},
                { EDLMES.NN, "Nn"},
                { EDLMES.YY, "Oo"}
            },
            0x40C, "fr-FR");
        }

        [TestMethod]
        public void TestSpanishStrings()
        {
            TestStrings(new Dictionary<EDLMES, string>(){
                { EDLMES.EOF, "Fin de la entrada\r\n"},
                { EDLMES.PROMPT, "*"},
                { EDLMES.QMES, "\x00BFAbortar? (S/N)? "},
                { EDLMES.BADCOM, "Error de entrada\r\n"},
                { EDLMES.ESCAPE, "\\\r\n"},
                { EDLMES.CTRLC, "^C\r\n\r\n"},
                { EDLMES.NDNAME, "Falta el nombre del archivo\r\n"},
                { EDLMES.NOSUCH, "No hay texto coincidente\r\n"},
                { EDLMES.ASK, "\x00BFDe acuerdo? "},
                { EDLMES.MRGERR, "No hay suficiente espacio para todo el archivo\r\n"},
                { EDLMES.MEMFUL, "No hay espacio disponible\r\n"},
                { EDLMES.TOOLNG, "La l\x00EDnea es demasiado larga\r\n"},
                { EDLMES.NEWFIL, "Nuevo archivo\r\n"},
                { EDLMES.NN, "Nn"},
                { EDLMES.YY, "Ss"}
            },
            0x40A, "es-ES_tradnl");
        }

        [TestMethod]
        public void TestItalianStrings()
        {
            TestStrings(new Dictionary<EDLMES, string>(){
                { EDLMES.EOF, "Fine dell'input\r\n"},
                { EDLMES.PROMPT, "*"},
                { EDLMES.QMES, "Interrompere? (S/N)? "},
                { EDLMES.BADCOM, "Errore di immissione\r\n"},
                { EDLMES.ESCAPE, "\\\r\n"},
                { EDLMES.CTRLC, "^C\r\n\r\n"},
                { EDLMES.NDNAME, "Nome file mancante\r\n"},
                { EDLMES.NOSUCH, "Non trovato\r\n"},
                { EDLMES.ASK, "OK? "},
                { EDLMES.MRGERR, "Spazio insufficiente per l'intero file\r\n"},
                { EDLMES.MEMFUL, "Nessuno spazio disponibile\r\n"},
                { EDLMES.TOOLNG, "Riga troppo lunga\r\n"},
                { EDLMES.NEWFIL, "Nuovo file\r\n"},
                { EDLMES.NN, "Nn"},
                { EDLMES.YY, "Ss"}
            },
            0x410, "it-IT");
        }

        void TestStrings(IDictionary<EDLMES, string> strings, UInt16 lang, string name)
        {
            StringBuilder sb = new StringBuilder(500);

            GetLocaleInfoW(lang, 0x5C, sb, 500);

            Assert.AreEqual(sb.ToString(), name);

            foreach (var id in strings)
            {
                string value = LoadString(hModule, (int)id.Key, lang);

                Assert.AreEqual(id.Value, value, $"{lang},{name},{id.Key}");
            }
        }

        static string LoadString(IntPtr hModule, int id, UInt16 lang)
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
        private static extern int FreeLibrary(IntPtr hModule);

        [DllImport("kernel32.dll")]
        static extern IntPtr FindResourceEx(IntPtr hModule, IntPtr lpType, IntPtr lpName, UInt16 wLanguage);

        [DllImport("kernel32.dll")]
        static extern int SizeofResource(IntPtr hModule, IntPtr hResource);

        [DllImport("kernel32.dll")]
        static extern IntPtr LoadResource(IntPtr hModule, IntPtr hResource);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern int GetLocaleInfoW(IntPtr lpLocaleName, UInt32 code, StringBuilder lpLCData, int cchData);
    }
}
