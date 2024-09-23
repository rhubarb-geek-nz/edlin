// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

namespace UnitTests
{
    [TestClass]
    public class StartupTests : BaseTests
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
        public void TestAppExists()
        {
            string pwd = Environment.CurrentDirectory;
            var file = File.GetAttributes(ExePath);
            Assert.IsNotNull(pwd);
            Assert.AreEqual(file, FileAttributes.Archive, "Should be a normal file");
        }

        [TestMethod]
        public void RunWithNoFile()
        {
            Result result = RunApp(null, null);

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("File name must be specified"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void RunWithFile()
        {
            Result result = RunApp("E\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("E\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }
    }
}
