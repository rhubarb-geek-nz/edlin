// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System.Diagnostics;

namespace UnitTests
{
    public class Result
    {
        public readonly string stdout, stderr;
        public string[] content;
        public readonly int exitCode;

        public Result(string stdout, string stderr, string[] content, int exitCode)
        {
            this.stdout = stdout;
            this.stderr = stderr;
            this.content = content;
            this.exitCode = exitCode;
        }
    }

    [TestClass]
    public abstract class BaseTests
    {
#if DEBUG
        protected static readonly string configuration = "Debug";
#else
        protected static readonly string configuration = "Release";
#endif
        protected static readonly string arch= System.Runtime.InteropServices.RuntimeInformation.ProcessArchitecture.ToString();
        protected static readonly string ExePath = "..\\..\\..\\..\\"+arch+"\\"+ configuration + "\\edlin.exe";

        protected static string SyntaxError = "Entry error";
        protected Result RunApp(string input, string[] file)
        {
            Result result;
            string fileName = "foo.txt";

            if (file != null)
            {
                File.WriteAllLines(fileName, file);
            }

            try
            {
                Process proc = new Process();
                if (file!=null)
                {
                    proc.StartInfo = new ProcessStartInfo(ExePath, fileName);
                }
                else
                {
                    proc.StartInfo = new ProcessStartInfo(ExePath);
                }
                proc.StartInfo.RedirectStandardError = true;
                proc.StartInfo.RedirectStandardOutput = true;
                proc.StartInfo.RedirectStandardInput = true;
                proc.Start();

                if (input != null)
                {
                    proc.StandardInput.Write(input);
                    proc.StandardInput.Close();
                }

                proc.WaitForExit();

                var stdout=proc.StandardOutput.ReadToEnd();
                var stderr = proc.StandardError.ReadToEnd();
                int exitCode = proc.ExitCode;

                string[] content = null;

                if (file != null)
                {
                    content = File.ReadAllLines(fileName);
                }

                result = new Result(stdout, stderr, content, exitCode);
            }
            finally
            {
                if (file != null)
                {
                    File.Delete(fileName);
                }
            }

            return result;
        }
    }
}
