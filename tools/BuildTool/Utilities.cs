using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;

namespace BuildTool
{
    static class Utilities
    {
        /// <summary>
        /// Polls the system for LPT availability. This does not actually check if the PSX is actually connected
        /// to an LPT port
        /// </summary>
        static public void PollLPTAvailability()
        {
            // TODO: actually poll this
            bool isLPTavailable = true;


        }

        /// <summary>
        /// Get the PSXDEV_PATH environment variable value.
        /// </summary>
        /// <returns>The psxdev root directory.</returns>
        static public string GetPsxDevRoot()
        {
            string psxdevPath = Environment.GetEnvironmentVariable("PSXDEV_PATH", EnvironmentVariableTarget.User);
            if (psxdevPath == null)
            {
                throw new Exception("Environment variable \"PSXDEV_PATH\" was not found! Have you ran EnvVarsSetup.bat?");
            }

            return psxdevPath;
        }

        /// <summary>
        /// Returns the build output directory, relative to the PSXDEV_PATH.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="projectDir">The project directory.</param>
        /// <returns></returns>
        static public string GetBuildOuputDirectory(BuildConfiguration config, string projectDir)
        {
            return Path.Combine(projectDir, "bin", config.ToString());
        }

        /// <summary>
        /// Returns the build data directory, relative to the PSXDEV_PATH.
        /// </summary>
        /// <param name="projectDir">The project directory.</param>
        /// <returns></returns>
        static public string GetBuildDataDirectory(string projectDir)
        {
            return Path.Combine(projectDir, "data");
        }

        private enum LaunchProcessFlags
        {
            None        = 0,
            WaitForExit = 1 << 0,
            NoWindow    = 1 << 2
        }

        static private bool LaunchProcessInternal(string filename, string arguments, string workingDirectory, LaunchProcessFlags flags, StringBuilder output)
        {
            if (Directory.Exists(workingDirectory))
            {
                Process p = new Process();
                p.StartInfo.RedirectStandardError = true;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.UseShellExecute = false;

                if (output != null)
                {
                    p.OutputDataReceived += (sender, e) => output.AppendLine(e.Data);
                    p.ErrorDataReceived += (sender, e) => output.AppendLine(e.Data);
                }

                p.StartInfo.FileName = filename;
                p.StartInfo.Arguments = arguments;

                if ((flags & LaunchProcessFlags.NoWindow) != 0)
                {
                    p.StartInfo.CreateNoWindow = true;
                }

                p.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                p.StartInfo.WorkingDirectory = workingDirectory;

                p.Start();

                p.BeginOutputReadLine();
                p.BeginErrorReadLine();

                if ((flags & LaunchProcessFlags.WaitForExit) != 0)
                {
                    p.WaitForExit();
                }

                try
                {
                    return p.ExitCode == 0;
                }
                catch
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Launch a new process and wait until it exits.
        /// </summary>
        /// <param name="filename">The executable to start.</param>
        /// <param name="arguments">The executable's arguments.</param>
        /// <param name="workingDirectory">The working directory to start the process in.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        static public bool LaunchProcessAndWait(string filename, string arguments, string workingDirectory, StringBuilder output)
        {
            return LaunchProcessInternal(filename, arguments, workingDirectory, LaunchProcessFlags.WaitForExit | LaunchProcessFlags.NoWindow, output);
        }

        /// <summary>
        /// Launch a new process.
        /// </summary>
        /// <param name="filename">The executable to start.</param>
        /// <param name="arguments">The executable's arguments.</param>
        /// <param name="workingDirectory">The working directory to start the process in.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        static public bool LaunchProcess(string filename, string arguments, string workingDirectory, StringBuilder output)
        {
            return LaunchProcessInternal(filename, arguments, workingDirectory, LaunchProcessFlags.NoWindow, output);
        }

        /// <summary>
        /// Checks if the specified build configuration is a debug config or not.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <returns>True if it's debug, false otherwise.</returns>
        static public bool IsDebugConfig(BuildConfiguration config)
        {
            return (config == BuildConfiguration.DebugEMU) || (config == BuildConfiguration.DebugPSX);
        }

        /// <summary>
        /// Checks if the specified build configuration is a release config or not.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <returns>True if it's release, false otherwise.</returns>
        static public bool IsReleaseConfig(BuildConfiguration config)
        {
            return (config == BuildConfiguration.ReleaseEMU) || (config == BuildConfiguration.ReleasePSX);
        }

        /// <summary>
        /// Checks if the specified build configuration is a profile config or not.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <returns>True if it's profile, false otherwise.</returns>
        static public bool IsProfileConfig(BuildConfiguration config)
        {
            return (config == BuildConfiguration.ProfileEMU) || (config == BuildConfiguration.ProfilePSX);
        }

        /// <summary>
        /// Checks if the specified build configuration is a PSX config or not.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <returns>True if it's PSX, false otherwise.</returns>
        static public bool IsPSXConfig(BuildConfiguration config)
        {
            return !IsEMUConfig(config);
        }

        /// <summary>
        /// Checks if the specified build configuration is an EMU config or not.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <returns>True if it's EMU, false otherwise.</returns>
        static public bool IsEMUConfig(BuildConfiguration config)
        {
            return (config == BuildConfiguration.DebugEMU) || (config == BuildConfiguration.ReleaseEMU) || (config == BuildConfiguration.ProfileEMU) || (config == BuildConfiguration.FinalEMU);
        }
    }
}
