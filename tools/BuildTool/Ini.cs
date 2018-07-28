using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Runtime.InteropServices;
using System.IO;

namespace BuildTool
{
    /// <summary>
    /// A helper class to read/write Ini files.
    /// </summary>
    class Ini
    {
        private string m_path;
        private string m_exe = Assembly.GetExecutingAssembly().GetName().Name;

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern long WritePrivateProfileString(string Section, string Key, string Value, string FilePath);

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern int GetPrivateProfileString(string Section, string Key, string Default, StringBuilder RetVal, int Size, string FilePath);

        public Ini(string IniPath = null)
        {
            m_path = new FileInfo(IniPath ?? m_exe + ".ini").FullName.ToString();
        }

        public string Read(string key, string section = null, string defaultValue = "")
        {
            var str = new StringBuilder(255);
            GetPrivateProfileString(section ?? m_exe, key, defaultValue, str, 255, m_path);
            return str.ToString();
        }

        public void Write(string key, string value, string section = null)
        {
            WritePrivateProfileString(section ?? m_exe, key, value, m_path);
        }

        public void DeleteKey(string key, string section = null)
        {
            Write(key, null, section ?? m_exe);
        }

        public void DeleteSection(string section = null)
        {
            Write(null, null, section ?? m_exe);
        }

        public bool KeyExists(string key, string section = null)
        {
            return Read(key, section).Length > 0;
        }
    }
}
