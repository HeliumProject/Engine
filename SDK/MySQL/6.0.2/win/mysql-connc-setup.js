WiXSource = "C:\\Program Files\\WiX\\"; // This should be automated.

ForReading = 1;
ForWriting = 2;
ForAppending = 8;

try
{
    var fso = new ActiveXObject("Scripting.FileSystemObject");
    if( checkDirectories(fso) != true)
        throw new Error(1, "Invalid directory structure.");

    var pkg_version      = "";
    var pkg_platform     = "win32";
    var pkg_signed       = "no"
    var pkg_default_dest = "ProgramFilesFolder";
    var pkg_pid_platform = "Intel";
    var pkg_product_dest = "MySQL Connector C ";

    var args = WScript.Arguments;
    for (i=0; i < args.Count(); i++)
    {
        var parts = args.Item(i).split('=');
        switch (parts[0])
        {
          case "version":
               pkg_version = parts[1];
               if ( !/\d.\d.\d.\d/.test(pkg_version))
                   throw new Error(1, "Incorrect version format");
               break;
          case "platform":
               pkg_platform = parts[1];
               break;
          case "signed":
               pkg_signed = parts[1];
               break;
          default:
               throw new Error(1, "Invalid argument supplied.");
        }
    }
    if (i != 2 && i != 3)
      throw new Error(1, "Incorrect number of arguments.");

    if (pkg_platform != "win32")
    {
        pkg_default_dest = "ProgramFiles64Folder";
        pkg_pid_platform = "x64";
    }
    pkg_product_dest += pkg_version;

    // read in the WiX template file
    var wix_templateTS = fso.OpenTextFile("win\\mysql-connc-msi-template.xml", ForReading);
    var wix_template = wix_templateTS.ReadAll();
    wix_templateTS.Close();

    wix_template = ReplaceValue(wix_template, "PKG_NEW_GUID", getGUIDString());
    wix_template = ReplaceValue(wix_template, "PKG_NEW_GUID", getGUIDString());
    wix_template = ReplaceValue(wix_template, "PKG_VERSION", pkg_version);
    wix_template = ReplaceValue(wix_template, "PKG_PID_PLATFORM", pkg_pid_platform);
    wix_template = ReplaceValue(wix_template, "PKG_DEFAULT_DEST", pkg_default_dest);
    wix_template = ReplaceValue(wix_template, "PKG_PRODUCT_DEST", pkg_product_dest);

    if (wix_template == 0)
      throw new Error(1, "Bad Template Input");

    var wix_final = fso.CreateTextFile("win\\mysql-connector-c-" + pkg_platform + ".xml", true);
    wix_final.Write(wix_template);
    wix_final.Close();

    WScript.Echo("Updated the WiX XML for this version/platform.");
    WScript.Echo("Please remember to check-in win\\mysql-connector-c-" + pkg_platform + " if this is an offical release.\n");

    WScript.Echo("Compiling WiX.\n");
    ExecuteShell("candle.exe -out mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".wixobj win\\mysql-connector-c-" + pkg_platform + ".xml");

    WScript.Echo("Linking WiX.\n");
    ExecuteShell("light.exe -out win\\mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".msi mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".wixobj \"" + WiXSource + "WixUI.wixlib\" -loc \"" + WiXSource + "WixUI_en-us.wxl\"");

    if (pkg_signed == "yes")
    {
        WScript.Echo("Signing MSI\n");
        ExecuteShell("signtool.exe sign /a /d \"MySQL Connector C " + pkg_version + "\" /du \"http://www.mysql.com\" /t \"http://timestamp.verisign.com/scripts/timestamp.dll\" mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".msi");

        WScript.Echo("Creating MD5 checksum\n");
        ExecuteShell("..\\..\\utils\\md5sum mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".msi > mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".msi.md5");
    }

    WScript.Echo("Cleaning up a little.\n");
    leftOvers = fso.GetFile(".\\mysql-connector-c-" + pkg_version + "-" + pkg_platform + ".wixobj");
    leftOvers.Delete();

    WScript.Echo("It's nice.\n");
    WScript.Quit(0);
}
catch (e)
{
    WScript.Echo("Error: " + e.description);
    printUsage();
    WScript.Quit(1);
}

/* Generate a GUID and return a formatted string */
function getGUIDString()
{
     return new ActiveXObject("Scriptlet.TypeLib").GUID.substr(1,36);
}

/* Replace the first occurance of key with rpl in str */
function ReplaceValue(str, key, rpl)
{
    var pos = str.indexOf(key);
      if (pos == -1) return 0;
    var new_string = str.substring(0, pos) + rpl + str.substring(pos + key.length, str.length);
    return new_string;
}

/* Print Usage */
function printUsage()
{
    WScript.Echo("mysql-connc-setup.js - Updates the Product GUID and version strings in the");
    WScript.Echo("                       mysql-connector-c-VERSION-PLAT.xml created for this");
    WScript.Echo("                       MSI creation attempt.");
    WScript.Echo("  3 command line parameters must be supplied:");
    WScript.Echo("  version  - Version of the package. ie 5.0.0.0");
    WScript.Echo("  platform - x64 or win32 (default)");
    WScript.Echo("  signed - yes or no (default)");
    WScript.Echo("  Example:");
    WScript.Echo("  cscript mysql-connc-setup.js version=5.0.32.0 platform=x64 signed=yes");
    WScript.Echo("  Note:");
    WScript.Echo("  This script should be executed from the root of an lp:libmysql repository.");
}

function checkDirectories(FSO)
{
    var retVal = true;
    retVal &= FSO.FolderExists(".\\include");
    retVal &= FSO.FolderExists(".\\include\\atomic");
    retVal &= FSO.FolderExists(".\\include\\mysys");
    retVal &= FSO.FolderExists(".\\extlib\\dbug\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\extlib\\dbug\\debug");
    retVal &= FSO.FolderExists(".\\extlib\\zlib\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\extlib\\zlib\\debug");
    retVal &= FSO.FolderExists(".\\extlib\\yassl\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\extlib\\yassl\\debug");
    retVal &= FSO.FolderExists(".\\extlib\\regex\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\extlib\\regex\\debug");
    retVal &= FSO.FolderExists(".\\libmysql\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\libmysql\\debug");
    retVal &= FSO.FolderExists(".\\mysql_config\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\mysql_config\\debug");
    retVal &= FSO.FolderExists(".\\mysys\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\mysys\\debug");
    retVal &= FSO.FolderExists(".\\strings\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\strings\\debug");
    retVal &= FSO.FolderExists(".\\tests\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\tests\\debug");
    retVal &= FSO.FolderExists(".\\vio\\relwithdebinfo");
    retVal &= FSO.FolderExists(".\\vio\\debug");
    return retVal;
}

function ReadAllFromAny(oExec)
{
     if (!oExec.StdOut.AtEndOfStream)
          return oExec.StdOut.ReadAll();

     if (!oExec.StdErr.AtEndOfStream)
          return oExec.StdErr.ReadAll();

     return -1;
}

function ExecuteShell(cmd)
{
    var WshShell = new ActiveXObject("WScript.Shell");
    var oExec    = WshShell.Exec(cmd);
    var allInput = "";
    var tryCount = 0;

    while (true)
    {
         var input = ReadAllFromAny(oExec);
         if (-1 == input)
         {
              if (tryCount++ > 10 && oExec.Status == 1)
                   break;
              WScript.Sleep(100);
         }
         else
         {
              allInput += input;
              tryCount = 0;
         }
    }

    WScript.Echo(allInput);

    if (oExec. ExitCode!= 0)
        throw new Error(1, "Shell command failed.  The error should have been displayed above.");
}
