using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Globalization;

namespace TomoKisStudioXml
{
    
    public enum MeshType
    {
        Invalid,
        Mode3D,
        Mode2D
    }

    public class MeshGenerator
    {
        private string path;

        public int ReturnCode;
        public MeshType Mode;

        /// <summary>
        /// A name of a file that contains nodes for generated geometry
        /// </summary>
        public string NodesFile;
       

        /// <summary>
        /// A flag determining that model is complete (3 sets of simplexes) or normal (WinRECO compatible)
        /// </summary>
        public bool IsComplete;

        /// <summary>
        /// A flag determining that model has attached files with material values (it can be either electrical permittivity or resistivnes).
        /// Number of available files is determined by the IsComplete flag (3 files for complete mode, 1 file for WinRECO-like model)
        /// </summary>
        public bool HasMaterials;
        
        /// <summary>
        /// A name of a file with all three sets of simplexes (1. for image reconstruction, 2. for electrodes modelling, 3. for outer screen placement)
        /// </summary>
        public string CompleteSimplexesFile;

        /// <summary>
        /// A name of a file with two sets of simplexes (1. for electrodes modelling, 2. for outer screen placement)
        /// </summary>
        public string ElectrodesSimplexesFile;

        /// <summary>
        /// A name of a file that contains set of simplexes for image reconstruction
        /// </summary>
        public string ReconstructionSimplexesFile;

        public string CompleteMaterialsFile;
        public string ElectrodesMaterialsFile;
        public string ReconstructionMaterialsFile;

        /// <summary>
        /// A name of a file with distribution of electrodes generated for 2D case. Elements are triangles
        /// </summary>
        public string PlainDistribution;

        /// <summary>
        /// A distance (radius) between the center of genenerated geometry and the 3D surface used to build the electrodes' distribution
        /// </summary>
        public double ElectrodeSurfaceRadius;

        /// <summary>
        /// A distance (radius) between the center of genenerated geometry and the 3D surface used to build the outer screen for sensor
        /// </summary>
        public double OuterScreenSurfaceRadius;


        public MeshGenerator(string xml_file_name)
        {
            this.path = Internal.GetStorePath(xml_file_name);
            this.Startup();
        }

        public MeshGenerator()
        {
            this.path = Internal.GetStorePath("RWMeshGen.xml");
            this.Startup();
        }

        public void Startup()
        {
            if (!File.Exists(this.path))
                this.CreateEmptyFile();
            else
                this.Load();
        }

        public void Clear()
        {
            this.ReturnCode = 0;
            this.Mode = MeshType.Invalid;

            this.NodesFile = string.Empty;

            this.IsComplete = false;
            this.HasMaterials = false;

            this.CompleteSimplexesFile = string.Empty;
            this.ElectrodesSimplexesFile = string.Empty;
            this.ReconstructionSimplexesFile = string.Empty;

            this.CompleteMaterialsFile = string.Empty;
            this.ElectrodesMaterialsFile = string.Empty;
            this.ReconstructionMaterialsFile = string.Empty;

            this.PlainDistribution = string.Empty;

            this.ElectrodeSurfaceRadius = 0;
            this.OuterScreenSurfaceRadius = 0;
        }

        private void CreateEmptyFile()
        {
            try
            {
                Directory.CreateDirectory(Path.GetDirectoryName(this.path));
            } catch(Exception e)
            {
                if (MessageBox.Show(string.Format("An exception has occured during 'create directory' attempt:\n{0}", e.Message), Application.ProductName, MessageBoxButtons.OKCancel, MessageBoxIcon.Error) == DialogResult.Cancel)
                    Application.Exit();
            }

            this.Clear();
            this.Save();
        }

        public void Save()
        {
            XmlDocument doc = new XmlDocument();
            XmlNode prolog = doc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            XmlNode root = doc.CreateElement("root");

            XmlElement return_code = doc.CreateElement("return_code");
            XmlElement mode = doc.CreateElement("mode");
            XmlElement nodes = doc.CreateElement("nodes");

            XmlElement is_complete = doc.CreateElement("is_complete");
            XmlElement has_materials = doc.CreateElement("has_materials");

            XmlElement complete = doc.CreateElement("complete");
            XmlElement electrodes = doc.CreateElement("electrodes");
            XmlElement reconstruction = doc.CreateElement("reconstruction");

            XmlElement mat_complete = doc.CreateElement("mat_complete");
            XmlElement mat_electrodes = doc.CreateElement("mat_electrodes");
            XmlElement mat_reconstruction = doc.CreateElement("mat_reconstruction");

            XmlElement distr = doc.CreateElement("plain_distribution");

            XmlElement electrode_radius = doc.CreateElement("electrode_radius");
            XmlElement outer_screen_radius = doc.CreateElement("outer_screen_radius");

            doc.AppendChild(prolog);
            doc.AppendChild(root);
            root.AppendChild(return_code);
            root.AppendChild(mode);
            root.AppendChild(nodes);
            root.AppendChild(is_complete);
            root.AppendChild(has_materials);
            root.AppendChild(complete);
            root.AppendChild(electrodes);
            root.AppendChild(reconstruction);
            root.AppendChild(mat_complete);
            root.AppendChild(mat_electrodes);
            root.AppendChild(mat_reconstruction);
            root.AppendChild(distr);
            root.AppendChild(electrode_radius);
            root.AppendChild(outer_screen_radius);


            return_code.InnerText = this.ReturnCode.ToString();
            if (this.Mode == MeshType.Mode2D) mode.InnerText = "2D";
            if (this.Mode == MeshType.Mode3D) mode.InnerText = "3D";
            if (this.Mode == MeshType.Invalid) mode.InnerText = string.Empty;

            nodes.InnerText = this.NodesFile;
            is_complete.InnerText = this.IsComplete ? "1" : "0";
            has_materials.InnerText = this.HasMaterials ? "1" : "0";

            complete.InnerText = this.CompleteSimplexesFile;
            electrodes.InnerText = this.ElectrodesSimplexesFile;
            reconstruction.InnerText = this.ReconstructionSimplexesFile;

            mat_complete.InnerText = this.CompleteMaterialsFile;
            mat_electrodes.InnerText = this.ElectrodesMaterialsFile;
            mat_reconstruction.InnerText = this.ReconstructionMaterialsFile;

            distr.InnerText = this.PlainDistribution;

            electrode_radius.InnerText = this.ElectrodeSurfaceRadius.ToString("N3", CultureInfo.InvariantCulture);
            outer_screen_radius.InnerText = this.OuterScreenSurfaceRadius.ToString("N3", CultureInfo.InvariantCulture);

            doc.Save(this.path);
        }

        private void Load()
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(this.path);

                XmlElement root = doc.DocumentElement;

                this.ReturnCode = int.Parse(root["return_code"].InnerText, CultureInfo.InvariantCulture);
                this.NodesFile = root["nodes"].InnerText;

                if (root["mode"].InnerText.ToUpper() == "3D")
                    this.Mode = MeshType.Mode3D;
                if (root["mode"].InnerText.ToUpper() == "2D")
                    this.Mode = MeshType.Mode2D;

                this.IsComplete = int.Parse(root["is_complete"].InnerText, CultureInfo.InvariantCulture) == 1;
                this.HasMaterials = int.Parse(root["has_materials"].InnerText, CultureInfo.InvariantCulture) == 1;

                this.CompleteSimplexesFile = root["complete"].InnerText;
                this.ElectrodesSimplexesFile = root["electrodes"].InnerText;
                this.ReconstructionSimplexesFile = root["reconstruction"].InnerText;

                this.CompleteMaterialsFile = root["mat_complete"].InnerText;
                this.ElectrodesMaterialsFile = root["mat_electrodes"].InnerText;
                this.ReconstructionMaterialsFile = root["mat_reconstruction"].InnerText;

                this.PlainDistribution = root["plain_distribution"].InnerText;

                this.ElectrodeSurfaceRadius = double.Parse(root["electrode_radius"].InnerText, CultureInfo.InvariantCulture);
                this.OuterScreenSurfaceRadius = double.Parse(root["outer_screen_radius"].InnerText, CultureInfo.InvariantCulture);


            }
            catch (Exception)
            {
                this.Clear();
            }

        }

    }
}
