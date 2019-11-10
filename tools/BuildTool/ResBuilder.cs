using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace BuildTool
{
    /// <summary>
    /// Provides the ResBuilder with an array of build options.
    /// </summary>
    public class ResBuilderOptions
    {
        public string m_projectPath;
        public StringBuilder m_stringBuilder;
    }

    /// <summary>
    /// Encapsulates resource building using external tools.
    /// </summary>
    public class ResBuilder
    {
        public ResBuilder(ResBuilderOptions options)
        {
            m_options = options;
        }

        /// <summary>
        /// Get the tools path.
        /// </summary>
        private string ToolsPath
        {
            get
            {
                return Path.Combine(Utilities.GetPsxDevRoot(), "tools");
            }
        }

        /// <summary>
        /// Get the material compiler full filename.
        /// </summary>
        private string MaterialCompilerPath
        {
            get
            {
                string compiler = Path.Combine(ToolsPath, "MaterialCompiler.exe");
                return compiler;
            }
        }

        /// <summary>
        /// Get the model compiler full filename.
        /// </summary>
        private string ModelCompilerPath
        {
            get
            {
                string compiler = Path.Combine(ToolsPath, "ModelCompiler.exe");
                return compiler;
            }
        }

        /// <summary>
        /// Issue the resource build, triggering the required resource compilers.
        /// </summary>
        public void Build()
        {
            m_options.m_stringBuilder.Clear();

            BuildModels();
            BuildMaterials();
        }

        /// <summary>
        /// Trigger the MaterialCompiler, converting the Materials.xml (if found) to the data/ROOT/MATLIB.MAT file.
        /// </summary>
        private void BuildMaterials()
        {
            m_options.m_stringBuilder.Append("Starting material builder...\n");

            string workingDir = Path.Combine(Utilities.GetPsxDevRoot(), m_options.m_projectPath);
            const string srcPath = "data_source\\Materials.xml";

            if (File.Exists(Path.Combine(workingDir, srcPath)))
            {
                // This is simple;
                // - Search for the Materials.xml in the PROJECT/data_source directory.
                // - Output to PROJECT/data/ROOT/MATLIB.MAT
                string args = string.Format("--input={0} --output=data\\ROOT\\MATLIB.MAT", srcPath);
                                
                Utilities.LaunchProcessAndWait(MaterialCompilerPath, args, workingDir, m_options.m_stringBuilder);
            }
            else
            {
                m_options.m_stringBuilder.Append("No Materials.xml found; skipping material compilation.\n");
            }            
        }

        /// <summary>
        /// Go through all the available source model files and convert them to PSM, write them to the destination directory.
        /// </summary>
        private void BuildModels()
        {
            m_options.m_stringBuilder.Append("Starting model builder...\n");

            const string srcDir = "data_source";
            const string dstDir = "data\\ROOT";
            string workingDir = Path.Combine(Utilities.GetPsxDevRoot(), m_options.m_projectPath);
            string srcPath = Path.Combine(workingDir, srcDir);

            // Enumerate available models to convert.
            var modelFilenames = Directory.EnumerateFiles(srcPath, "*.obj", SearchOption.AllDirectories);
            foreach (string f in modelFilenames)
            {
                // assemble the relative source and target directories:
                string srcRelativePath = f.Substring(f.LastIndexOf(srcDir) + srcDir.Length + 1);
                string dstRelativePath = (Path.ChangeExtension(srcRelativePath, ".PSM")).ToUpper();

                string compilerInputPath = Path.Combine(srcDir, srcRelativePath);
                string compilerOutputPath = Path.Combine(dstDir, dstRelativePath);

                // Create the destination directory.
                // This should probably be in the ModelCompiler, but easier to do in .NET
                string dirToCreate = Path.GetDirectoryName(Path.Combine(workingDir, compilerOutputPath));
                Directory.CreateDirectory(dirToCreate);

                // Build the ModelCompiler arguments:
                string args = string.Format("--input={0} --output={1} -s 0.01", compilerInputPath, compilerOutputPath);

                Utilities.LaunchProcessAndWait(ModelCompilerPath, args, workingDir, m_options.m_stringBuilder);
            }
        }

        private ResBuilderOptions m_options;
    }
}
