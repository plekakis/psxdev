using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace BuildTool
{
    /// <summary>
    /// The configuration type
    /// </summary>
    public enum BuildConfiguration
    {
        DebugEMU,
        ReleaseEMU,
        FinalEMU,
        DebugPSX,
        ReleasePSX,
        FinalPSX,
        ConfigCount = FinalPSX
    }

    /// <summary>
    /// The license to embed in the generated CD image.
    /// </summary>
    public enum CDLicense
    {
        Europe,
        Americas,
        Japan
    }

    /// <summary>
    /// Encapsulates all clean/build/run functionality for a project.
    /// Supports the following configurations in <see cref="BuildConfiguration"/>    
    /// </summary>
    class Builder
    {
        public struct CompilationStages
        {
            public string m_ccpsxEngine; // The compilation step for the engine.c, this generates object file to be used in dmpsx engine step
            public string m_dmpsxEngine; // The dmpsx engine step; takes engine.obj and processes it to generate inline gte compatible code
            public string m_ccpsxGame;   // The compilation step for the game.c, this generates object file to be used in dmpsx game step  
            public string m_dmpsxGame;   // The dmpsx game step; takes game.obj and processes it to generate inline gte compatible code
            public string m_ccpsxFinal;  // Final compilation step  
        };

        private string      m_projectDirectory;        
        private bool        m_preserveOutput;

        /// <summary>
        /// Create a new builder which will work on the specified directory.
        /// </summary>
        /// <param name="projectDirectory">The project's root directory.</param>
        public Builder(string projectDirectory)
        {
            m_projectDirectory = projectDirectory;
        }

        /// <summary>
        /// Set/get the CDLicense.
        /// </summary>
        public CDLicense CDLicenseRegion
        {
            set;
            get;
        }

        /// <summary>
        /// Set/get whether cd image can be generated.
        /// </summary>
        public bool GenerateCDImage
        {
            set;
            get;
        }

        /// <summary>
        /// Get the project name (derive it from the project directory).
        /// </summary>
        private string ProjectName
        {
            get
            {
                return Path.GetFileName(m_projectDirectory);
            }
        }

        /// <summary>
        /// Generate an argument string for input to ccpsx.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="additionalPreprocessor">An array of preprocessor names.</param>
        /// <param name="additionalLinker">An array of linker settings.</param>
        /// <param name="additionalLibDirs">An array of library directories.</param>
        /// <param name="additionalIncludeDirs">An array of include directories.</param>
        /// <returns>The arguments string.</returns>
        public CompilationStages GetCCPSXArgs(BuildConfiguration config, string[] additionalPreprocessor, string[] additionalLinker, string[] additionalLibDirs, string[] additionalIncludeDirs)
        {
            CompilationStages stages = new CompilationStages();

            string psxdevRoot = Utilities.GetPsxDevRoot();

            // assemble the arguments
            string engineDirectory = "engine";
            string engineSources = Path.Combine(engineDirectory, "engine_scu");
            string gameSources = Path.Combine(m_projectDirectory, "source", "game_scu");
            string optimisationLevel = Utilities.IsDebugConfig(config) ? "-O0" : "-O3";
            string[] libraries = { "libpad" };

            // Per-config, per-target preprocessor macros.
            List<string> preprocessor = new List<string>();
            if (Utilities.IsDebugConfig(config))
            {
                preprocessor.Add("CONFIG_DEBUG");
            }
            else if (Utilities.IsReleaseConfig(config))
            {
                preprocessor.Add("CONFIG_RELEASE");
            }
            else
            {
                preprocessor.Add("CONFIG_FINAL");
            }

            if (Utilities.IsPSXConfig(config))
            {
                preprocessor.Add("TARGET_PSX");
            }
            else
            {
                preprocessor.Add("TARGET_EMU");
            }
            preprocessor.AddRange(additionalPreprocessor);

            // Linker options
            List<string> linker = new List<string>();
            linker.AddRange(libraries);
            linker.AddRange(additionalLinker);

            // Include directories
            List<string> includeDirs = new List<string>();
            includeDirs.Add(engineDirectory);
            includeDirs.AddRange(additionalIncludeDirs);

            // Library directories
            List<string> libraryDirs = new List<string>();
            libraryDirs.AddRange(additionalLibDirs);

            string compilationArgs = "";
            string linkerArgs = "";

            // Optimisation level
            compilationArgs += optimisationLevel + " ";
            
            // Add the preprocessor macros
            foreach (string pp in preprocessor)
            {
                compilationArgs += "-D" + pp + " ";
            }

            // Add the include directories
            foreach (string dir in includeDirs)
            {
                compilationArgs += "-I" + dir + " ";
            }

            // Add the linker options
            foreach (string ll in linker)
            {
                linkerArgs += "-l" + ll + " ";
            }

            // Add the library directories
            foreach (string dir in libraryDirs)
            {
                linkerArgs += "-L" + dir + " ";
            }

            string buildDir = Utilities.GetBuildOuputDirectory(config, m_projectDirectory);

            // CCPSX engine generation
            stages.m_ccpsxEngine += "-c -O " + engineSources + ".c" + " -o" + Path.Combine(buildDir, "engine.obj") + " ";
            stages.m_ccpsxEngine += compilationArgs;

            // CCPSX game generation
            stages.m_dmpsxEngine += Path.Combine(buildDir, "engine.obj");

            // DMPSX engine generation
            stages.m_ccpsxGame += "-c -O " + gameSources + ".c" + " -o" + Path.Combine(buildDir, "game.obj") + " ";
            stages.m_ccpsxGame += compilationArgs;

            // DMPSX game generation
            stages.m_dmpsxGame += Path.Combine(buildDir, "game.obj");

            // Final compilation step generation
            // Final arguments, output
            stages.m_ccpsxFinal = "-Xo$80010000 ";
            stages.m_ccpsxFinal += compilationArgs;
            stages.m_ccpsxFinal += linkerArgs;

            // Input source files
            stages.m_ccpsxFinal += Path.Combine(buildDir, "engine.obj") + " ";
            stages.m_ccpsxFinal += Path.Combine(buildDir, "game.obj") + " ";

            stages.m_ccpsxFinal += "-o";
            stages.m_ccpsxFinal += Path.Combine(buildDir, "main.cpe") + ",";
            stages.m_ccpsxFinal += Path.Combine(buildDir, "main.sym") + ",";
            stages.m_ccpsxFinal += Path.Combine(buildDir, "mem.map");

            return stages;
        }

        /// <summary>
        /// Clean the generated files for the specified configuration.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        public void Clean(BuildConfiguration config, StringBuilder output)
        {
            output.Clear();
            output.AppendLine("Cleaning output...");

            string psxdevRoot = Utilities.GetPsxDevRoot();
            string dir = Path.Combine(psxdevRoot, Utilities.GetBuildOuputDirectory(config, m_projectDirectory));
            if (Directory.Exists(dir))
            {                
                output.AppendLine("Deleting directory " + dir + "...");
                try
                {
                    Directory.Delete(dir, true);
                    output.AppendLine("Success!");
                }
                catch (IOException exc)
                {
                    output.AppendLine("Output being used by another process!");
                }                
            }                        
        }

        /// <summary>
        /// Starts a project build for the specified configuration, adding any extra preprocessor or linker options.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="additionalPreprocessor">An array of preprocessor names.</param>
        /// <param name="additionalLinker">An array of linker settings.</param>
        /// <param name="additionalLibDirs">An array of library directories.</param>
        /// <param name="additionalIncludeDirs">An array of include directories.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        public bool Build(BuildConfiguration config, string[] additionalPreprocessor, string[] additionalLinker, string[] additionalLibDirs, string[] additionalIncludeDirs, StringBuilder output)
        {
            output.Clear();
            output.AppendLine("Starting " + config.ToString() + " build...");

            string psxdevRoot = Utilities.GetPsxDevRoot();

            // Create the output directories, if needed
            string outputDir = Path.Combine(psxdevRoot, Utilities.GetBuildOuputDirectory(config, m_projectDirectory));
            if (!Directory.Exists(outputDir))
            {
                output.AppendLine("Output directory doesn't exist, creating " + outputDir + "...");
                Directory.CreateDirectory(outputDir);
                output.AppendLine("Success!");
            }

            // First, the build process.
            bool success = true;
            {                
                // CCPSX
                {                    
                    CompilationStages stages = GetCCPSXArgs(config, additionalPreprocessor, additionalLinker, additionalLibDirs, additionalIncludeDirs);

                    output.AppendLine("Launching CCPSX (engine)...");
                    success &= Utilities.LaunchProcessAndWait("ccpsx", stages.m_ccpsxEngine, psxdevRoot, output);
                    output.AppendLine("Launching DMPSX (engine)...");
                    success &= Utilities.LaunchProcessAndWait("dmpsx", stages.m_dmpsxEngine, psxdevRoot, output);
                    output.AppendLine("Launching CCPSX (game)...");
                    success &= Utilities.LaunchProcessAndWait("ccpsx", stages.m_ccpsxGame, psxdevRoot, output);
                    output.AppendLine("Launching DMPSX (game)...");
                    success &= Utilities.LaunchProcessAndWait("dmpsx", stages.m_dmpsxGame, psxdevRoot, output);
                    output.AppendLine("Launching CCPSX (final compilation & linking)...");
                    success &= Utilities.LaunchProcessAndWait("ccpsx", stages.m_ccpsxFinal, psxdevRoot, output);
                }

                // CPE2X
                {
                    output.AppendLine("Launching CPE2X...");
                    success &= Utilities.LaunchProcessAndWait("cpe2x", "main.cpe", outputDir, output);
                }
            }

            // Then, make the CD image
            // This is useful for NO$PSX, as it seems that's the only way it can load PSX executable at the moment (unless of course we want to burn the image to a CD).
            // First, read the templates (system & cd) and make a copy of them in the output directory. 
            // Modify their placeholder values and add any files to the tracks.
            if (GenerateCDImage && success)
            {
                string cdOutputDir = Path.Combine(outputDir, "cdrom");
                Directory.CreateDirectory(cdOutputDir);

                // Generate the cd xml for this project.
                {
                    const string kProjectNameStr = "PROJECTNAME_PLACEHOLDER";
                    const string kPublisherNameStr = "PUBLISHERNAME_PLACEHOLDER";
                    const string kLicenseStr = "LICENSE_PLACEHOLDER";

                    // Pick license from region
                    string licenseFile = "";
                    switch (CDLicenseRegion)
                    {
                        case CDLicense.Europe:
                            licenseFile = "LICENSEE.DAT";
                            break;
                        case CDLicense.Americas:
                            licenseFile = "LICENSEA.DAT";
                                break;
                        case CDLicense.Japan:
                            licenseFile = "LICENSEJ.DAT";
                            break;
                        default: break;
                    }

                    string licenseSource = Path.Combine(psxdevRoot, "psyq", "cdgen", "LCNSFILE", licenseFile);

                    string cdTemplateContents = File.ReadAllText(Path.Combine(psxdevRoot, "cdrom", "cd_template.xml"));
                    cdTemplateContents = cdTemplateContents.Replace(kProjectNameStr, ProjectName);
                    cdTemplateContents = cdTemplateContents.Replace(kLicenseStr, licenseSource);

                    File.WriteAllText(Path.Combine(cdOutputDir, "cd.xml"), cdTemplateContents);
                }

                // Copy the system file.
                {                    
                    string srcSystemFilename = Path.Combine(psxdevRoot, "cdrom", "system_template.txt");
                    string dstSystemFilename = Path.Combine(cdOutputDir, "system.txt");
                    File.Copy(srcSystemFilename, dstSystemFilename, true);
                }

                // Now we can build the CD image, the MKPSXISO is used for this.
                {
                    output.AppendLine("Launching MKPSXISO...");

                    string args = "cdrom/cd.xml -y";
                    Utilities.LaunchProcessAndWait("mkpsxiso", args, outputDir, output);
                }
            }
            if (!success)
            {
                MessageBox.Show("Unsuccessful compilation, has compile/linker errors! Execution (if requested) has been skipped.", "BuildTool", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return success;
        }

        /// <summary>
        /// Starts a project build for the specified configuration, adding any extra preprocessor or linker options. After the build succeeds, the
        /// PSX executable will be launched on either the connected PSX (if any) or the NO$PSX emulator.
        /// In the case of a EMU config, a full CD image will be generated.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="additionaPreprocessor">An array of preprocessor names.</param>
        /// <param name="additionalLinker">An array of linker settings.</param>
        /// <param name="additionalLibDirs">An array of library directories.</param>
        /// <param name="additionalIncludeDirs">An array of include directories.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        public void BuildAndRun(BuildConfiguration config, string[] additionaPreprocessor, string[] additionalLinker, string[] additionalLibDirs, string[] additionalIncludeDirs, StringBuilder output)
        {
            bool success = Build(config, additionaPreprocessor, additionalLinker, additionalLibDirs, additionalIncludeDirs, output);

            if (success)
            {
                m_preserveOutput = true;
                Run(config, output);
                m_preserveOutput = false;
            }
        }

        /// <summary>
        /// Launches the previously built PSX executableon either the connected PSX (if any) or the NO$PSX emulator.
        /// If there is no output for the specified configuration, the launch will fail, no re-build will be started.
        /// </summary>
        /// <param name="config">The build configuration.</param>
        /// <param name="output">The StringBuilder to write out the stdout and stderr streams.</param>
        public void Run(BuildConfiguration config, StringBuilder output)
        {
            if (!m_preserveOutput)
            {
                output.Clear();
            }

            string psxdevRoot = Utilities.GetPsxDevRoot();

            string outputDir = Path.Combine(psxdevRoot, Utilities.GetBuildOuputDirectory(config, m_projectDirectory));

            if (Utilities.IsPSXConfig(config))
            {
                // Catflap
                output.AppendLine("Launching on PSX using catflap...");
                Utilities.LaunchProcess("catflap", "run main.exe", outputDir, output);
            }
            else if (Utilities.IsEMUConfig(config))
            {
                // NO$PSX
                output.AppendLine("Launching on NO$PSX emulator...");
                bool success = Utilities.LaunchProcess("no$psx", "cdrom/" + ProjectName + ".cue", outputDir, output);
                output.AppendLine(success ? "Success!" : "Failed (build unavailable?)");
            }
        }
    }
}
