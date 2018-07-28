namespace BuildTool
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button2 = new System.Windows.Forms.Button();
            this.cmbConfiguration = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbProjectName = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.grpBuildSettings = new System.Windows.Forms.GroupBox();
            this.txtAdditionalIncDirs = new System.Windows.Forms.TextBox();
            this.txtAdditionalLibDirs = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.btnKillProcess = new System.Windows.Forms.Button();
            this.btnRun = new System.Windows.Forms.Button();
            this.btnBuildAndRun = new System.Windows.Forms.Button();
            this.btnBuild = new System.Windows.Forms.Button();
            this.btnClean = new System.Windows.Forms.Button();
            this.txtAdditionalLinker = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtAdditionalPreprocessor = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.timUpdateOutput = new System.Windows.Forms.Timer(this.components);
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.txtOutput = new System.Windows.Forms.RichTextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.chkGenerateCD = new System.Windows.Forms.CheckBox();
            this.cmbCDLicense = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox1.SuspendLayout();
            this.grpBuildSettings.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button2);
            this.groupBox1.Controls.Add(this.cmbConfiguration);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbProjectName);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 97);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(662, 62);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Build Target";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(307, 25);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(58, 21);
            this.button2.TabIndex = 6;
            this.button2.Text = "Refresh";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // cmbConfiguration
            // 
            this.cmbConfiguration.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbConfiguration.FormattingEnabled = true;
            this.cmbConfiguration.Items.AddRange(new object[] {
            "Debug EMU",
            "Release EMU",
            "Debug PSX",
            "Release PSX"});
            this.cmbConfiguration.Location = new System.Drawing.Point(455, 25);
            this.cmbConfiguration.Name = "cmbConfiguration";
            this.cmbConfiguration.Size = new System.Drawing.Size(181, 21);
            this.cmbConfiguration.TabIndex = 5;
            this.cmbConfiguration.SelectedIndexChanged += new System.EventHandler(this.cmbConfiguration_SelectedIndexChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(379, 28);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Configuration:";
            // 
            // cmbProjectName
            // 
            this.cmbProjectName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProjectName.FormattingEnabled = true;
            this.cmbProjectName.Location = new System.Drawing.Point(66, 25);
            this.cmbProjectName.Name = "cmbProjectName";
            this.cmbProjectName.Size = new System.Drawing.Size(240, 21);
            this.cmbProjectName.TabIndex = 3;
            this.cmbProjectName.SelectedIndexChanged += new System.EventHandler(this.cmbProjectName_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 28);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Project:";
            // 
            // grpBuildSettings
            // 
            this.grpBuildSettings.Controls.Add(this.txtAdditionalIncDirs);
            this.grpBuildSettings.Controls.Add(this.txtAdditionalLibDirs);
            this.grpBuildSettings.Controls.Add(this.label7);
            this.grpBuildSettings.Controls.Add(this.label6);
            this.grpBuildSettings.Controls.Add(this.btnKillProcess);
            this.grpBuildSettings.Controls.Add(this.btnRun);
            this.grpBuildSettings.Controls.Add(this.btnBuildAndRun);
            this.grpBuildSettings.Controls.Add(this.btnBuild);
            this.grpBuildSettings.Controls.Add(this.btnClean);
            this.grpBuildSettings.Controls.Add(this.txtAdditionalLinker);
            this.grpBuildSettings.Controls.Add(this.label4);
            this.grpBuildSettings.Controls.Add(this.txtAdditionalPreprocessor);
            this.grpBuildSettings.Controls.Add(this.label3);
            this.grpBuildSettings.Location = new System.Drawing.Point(12, 169);
            this.grpBuildSettings.Name = "grpBuildSettings";
            this.grpBuildSettings.Size = new System.Drawing.Size(662, 184);
            this.grpBuildSettings.TabIndex = 8;
            this.grpBuildSettings.TabStop = false;
            this.grpBuildSettings.Text = "Build Settings";
            // 
            // txtAdditionalIncDirs
            // 
            this.txtAdditionalIncDirs.Location = new System.Drawing.Point(164, 123);
            this.txtAdditionalIncDirs.Name = "txtAdditionalIncDirs";
            this.txtAdditionalIncDirs.Size = new System.Drawing.Size(472, 20);
            this.txtAdditionalIncDirs.TabIndex = 18;
            this.txtAdditionalIncDirs.TextChanged += new System.EventHandler(this.txtAdditionalIncDirs_TextChanged);
            // 
            // txtAdditionalLibDirs
            // 
            this.txtAdditionalLibDirs.Location = new System.Drawing.Point(164, 97);
            this.txtAdditionalLibDirs.Name = "txtAdditionalLibDirs";
            this.txtAdditionalLibDirs.Size = new System.Drawing.Size(472, 20);
            this.txtAdditionalLibDirs.TabIndex = 17;
            this.txtAdditionalLibDirs.TextChanged += new System.EventHandler(this.txtAdditionalLibDirs_TextChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(17, 126);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(144, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "Additional include directories:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(38, 100);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(120, 13);
            this.label6.TabIndex = 15;
            this.label6.Text = "Additional lib directories:";
            // 
            // btnKillProcess
            // 
            this.btnKillProcess.Location = new System.Drawing.Point(491, 152);
            this.btnKillProcess.Name = "btnKillProcess";
            this.btnKillProcess.Size = new System.Drawing.Size(103, 26);
            this.btnKillProcess.TabIndex = 14;
            this.btnKillProcess.Text = "&Kill Process";
            this.btnKillProcess.UseVisualStyleBackColor = true;
            this.btnKillProcess.Click += new System.EventHandler(this.btnKillProcess_Click);
            // 
            // btnRun
            // 
            this.btnRun.Location = new System.Drawing.Point(382, 152);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(103, 26);
            this.btnRun.TabIndex = 13;
            this.btnRun.Text = "Run";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click_1);
            // 
            // btnBuildAndRun
            // 
            this.btnBuildAndRun.Location = new System.Drawing.Point(273, 152);
            this.btnBuildAndRun.Name = "btnBuildAndRun";
            this.btnBuildAndRun.Size = new System.Drawing.Size(103, 26);
            this.btnBuildAndRun.TabIndex = 12;
            this.btnBuildAndRun.Text = "Build and &run";
            this.btnBuildAndRun.UseVisualStyleBackColor = true;
            this.btnBuildAndRun.Click += new System.EventHandler(this.btnBuildAndRun_Click_1);
            // 
            // btnBuild
            // 
            this.btnBuild.Location = new System.Drawing.Point(164, 152);
            this.btnBuild.Name = "btnBuild";
            this.btnBuild.Size = new System.Drawing.Size(103, 26);
            this.btnBuild.TabIndex = 11;
            this.btnBuild.Text = "&Build";
            this.btnBuild.UseVisualStyleBackColor = true;
            this.btnBuild.Click += new System.EventHandler(this.btnBuild_Click_1);
            // 
            // btnClean
            // 
            this.btnClean.Location = new System.Drawing.Point(55, 152);
            this.btnClean.Name = "btnClean";
            this.btnClean.Size = new System.Drawing.Size(103, 26);
            this.btnClean.TabIndex = 10;
            this.btnClean.Text = "&Clean";
            this.btnClean.UseVisualStyleBackColor = true;
            this.btnClean.Click += new System.EventHandler(this.btnClean_Click_1);
            // 
            // txtAdditionalLinker
            // 
            this.txtAdditionalLinker.Location = new System.Drawing.Point(164, 53);
            this.txtAdditionalLinker.Name = "txtAdditionalLinker";
            this.txtAdditionalLinker.Size = new System.Drawing.Size(472, 20);
            this.txtAdditionalLinker.TabIndex = 9;
            this.txtAdditionalLinker.TextChanged += new System.EventHandler(this.txtAdditionalLinker_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(74, 56);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(84, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Additional linker:";
            // 
            // txtAdditionalPreprocessor
            // 
            this.txtAdditionalPreprocessor.Location = new System.Drawing.Point(164, 27);
            this.txtAdditionalPreprocessor.Name = "txtAdditionalPreprocessor";
            this.txtAdditionalPreprocessor.Size = new System.Drawing.Size(472, 20);
            this.txtAdditionalPreprocessor.TabIndex = 7;
            this.txtAdditionalPreprocessor.TextChanged += new System.EventHandler(this.txtAdditionalPreprocessor_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(38, 30);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(120, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Additional preprocessor:";
            // 
            // timUpdateOutput
            // 
            this.timUpdateOutput.Enabled = true;
            this.timUpdateOutput.Interval = 10;
            this.timUpdateOutput.Tick += new System.EventHandler(this.timUpdateOutput_Tick);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.txtOutput);
            this.groupBox2.Location = new System.Drawing.Point(17, 431);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(661, 211);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Build Output";
            // 
            // txtOutput
            // 
            this.txtOutput.Location = new System.Drawing.Point(20, 20);
            this.txtOutput.Name = "txtOutput";
            this.txtOutput.ReadOnly = true;
            this.txtOutput.Size = new System.Drawing.Size(616, 183);
            this.txtOutput.TabIndex = 0;
            this.txtOutput.Text = "";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.chkGenerateCD);
            this.groupBox3.Controls.Add(this.cmbCDLicense);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Location = new System.Drawing.Point(17, 359);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(661, 66);
            this.groupBox3.TabIndex = 11;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "CD Settings";
            // 
            // chkGenerateCD
            // 
            this.chkGenerateCD.AutoSize = true;
            this.chkGenerateCD.Location = new System.Drawing.Point(262, 30);
            this.chkGenerateCD.Name = "chkGenerateCD";
            this.chkGenerateCD.Size = new System.Drawing.Size(244, 17);
            this.chkGenerateCD.TabIndex = 2;
            this.chkGenerateCD.Text = "Generate CD Image (Required for EMU builds)";
            this.chkGenerateCD.UseVisualStyleBackColor = true;
            this.chkGenerateCD.CheckedChanged += new System.EventHandler(this.chkGenerateCD_CheckedChanged);
            // 
            // cmbCDLicense
            // 
            this.cmbCDLicense.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCDLicense.FormattingEnabled = true;
            this.cmbCDLicense.Items.AddRange(new object[] {
            "Europe",
            "Americas",
            "Japan"});
            this.cmbCDLicense.Location = new System.Drawing.Point(92, 28);
            this.cmbCDLicense.Name = "cmbCDLicense";
            this.cmbCDLicense.Size = new System.Drawing.Size(133, 21);
            this.cmbCDLicense.TabIndex = 1;
            this.cmbCDLicense.SelectedIndexChanged += new System.EventHandler(this.cmbCDLicense_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 31);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(79, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "License region:";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.Color.White;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.InitialImage = null;
            this.pictureBox1.Location = new System.Drawing.Point(-3, 1);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(697, 94);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 12;
            this.pictureBox1.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(690, 642);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.grpBuildSettings);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "BuildTool";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.grpBuildSettings.ResumeLayout(false);
            this.grpBuildSettings.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox cmbConfiguration;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbProjectName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox grpBuildSettings;
        private System.Windows.Forms.TextBox txtAdditionalLinker;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtAdditionalPreprocessor;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button btnKillProcess;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.Button btnBuildAndRun;
        private System.Windows.Forms.Button btnBuild;
        private System.Windows.Forms.Button btnClean;
        private System.Windows.Forms.Timer timUpdateOutput;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox cmbCDLicense;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox chkGenerateCD;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.RichTextBox txtOutput;
        private System.Windows.Forms.TextBox txtAdditionalIncDirs;
        private System.Windows.Forms.TextBox txtAdditionalLibDirs;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
    }
}

