// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

namespace UnitTests
{
    [TestClass]
    public class FileTests : BaseTests
    {

        [TestInitialize]
        public void Initialize()
        {
        }

        [TestCleanup]
        public void Cleanup()
        {
        }

        [TestMethod]
        public void FileSaveWorks()
        {
            Result result = RunApp("1D\r\nE\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("E\r\n"));

            Assert.AreEqual(result.content.Length, 1);
            Assert.AreEqual(result.content[0],"bar");

            Assert.AreEqual(result.exitCode, 0);
        }
    }
}
