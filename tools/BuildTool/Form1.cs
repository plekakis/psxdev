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

namespace BuildTool
{
    public partial class Form1 : Form
    {
        private Builder m_currentBuilder;
        private List<string> m_projectDirectoryNames;
        private string m_currentProjectName;
        private StringBuilder m_outputStringBuilder = new StringBuilder();

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
        /// Scan the project directory.
        /// </summary>
        private void ScanProjects()
        {
            // Retrieve the PSXDEV_PATH, this is our development root directory.
            string psxdevPath = Utilities.GetPsxDevRoot();
            m_projectDirectoryNames = new List<string>(System.IO.Directory.EnumerateDirectories(Path.Combine(psxdevPath, "projects")));

            // Go through the found project paths and extract only the directory name. This is our project directory.
            for (int i=0; i<m_projectDirectoryNames.Count; ++i)
            {
                m_projectDirectoryNames[i] = Path.GetFileName(m_projectDirectoryNames[i]);
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

        private void Form1_Load(object sender, EventArgs e)
        {
            // Default to DebugEMU
            cmbConfiguration.SelectedIndex = (int)BuildConfiguration.DebugEMU;

            // Default to Europe license
            cmbCDLicense.SelectedIndex = (int)CDLicense.Europe;

            ScanProjects();
            Utilities.PollLPTAvailability();
        }

        private void cmbProjectName_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Update the current builder
            if (m_currentProjectName != kNoProjectsStr)
            {
                m_currentBuilder = new Builder(Path.Combine("projects", m_currentProjectName), (CDLicense)cmbCDLicense.SelectedIndex, chkGenerateCD.Checked);
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

            m_currentBuilder.Build((BuildConfiguration)cmbConfiguration.SelectedIndex, additionalPreprocessor, additionalLinker, m_outputStringBuilder);
        }

        private void btnBuildAndRun_Click_1(object sender, EventArgs e)
        {

        }

        private void btnRun_Click_1(object sender, EventArgs e)
        {
            m_currentBuilder.Run((BuildConfiguration)cmbConfiguration.SelectedIndex, m_outputStringBuilder);
        }

        private void btnKillProcess_Click(object sender, EventArgs e)
        {
            
        }

        private void btnClean_Click_1(object sender, EventArgs e)
        {
            m_currentBuilder.Clean((BuildConfiguration)cmbConfiguration.SelectedIndex, m_outputStringBuilder);
        }

        private void timUpdateOutput_Tick(object sender, EventArgs e)
        {
            txtOutput.Text = m_outputStringBuilder.ToString();
        }

        private void cmbConfiguration_SelectedIndexChanged(object sender, EventArgs e)
        {
            chkGenerateCD.Checked = Utilities.IsEMUConfig((BuildConfiguration)cmbConfiguration.SelectedIndex);
        }
    }
}
