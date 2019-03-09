namespace BuildTool
{
    partial class DataView
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DataView));
            this.fileSystemWatcher1 = new System.IO.FileSystemWatcher();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treeLeft = new System.Windows.Forms.TreeView();
            this.listRight = new System.Windows.Forms.ListView();
            this.headerName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.headerType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.headerLastModified = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // fileSystemWatcher1
            // 
            this.fileSystemWatcher1.EnableRaisingEvents = true;
            this.fileSystemWatcher1.SynchronizingObject = this;
            this.fileSystemWatcher1.Changed += new System.IO.FileSystemEventHandler(this.fileSystemWatcher1_Changed);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "folder.png");
            this.imageList1.Images.SetKeyName(1, "document.png");
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.splitContainer1);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(811, 410);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Project data";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(3, 16);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeLeft);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listRight);
            this.splitContainer1.Size = new System.Drawing.Size(805, 391);
            this.splitContainer1.SplitterDistance = 268;
            this.splitContainer1.TabIndex = 1;
            // 
            // treeLeft
            // 
            this.treeLeft.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeLeft.ImageIndex = 0;
            this.treeLeft.ImageList = this.imageList1;
            this.treeLeft.Location = new System.Drawing.Point(0, 0);
            this.treeLeft.Name = "treeLeft";
            this.treeLeft.SelectedImageIndex = 0;
            this.treeLeft.Size = new System.Drawing.Size(268, 391);
            this.treeLeft.TabIndex = 0;
            this.treeLeft.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeLeft_NodeMouseClick);
            // 
            // listRight
            // 
            this.listRight.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.headerName,
            this.headerType,
            this.headerLastModified});
            this.listRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listRight.Location = new System.Drawing.Point(0, 0);
            this.listRight.Name = "listRight";
            this.listRight.Size = new System.Drawing.Size(533, 391);
            this.listRight.SmallImageList = this.imageList1;
            this.listRight.TabIndex = 0;
            this.listRight.UseCompatibleStateImageBehavior = false;
            this.listRight.View = System.Windows.Forms.View.Details;
            // 
            // headerName
            // 
            this.headerName.Text = "Name";
            // 
            // headerType
            // 
            this.headerType.Text = "Type";
            // 
            // headerLastModified
            // 
            this.headerLastModified.Text = "Last Modified";
            // 
            // DataView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(835, 474);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "DataView";
            this.Text = "DataView";
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.IO.FileSystemWatcher fileSystemWatcher1;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView treeLeft;
        private System.Windows.Forms.ListView listRight;
        private System.Windows.Forms.ColumnHeader headerName;
        private System.Windows.Forms.ColumnHeader headerType;
        private System.Windows.Forms.ColumnHeader headerLastModified;
    }
}