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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cmbProjectName = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbConfiguration = new System.Windows.Forms.ComboBox();
            this.btnClean = new System.Windows.Forms.Button();
            this.btnBuild = new System.Windows.Forms.Button();
            this.btnBuildAndRun = new System.Windows.Forms.Button();
            this.btnRun = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cmbConfiguration);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbProjectName);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(662, 62);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Build Target";
            // 
            // cmbProjectName
            // 
            this.cmbProjectName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProjectName.FormattingEnabled = true;
            this.cmbProjectName.Location = new System.Drawing.Point(66, 25);
            this.cmbProjectName.Name = "cmbProjectName";
            this.cmbProjectName.Size = new System.Drawing.Size(264, 21);
            this.cmbProjectName.TabIndex = 3;
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
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(351, 28);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Configuration:";
            // 
            // cmbConfiguration
            // 
            this.cmbConfiguration.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbConfiguration.FormattingEnabled = true;
            this.cmbConfiguration.Location = new System.Drawing.Point(429, 25);
            this.cmbConfiguration.Name = "cmbConfiguration";
            this.cmbConfiguration.Size = new System.Drawing.Size(207, 21);
            this.cmbConfiguration.TabIndex = 5;
            // 
            // btnClean
            // 
            this.btnClean.Location = new System.Drawing.Point(73, 80);
            this.btnClean.Name = "btnClean";
            this.btnClean.Size = new System.Drawing.Size(103, 26);
            this.btnClean.TabIndex = 3;
            this.btnClean.Text = "&Clean";
            this.btnClean.UseVisualStyleBackColor = true;
            // 
            // btnBuild
            // 
            this.btnBuild.Location = new System.Drawing.Point(182, 80);
            this.btnBuild.Name = "btnBuild";
            this.btnBuild.Size = new System.Drawing.Size(103, 26);
            this.btnBuild.TabIndex = 4;
            this.btnBuild.Text = "&Build";
            this.btnBuild.UseVisualStyleBackColor = true;
            // 
            // btnBuildAndRun
            // 
            this.btnBuildAndRun.Location = new System.Drawing.Point(291, 80);
            this.btnBuildAndRun.Name = "btnBuildAndRun";
            this.btnBuildAndRun.Size = new System.Drawing.Size(103, 26);
            this.btnBuildAndRun.TabIndex = 5;
            this.btnBuildAndRun.Text = "Build and &run";
            this.btnBuildAndRun.UseVisualStyleBackColor = true;
            // 
            // btnRun
            // 
            this.btnRun.Location = new System.Drawing.Point(400, 80);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(103, 26);
            this.btnRun.TabIndex = 6;
            this.btnRun.Text = "Run";
            this.btnRun.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(509, 80);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(103, 26);
            this.button1.TabIndex = 7;
            this.button1.Text = "&Kill Process";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(690, 120);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btnRun);
            this.Controls.Add(this.btnBuildAndRun);
            this.Controls.Add(this.btnBuild);
            this.Controls.Add(this.btnClean);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "BuildTool";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox cmbConfiguration;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbProjectName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnClean;
        private System.Windows.Forms.Button btnBuild;
        private System.Windows.Forms.Button btnBuildAndRun;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.Button button1;
    }
}

