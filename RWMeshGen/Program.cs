using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace RWMeshGen
{
    static class Program
    {
        private static TomoKisStudioXml.MeshGenerator kis;

        public static TomoKisStudioXml.MeshGenerator KIS
        {
            get { return kis; }
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Program.kis = new TomoKisStudioXml.MeshGenerator();
            Program.kis.Clear();
            Program.kis.Save();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainWindow());

        }
    }
}
