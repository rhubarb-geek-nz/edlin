#!/usr/bin/env pwsh
# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

param(
	$CertificateThumbprint = '601A8B683F791E51F647D34AD102C38DA4DDB65F',
	$BundleThumbprint = '5F88DFB53180070771D4507244B2C9C622D741F8'
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

trap
{
	throw $PSItem
}

$Version = (Get-Content "$PSScriptRoot/version").Split('=')[-1]

Write-Output "Version is $Version"

if ($IsLinux)
{
	& make clean

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}

	& make CFLAGS="-Wall -Werror" edlin

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}

	& strip edlin

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}

	& make CFLAGS="-Wall -Werror"

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}
}

if ($IsMacOS)
{
	& make clean

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}

	foreach ($Arch in 'arm64', 'x86_64')
	{
		if (Test-Path -LiteralPath 'config.h' -PathType Leaf)
		{
			Remove-Item -LiteralPath 'config.h'
		}

		& make CFLAGS="-Wall -Werror -pedantic -arch $Arch" edlin

		If ( $LastExitCode -ne 0 )
		{
			Exit $LastExitCode
		}

		& strip edlin
		
		If ( $LastExitCode -ne 0 )
		{
			Exit $LastExitCode
		}

		& codesign --timestamp --sign "Developer ID Application: $Env:APPLE_DEVELOPER" edlin

		If ( $LastExitCode -ne 0 )
		{
			Exit $LastExitCode
		}

		Move-Item -LiteralPath 'edlin' -Destination "edlin.$Arch"
	}

	& lipo edlin.arm64 edlin.x86_64 -create -output edlin

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}

	@( 'edlin.arm64', 'edlin.x86_64' ) | ForEach-Object {
		Remove-Item -LiteralPath $_
	}

	& make dist

	If ( $LastExitCode -ne 0 )
	{
		Exit $LastExitCode
	}
}

if ($IsWindows -or ( 'Desktop' -eq $PSEdition ))
{
	foreach ($EDITION in 'Community', 'Professional')
	{
		$VCVARSDIR = "${Env:ProgramFiles}\Microsoft Visual Studio\2022\$EDITION\VC\Auxiliary\Build"

		if ( Test-Path -LiteralPath $VCVARSDIR -PathType Container )
		{
			break
		}
	}

	$VCVARSARM = 'vcvarsarm.bat'
	$VCVARSARM64 = 'vcvarsarm64.bat'
	$VCVARSAMD64 = 'vcvars64.bat'
	$VCVARSX86 = 'vcvars32.bat'
	$VCVARSHOST = 'vcvars32.bat'

	switch ($Env:PROCESSOR_ARCHITECTURE)
	{
		'AMD64' {
			$VCVARSX86 = 'vcvarsamd64_x86.bat'
			$VCVARSARM = 'vcvarsamd64_arm.bat'
			$VCVARSARM64 = 'vcvarsamd64_arm64.bat'
			$VCVARSHOST = $VCVARSAMD64
			}
		'ARM64' {
			$VCVARSX86 = 'vcvarsarm64_x86.bat'
			$VCVARSARM = 'vcvarsarm64_arm.bat'
			$VCVARSAMD64 = 'vcvarsarm64_amd64.bat'
			$VCVARSHOST = $VCVARSARM64
		}
		'X86' {
			$VCVARSXARM64 = 'vcvarsx86_arm64.bat'
			$VCVARSARM = 'vcvarsx86_arm.bat'
			$VCVARSAMD64 = 'vcvarsx86_amd64.bat'
		}
		Default {
			throw "Unknown architecture $Env:PROCESSOR_ARCHITECTURE"
		}
	}

	$VCVARSARCH = @{'arm' = $VCVARSARM; 'arm64' = $VCVARSARM64; 'x86' = $VCVARSX86; 'x64' = $VCVARSAMD64}

	$ARCHLIST = ( $VCVARSARCH.Keys | ForEach-Object {
		$VCVARS = $VCVARSARCH[$_];
		if ( Test-Path -LiteralPath "$VCVARSDIR/$VCVARS" -PathType Leaf )
		{
			$_
		}
	} | Sort-Object )

	$ARCHLIST | ForEach-Object {
		New-Object PSObject -Property @{
			Architecture=$_;
			Environment=$VCVARSARCH[$_]
		}
	} | Format-Table -Property Architecture,'Environment'

	Push-Location 'win32'

	$Win32Dir = $PWD

	foreach ($DIR in 'obj', 'bin', 'bundle')
	{
		if (Test-Path -LiteralPath $DIR)
		{
			Remove-Item -LiteralPath $DIR -Force -Recurse
		}
	}

	try
	{
		$ARCHLIST | ForEach-Object {
			$ARCH = $_

			$VCVARS = ( '{0}\{1}' -f $VCVARSDIR, $VCVARSARCH[$ARCH] )

			$VersionStr4="$Version.0"
			$VersionInt4=$VersionStr4.Replace(".",",")

			$xmlDoc = [System.Xml.XmlDocument](Get-Content "Package.appxmanifest")

			$nsMgr = New-Object -TypeName System.Xml.XmlNamespaceManager -ArgumentList $xmlDoc.NameTable

			$nsmgr.AddNamespace("man", "http://schemas.microsoft.com/appx/manifest/foundation/windows10")

			$xmlNode = $xmlDoc.SelectSingleNode("/man:Package/man:Identity", $nsmgr)

			$xmlNode.ProcessorArchitecture = "$ARCH"
			$xmlNode.Version = $VersionStr4

			$xmlDoc.Save("$Win32Dir\AppxManifest.xml")

			$env:PRODUCTVERSION = $VersionStr4
			$env:PRODUCTARCH = $ARCH
			$env:PRODUCTWIN64 = 'yes'
			$env:PRODUCTPROGFILES = 'ProgramFiles64Folder'
			$env:INSTALLERVERSION = '500'

			switch ($ARCH)
			{
				'arm64' {
					$env:UPGRADECODE = '218D799F-DCCF-4738-9F16-6881EBDCB9D8'
				}

				'arm' {
					$env:UPGRADECODE = '5CA33CBB-A624-4BE3-9545-4B74295C26DC'
					$env:PRODUCTWIN64 = 'no'
					$env:PRODUCTPROGFILES = 'ProgramFilesFolder'
				}

				'x86' {
					$env:UPGRADECODE = 'A62112C4-69F3-460F-BDC2-ECFD2C941D2A'
					$env:PRODUCTWIN64 = 'no'
					$env:PRODUCTPROGFILES = 'ProgramFilesFolder'
					$env:INSTALLERVERSION = '200'
				}

				'x64' {
					$env:UPGRADECODE = '47A4431E-6C97-493B-8D20-7C9594A33A7D'
					$env:INSTALLERVERSION = '200'
				}
			}

			@"
CALL "$VCVARS"
IF ERRORLEVEL 1 EXIT %ERRORLEVEL%
NMAKE /NOLOGO clean
IF ERRORLEVEL 1 EXIT %ERRORLEVEL%
NMAKE /NOLOGO DEPVERS_edlin_STR4="$VersionStr4" DEPVERS_edlin_INT4="$VersionInt4" CertificateThumbprint="$CertificateThumbprint" BundleThumbprint="$BundleThumbprint"
EXIT %ERRORLEVEL%
"@ | & "$env:COMSPEC"

			if ($LastExitCode -ne 0)
			{
				exit $LastExitCode
			}
		}

		@"
CALL "$VCVARSDIR\$VCVARSHOST"
IF ERRORLEVEL 1 EXIT %ERRORLEVEL%
NMAKE /NOLOGO DEPVERS_edlin_STR4="$VersionStr4" DEPVERS_edlin_INT4="$VersionInt4" CertificateThumbprint="$CertificateThumbprint" BundleThumbprint="$BundleThumbprint" "edlin-$VersionStr4.msixbundle"
EXIT %ERRORLEVEL%
"@ | & "$env:COMSPEC"

		if ($LastExitCode -ne 0)
		{
			exit $LastExitCode
		}

		Push-Location 'bin'

		try
		{
			Compress-Archive $ARCHLIST -DestinationPath "..\edlin-$Version-win.zip" -Force

			$ARCHLIST | ForEach-Object {
				$ARCH = $_
				$VCVARS = ( '{0}\{1}' -f $VCVARSDIR, $VCVARSARCH[$ARCH] )
				foreach ($EXE in "$ARCH\edlin.exe", "$ARCH\edlmes.dll")
				{
					$MACHINE = ( @"
@CALL "$VCVARS" > NUL:
IF ERRORLEVEL 1 EXIT %ERRORLEVEL%
dumpbin /headers $EXE
IF ERRORLEVEL 1 EXIT %ERRORLEVEL%
EXIT %ERRORLEVEL%
"@ | & "$env:COMSPEC" /nologo /Q | Select-String -Pattern " machine " )

					$MACHINE = $MACHINE.ToString().Trim()

					$MACHINE = $MACHINE.Substring($MACHINE.LastIndexOf(' ')+1)

					New-Object PSObject -Property @{
						Architecture=$ARCH;
						Executable=$EXE;
						Machine=$MACHINE;
						FileVersion=(Get-Item $EXE).VersionInfo.FileVersion;
						ProductVersion=(Get-Item $EXE).VersionInfo.ProductVersion;
						FileDescription=(Get-Item $EXE).VersionInfo.FileDescription
					}
				}
			} | Format-Table -Property Architecture, Executable, Machine, FileVersion, ProductVersion, FileDescription
		}
		finally
		{
			Pop-Location
		}
	}
	finally
	{
		Pop-Location
	}
}
