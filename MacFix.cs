using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SuperSystems.UnityBuild;
using System;
using UnityEditor;
using System.Diagnostics;
using System.IO;
using System.Text;
//using System.Diagnostics;

public class MacFix : BuildAction, IPostBuildAction, IPostBuildPerPlatformAction
{
    [FilePath(true)]
    public string cygwinRunPath;

    public string outputPath = "$BUILDPATH";
    public string outputFileName;

    private static Process process;


    public override void PerBuildExecute(BuildReleaseType releaseType, BuildPlatform platform, BuildArchitecture architecture, BuildDistribution distribution, DateTime buildTime, ref BuildOptions options, string configKey, string buildPath)
    {
        string resolvedCygwinRunPath = Path.Combine(cygwinRunPath, "run");
        string resolvedCygwinBashPath = Path.Combine(cygwinRunPath, "bash.exe");

        //string resolvedOutputPath = Path.Combine(outputPath.Replace("$BUILDPATH", buildPath), outputFileName);
        string resolvedOutputPath = Path.Combine(outputPath.Replace("$BUILDPATH", buildPath), outputFileName);
        resolvedOutputPath = BuildProject.ResolvePath(resolvedOutputPath, releaseType, platform, architecture, distribution, buildTime);
        if (!resolvedOutputPath.EndsWith(".zip"))
            resolvedOutputPath += ".zip";

        //This is full path till name.zip
        string finalOutputPath = Path.GetFullPath(resolvedOutputPath);

        UnityEngine.Debug.Log("resolvedOutputPath " + resolvedOutputPath);
        UnityEngine.Debug.Log("finalOutputPath " + finalOutputPath);

        string parentPath = Path.GetDirectoryName(finalOutputPath);
        if (!Directory.Exists(parentPath))
        {
            Directory.CreateDirectory(parentPath);
        }

        //cygwin chmod

        //UnityEngine.Debug.Log("MAC BUILD: " + "chmod a+x " + buildPath.Replace("\\", "/") + @"/HelpClub.app/Contents/MacOS/HelpClub");

        process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = resolvedCygwinBashPath,
                Arguments = "-l -c 'cd " + buildPath.Replace("\\", "/") + @"/Innerworld.app/Contents/MacOS;chmod a+x Innerworld'",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardError = true
            }
        };
        process.Start();


        StringBuilder outputText = new StringBuilder();

        // Capture stdout.
        process.OutputDataReceived += (sendingProcess, outputLine) =>
        {
            outputText.AppendLine(outputLine.Data);
        };

        while (!process.StandardOutput.EndOfStream)
        {
            outputText.Append(process.StandardOutput.ReadLine());
        }

        UnityEngine.Debug.Log(outputText);


        process.WaitForExit();


        outputText = new StringBuilder();

        bool IsFailure = false;
        while (!process.StandardError.EndOfStream)
        {
            IsFailure = true;
            outputText.Append(process.StandardError.ReadLine());
        }
        UnityEngine.Debug.Log(outputText);


        if (IsFailure)
        {
            BuildNotificationList.instance.AddNotification(new BuildNotification(
          BuildNotification.Category.Error,
          "MacFix Failed at chmod.", string.Format("Exit code: {0}\n{1}", process.ExitCode, outputText.ToString()),
          true, null));
        }


        //cygwin zip
        process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = resolvedCygwinBashPath,
                Arguments = "-l -c 'cd "+ buildPath.Replace("\\", "/") + ";zip -r " + finalOutputPath.Replace("\\", "/") + " " + @"Innerworld.app'",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardError = true
            }
        };
        process.Start();


        StringBuilder outputText2 = new StringBuilder();

        // Capture stdout.
        process.OutputDataReceived += (sendingProcess, outputLine) =>
        {
            outputText2.AppendLine(outputLine.Data);
        };

        while (!process.StandardOutput.EndOfStream)
        {
            outputText2.Append(process.StandardOutput.ReadLine());
        }

        UnityEngine.Debug.Log(outputText2);


        process.WaitForExit();


        outputText2 = new StringBuilder();

        bool IsFailure2 = false;
        while (!process.StandardError.EndOfStream)
        {
            IsFailure2 = true;
            outputText2.Append(process.StandardError.ReadLine());
        }
        UnityEngine.Debug.Log(outputText2);


        if (IsFailure2)
        {
            BuildNotificationList.instance.AddNotification(new BuildNotification(
          BuildNotification.Category.Error,
          "MacFix Failed at zipping.", string.Format("Exit code: {0}\n{1}", process.ExitCode, outputText2.ToString()),
          true, null));
        }
        
    }
}


