using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace BuildTool
{
    public partial class Form1 : Form
    {
        private Builder m_currentBuilder;
        private List<string> m_projectDirectoryNames;
        private string m_currentProjectName;
        private StringBuilder m_outputStringBuilder = new StringBuilder();
        private FileSystemWatcher m_watcher = new FileSystemWatcher();

        private bool[] m_outputAvailability = new bool[(int)BuildConfiguration.ConfigCount + 1];
        private bool[] m_cdImageAvailability = new bool[(int)BuildConfiguration.ConfigCount + 1];

        private BuildConfiguration m_configuration;

        private const string kNoProjectsStr = "<no projects found>";
        public Form1()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Disables the build pane. This is typically if no projects are found.
        /// </summary>
        private void DisableBuildPane()
        {
            grpBuildSettings.Enabled = false;
        }

        /// <summary>
        /// Enables the build pane.
        /// </summary>
        private void EnableBuildPane()
        {
            grpBuildSettings.Enabled = true;
        }

        /// <summary>
        /// Go through the configs for the current project and see which outputs are built.
        /// Expected is at least to find the main.exe in the output directory.
        /// </summary>
        private void UpdateOutputAvailability()
        {
            if (m_currentProjectName != null && m_currentProjectName != kNoProjectsStr)
            {
                for (int i = 0; i < (int)BuildConfiguration.ConfigCount+1; ++i)
                {
                    BuildConfiguration config = (BuildConfiguration)i;
                    string path = Path.Combine(Utilities.GetPsxDevRoot(), "projects", Utilities.GetBuildOuputDirectory((BuildConfiguration)i, m_currentProjectName));

                    m_outputAvailability[i] = File.Exists(Path.Combine(path, "main.exe"));
                    m_cdImageAvailability[i] = m_outputAvailability[i];
                    
                    // On EMU, also check for generated .bin and .cue
                    if (Utilities.IsEMUConfig(config))
                    {
                        string cdBin = Path.Combine(path, "cdrom", m_currentProjectName + ".bin");
                        string cdCue = Path.Combine(path, "cdrom", m_currentProjectName + ".cue");

                        m_cdImageAvailability[i] &= File.Exists(cdBin);
                        m_cdImageAvailability[i] &= File.Exists(cdCue);
                    }
                }
            }
        }

        /// <summary>
        /// Scan the project directory.
        /// </summary>
        private void ScanProjects()
        {
            // Retrieve the PSXDEV_PATH, this is our development root directory.
            string psxdevPath = Utilities.GetPsxDevRoot();
            string projectsPath = Path.Combine(psxdevPath, "projects");

            m_projectDirectoryNames = new List<string>();

            if (Directory.Exists(projectsPath))
            {
                m_projectDirectoryNames.AddRange (System.IO.Directory.EnumerateDirectories(projectsPath));

                // Go through the found project paths and extract only the directory name. This is our project directory.
                for (int i = 0; i < m_projectDirectoryNames.Count; ++i)
                {
                    m_projectDirectoryNames[i] = Path.GetFileName(m_projectDirectoryNames[i]);
                }
            }

            // If the current project name is invalid, get the first available one.
            if ((m_currentProjectName == null) || (!m_projectDirectoryNames.Contains(m_currentProjectName)))
            {
                if (m_projectDirectoryNames.Count > 0)
                {
                    m_currentProjectName = m_projectDirectoryNames[0];
                }
                else
                {
                    m_projectDirectoryNames.Add(kNoProjectsStr);
                    m_currentProjectName = kNoProjectsStr;
                }
            }

            // Clear and append all the found names in the combobox list.
            cmbProjectName.Items.Clear();
            cmbProjectName.Items.AddRange(m_projectDirectoryNames.ToArray());

            cmbProjectName.SelectedItem = m_currentProjectName;

            // Enable/disable build support
            if (m_currentProjectName == kNoProjectsStr)
            {
                DisableBuildPane();
            }
            else
            {
                EnableBuildPane();
            }
        }

        private void FileWatcher_OnChanged(object sender, FileSystemEventArgs e)
        {
            UpdateOutputAvailability();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Default to DebugEMU
            cmbConfiguration.SelectedIndex = (int)BuildConfiguration.DebugEMU;

            // Default to Europe license
            cmbCDLicense.SelectedIndex = (int)CDLicense.Europe;

            ScanProjects();
            Utilities.PollLPTAvailability();

            // Setup the watcher to check for directory changes
            if (Directory.Exists(m_watcher.Path))
            {
                m_watcher.NotifyFilter = NotifyFilters.DirectoryName | NotifyFilters.FileName;
                m_watcher.IncludeSubdirectories = true;

                m_watcher.Created += new FileSystemEventHandler(FileWatcher_OnChanged);
                m_watcher.Deleted += new FileSystemEventHandler(FileWatcher_OnChanged);
                m_watcher.EnableRaisingEvents = true;
            }
            UpdateOutputAvailability();
        }

        private void cmbProjectName_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Update the current builder
            if (m_currentProjectName != kNoProjectsStr)
            {
                m_currentBuilder = new Builder(Path.Combine("projects", m_currentProjectName));
                m_watcher.Path = Path.Combine(Utilities.GetPsxDevRoot(), "projects", m_currentProjectName);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ScanProjects();
        }

        /// <summary>
        /// Split the source on the specified delimiter, trim the individual tokens and update the output array.
        /// </summary>
        /// <param name="source">The input string to split.</param>
        /// <param name="delimiter">The delimiter character/</param>
        /// <param name="output">The output array, containing any non empty tokens.</param>
        private void ParseAdditionalOptions(string source, char delimiter, ref string[] output)
        {
            List<string> temp = new List<string>();
            output = source.Split(delimiter);
            for (int i = 0; i < output.Length; ++i)
            {
                output[i] = output[i].Trim();
                if (!string.IsNullOrEmpty(output[i]))
                {
                    temp.Add(output[i]);
                }
            }
            output = temp.ToArray();
        }

        private void PreBuild(ref string[] additionalPreprocessor, ref string[] additionalLinker, ref string[] additionalLibDirs, ref string[] additionalIncludeDirs)
        {            
            if (m_currentBuilder != null)
            {
                // Update license and cd image generation
                m_currentBuilder.CDLicenseRegion = (CDLicense)cmbCDLicense.SelectedIndex;
                m_currentBuilder.GenerateCDImage = chkGenerateCD.Checked;

                // Split the preprocessor definitions and add them to the output
                // Valid preprocessor formats:
                // 1) MACRO_NAME
                // 2) MACRO_NAME=VALUE
                ParseAdditionalOptions(txtAdditionalPreprocessor.Text, ';', ref additionalPreprocessor);

                // Split the linker options and add them to the output
                ParseAdditionalOptions(txtAdditionalLinker.Text, ';', ref additionalLinker);

                // Split the library directories and add them to the output
                ParseAdditionalOptions(txtAdditionalLibDirs.Text, ';', ref additionalLibDirs);

                // Split the include directories and add them to the output
                ParseAdditionalOptions(txtAdditionalIncDirs.Text, ';', ref additionalIncludeDirs);
            }
        }

        private void btnBuild_Click_1(object sender, EventArgs e)
        {
            string[] additionalPreprocessor = null;
            string[] additionalLinker = null;
            string[] additionalLibDirs = null;
            string[] additionalIncludeDirs = null;

            PreBuild(ref additionalPreprocessor, ref additionalLinker, ref additionalLibDirs, ref additionalIncludeDirs);

            m_currentBuilder.Build(m_configuration, additionalPreprocessor, additionalLinker, additionalLibDirs, additionalIncludeDirs, m_outputStringBuilder);
        }

        private void btnBuildAndRun_Click_1(object sender, EventArgs e)
        {
            string[] additionalPreprocessor = null;
            string[] additionalLinker = null;
            string[] additionalLibDirs = null;
            string[] additionalIncludeDirs = null;

            PreBuild(ref additionalPreprocessor, ref additionalLinker, ref additionalLibDirs, ref additionalIncludeDirs);

            m_currentBuilder.BuildAndRun(m_configuration, additionalPreprocessor, additionalLinker, additionalLibDirs, additionalIncludeDirs, m_outputStringBuilder);
        }

        private void btnRun_Click_1(object sender, EventArgs e)
        {
            m_currentBuilder.Run(m_configuration, m_outputStringBuilder);
        }

        private void btnKillProcess_Click(object sender, EventArgs e)
        {
            Process[] processes = null;

            // For EMU, find NO$PSX and kill it
            if (Utilities.IsEMUConfig(m_configuration))
            {
                processes = Process.GetProcessesByName("no$psx");
                 
                foreach (var process in processes)
                {
                    process.Kill();
                }
            }
            // For PSX, find catflap, send the restart command and then kill catflap
            else
            {
                // Kill any existing catflap processes:
                processes = Process.GetProcessesByName("catflap");
                foreach (var process in processes)
                {
                    process.Kill();
                }

                // Run a new catflap process, reset the PSX:
                Utilities.LaunchProcessAndWait("catflap", "reset", Utilities.GetPsxDevRoot(), m_outputStringBuilder);
            }

            if ((processes == null) || (processes.Length == 0))
            {
                MessageBox.Show("Didn't find any processes to kill!", "BuildTool", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void btnClean_Click_1(object sender, EventArgs e)
        {
            m_currentBuilder.Clean(m_configuration, m_outputStringBuilder);
        }

        private void timUpdateOutput_Tick(object sender, EventArgs e)
        {
            // Update build output
            txtOutput.Text = m_outputStringBuilder.ToString();

            // Update button state
            btnClean.Enabled = m_outputAvailability[(int)m_configuration];
            btnRun.Enabled = m_cdImageAvailability[(int)m_configuration]; ;
        }

        private void cmbConfiguration_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_configuration = (BuildConfiguration)cmbConfiguration.SelectedIndex;
            chkGenerateCD.Checked = Utilities.IsEMUConfig(m_configuration);

            UpdateOutputAvailability();
        }
    }
}
