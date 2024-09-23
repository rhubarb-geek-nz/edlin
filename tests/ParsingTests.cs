// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

namespace UnitTests
{
    [TestClass]
    public class ParsingTests : BaseTests
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
        public void ParseSoloComma()
        {
            Result result = RunApp(",\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains(SyntaxError));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void ParseSpaceInNumber()
        {
            Result result = RunApp("1 2\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains(SyntaxError));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void ParseTwoCommas()
        {
            Result result = RunApp(",,\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains(SyntaxError));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void ParseNumberComma()
        {
            Result result = RunApp("1,\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains(SyntaxError));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void ParseNumberMinusNumber()
        {
            Result result = RunApp("1-2\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains(SyntaxError));

            Assert.AreEqual(result.exitCode, 0);
        }


        [TestMethod]
        public void CommandOnly()
        {
            Result result = RunApp("Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=0\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandWithOption()
        {
            Result result = RunApp("?Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=0,?\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandWithOneArg()
        {
            Result result = RunApp("10Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=1,10\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandWithTwoArgs()
        {
            Result result = RunApp("10,20Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=2,10,20\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandWithThreeArgs()
        {
            Result result = RunApp("12,34,56Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=3,12,34,56\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandWithFourArgs()
        {
            Result result = RunApp("12,34,56,78Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,34,56,78\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefault1()
        {
            Result result = RunApp(",34,56,78Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,1,34,56,78\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefault2()
        {
            Result result = RunApp("12,,56,78Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,1,56,78\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefault3()
        {
            Result result = RunApp("12,34,,78Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,34,1,78\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefault4()
        {
            Result result = RunApp("12,34,56,Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,34,56,1\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefaultNonOrigin2()
        {
            Result result = RunApp("2\r\n\r\n12,,56,78Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,2,56,78\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefaultNonOrigin4()
        {
            Result result = RunApp("2\r\n\r\n12,34,56,Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,12,34,56,1\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefaultAllNonOrigin()
        {
            Result result = RunApp("2\r\n\r\n,,,Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,2,2,2,1\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandDefaultSpacesAllNonOrigin()
        {
            Result result = RunApp("2\r\n\r\n , , , Z\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=4,2,2,2,1\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }

        [TestMethod]
        public void CommandMultipleCommands()
        {
            Result result = RunApp("z 1z 2,1z 3,4,5z 4,5,6z562\r\n", new string[] { "foo", "bar" });

            Assert.AreEqual(result.stderr, String.Empty);
            Assert.IsTrue(result.stdout.Contains("Z=0:25\r\n"));
            Assert.IsTrue(result.stdout.Contains("Z=1,1:22\r\n"));
            Assert.IsTrue(result.stdout.Contains("Z=2,2,1:17\r\n"));
            Assert.IsTrue(result.stdout.Contains("Z=3,3,4,5:10\r\n"));
            Assert.IsTrue(result.stdout.Contains("Z=3,4,5,6:3\r\n"));

            Assert.AreEqual(result.exitCode, 0);
        }
    }
}
