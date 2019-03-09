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
    public partial class DataView : Form
    {
        private Builder m_builder;

        public DataView(Builder builder)
        {
            InitializeComponent();

            m_builder = builder;
            InitListView();
            UpdateTreeView();
        }

        private void fileSystemWatcher1_Changed(object sender, System.IO.FileSystemEventArgs e)
        {

        }

        private void GetDirectories(DirectoryInfo[] subDirs, TreeNode nodeToAddTo)
        {
            TreeNode aNode;
            DirectoryInfo[] subSubDirs;
            foreach (DirectoryInfo subDir in subDirs)
            {
                aNode = new TreeNode(subDir.Name, 0, 0);
                aNode.Tag = subDir;
                aNode.ImageKey = "folder";
                subSubDirs = subDir.GetDirectories();
                if (subSubDirs.Length != 0)
                {
                    GetDirectories(subSubDirs, aNode);
                }
                nodeToAddTo.Nodes.Add(aNode);
            }
        }

        /// <summary>
        /// Initialize the list view's data.
        /// </summary>
        private void InitListView()
        {
            
        }

        /// <summary>
        /// Update the tree view with directory contents.
        /// </summary>
        private void UpdateTreeView()
        {
            TreeNode rootNode;

            string dataDirectory = Path.Combine(Utilities.GetPsxDevRoot(), Utilities.GetBuildDataDirectory(m_builder.ProjectDirectory));
            DirectoryInfo info = new DirectoryInfo(dataDirectory);
            if (info.Exists)
            {
                rootNode = new TreeNode(info.Name);
                rootNode.Tag = info;
                GetDirectories(info.GetDirectories(), rootNode);
                treeLeft.Nodes.Add(rootNode);
            }
        }

        private void treeLeft_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void treeLeft_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode newSelected = e.Node;
            listRight.Items.Clear();
            DirectoryInfo nodeDirInfo = (DirectoryInfo)newSelected.Tag;
            ListViewItem.ListViewSubItem[] subItems;
            ListViewItem item = null;

            foreach (DirectoryInfo dir in nodeDirInfo.GetDirectories())
            {
                item = new ListViewItem(dir.Name, 0);
                subItems = new ListViewItem.ListViewSubItem[]
                    {new ListViewItem.ListViewSubItem(item, "Directory"),
             new ListViewItem.ListViewSubItem(item,
                dir.LastAccessTime.ToShortDateString())};
                item.SubItems.AddRange(subItems);
                listRight.Items.Add(item);
            }
            foreach (FileInfo file in nodeDirInfo.GetFiles())
            {
                item = new ListViewItem(file.Name, 1);
                subItems = new ListViewItem.ListViewSubItem[]
                    { new ListViewItem.ListViewSubItem(item, "File"),
             new ListViewItem.ListViewSubItem(item,
                file.LastAccessTime.ToShortDateString())};

                item.SubItems.AddRange(subItems);
                listRight.Items.Add(item);
            }

            listRight.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);
        }
    }
}
