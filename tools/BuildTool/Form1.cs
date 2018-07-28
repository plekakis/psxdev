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
                    string path = Path.Combine(Utilities.GetPsxDevRoot(), "projects", Utilities.GetBuildOuputDirectory((BuildConfiguration)i, m_currentProjectName), "main.exe");
                    m_outputAvailability[i] = File.Exists(path);
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
                m_watcher.NotifyFilter = NotifyFilters.DirectoryName;
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
                m_currentBuilder = new Builder(Path.Combine("projects", m_currentProjectName), (CDLicense)cmbCDLicense.SelectedIndex, chkGenerateCD.Checked);
                m_watcher.Path = Path.Combine(Utilities.GetPsxDevRoot(), "projects", m_currentProjectName);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ScanProjects();
        }

        private void btnBuild_Click_1(object sender, EventArgs e)
        {
            string[] additionalPreprocessor = null;
            string[] additionalLinker = null;

            m_currentBuilder.Build(m_configuration, additionalPreprocessor, additionalLinker, m_outputStringBuilder);
        }

        private void btnBuildAndRun_Click_1(object sender, EventArgs e)
        {

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
            bool avail = m_outputAvailability[(int)m_configuration];
            btnClean.Enabled = avail;
            btnRun.Enabled = avail;
        }

        private void cmbConfiguration_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_configuration = (BuildConfiguration)cmbConfiguration.SelectedIndex;
            chkGenerateCD.Checked = Utilities.IsEMUConfig(m_configuration);

            UpdateOutputAvailability();
        }
    }
}
