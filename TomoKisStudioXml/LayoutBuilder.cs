using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.Globalization;

namespace TomoKisStudioXml
{
    public class LayoutBuilder
    {
        private string path;
        private string layout;
        private MeshFormat output_mesh;
        private string electrodes;

        private string volume;
        private string nodes;
        private string simples;
        private bool mesh_changed;

        /// <summary>
        /// 0 - nic istotnego, 1 - jest rozklad elektrod
        /// </summary>
        private int ReturnCode;

        public string Nodes
        {
            get { return this.nodes; }
            set
            {
                this.nodes = value;
                this.volume = string.Empty;
                this.output_mesh = MeshFormat.WinRECO;
            }
        }

        public string Simples
        {
            get { return this.simples; }
            set
            {
                this.simples = value;
                this.volume = string.Empty;
                this.output_mesh = MeshFormat.WinRECO;
            }
        }

        public string Volume
        {
            get { return this.volume; }
            set
            {
                this.volume = value;
                this.nodes = string.Empty;
                this.simples = string.Empty;
                this.output_mesh = MeshFormat.NETGEN;
            }
        }

        public string Electrodes
        {
            get { return this.electrodes; }
            set
            {
                this.electrodes = value;
                this.ReturnCode = 0;

                if (!string.IsNullOrEmpty(this.electrodes))
                    if (File.Exists(this.electrodes))
                        this.ReturnCode = 1;
            }
        }

        public bool MeshChanged
        {
            get { return this.mesh_changed; }
            set { this.mesh_changed = value; }
        }

        public string Layout
        {
            get { return this.layout; }
            set { this.layout = value; }
        }

        public LayoutBuilder(string xml_file_name)
        {
            this.path = Internal.GetStorePath(xml_file_name);
            this.Startup();
        }

        public LayoutBuilder()
        {
            this.path = Internal.GetStorePath("ECTLayoutBuilder.xml");
            this.Startup();
        }

        private void Startup()
        {
            if (!File.Exists(this.path))
                this.CreateEmptyFile();
            else
                this.Load();
        }

        public void Clear()
        {
            this.ReturnCode = 0;
            this.MeshChanged = false;
            this.Nodes = string.Empty;
            this.Simples = string.Empty;
            this.Volume = string.Empty;
            this.Electrodes = string.Empty;
            this.output_mesh = MeshFormat.Invalid;
            this.layout = string.Empty;
        }

        private void CreateEmptyFile()
        {
            try
            {
                Directory.CreateDirectory(Path.GetDirectoryName(this.path));
            }
            catch (Exception e)
            {
                if (MessageBox.Show(string.Format("An exception has occured during 'create directory' attempt:\n{0}", e.Message), Application.ProductName, MessageBoxButtons.OKCancel, MessageBoxIcon.Error) == DialogResult.Cancel)
                    Application.Exit();
            }

            this.Clear();
            this.Save();
        }

        private void Load()
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(this.path);

                XmlElement root = doc.DocumentElement;

                this.ReturnCode = int.Parse(root["return_code"].InnerText, CultureInfo.InvariantCulture);
                this.Nodes = root["nodes"].InnerText;
                this.Simples = root["simples"].InnerText;
                this.Volume = root["volume"].InnerText;
                this.Electrodes = root["electrodes"].InnerText;
                this.layout = root["layout"].InnerText;
                this.MeshChanged = int.Parse(root["mesh_changed"].InnerText, CultureInfo.InvariantCulture) == 1;

                switch (int.Parse(root["output_format"].InnerText, CultureInfo.InvariantCulture))
                {
                    case 0:
                        this.output_mesh = MeshFormat.NETGEN;
                        break;
                    case 1:
                        this.output_mesh = MeshFormat.WinRECO;
                        break;
                    default:
                        this.output_mesh = MeshFormat.Invalid;
                        break;
                }

            }
            catch (Exception)
            {
                this.Clear();
            }
        }

        public void Save()
        {
            XmlDocument doc = new XmlDocument();
            XmlNode prolog = doc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            XmlNode root = doc.CreateElement("root");

            XmlElement return_code = doc.CreateElement("return_code");
            XmlElement nodes = doc.CreateElement("nodes");
            XmlElement simples = doc.CreateElement("simples");
            XmlElement volume = doc.CreateElement("volume");
            XmlElement electrodes = doc.CreateElement("electrodes");
            XmlElement output_format = doc.CreateElement("output_format");
            XmlElement mesh_changed = doc.CreateElement("mesh_changed");
            XmlElement layout = doc.CreateElement("layout");

            doc.AppendChild(prolog);
            doc.AppendChild(root);

            root.AppendChild(return_code);
            root.AppendChild(output_format);
            root.AppendChild(nodes);
            root.AppendChild(simples);
            root.AppendChild(volume);
            root.AppendChild(electrodes);
            root.AppendChild(mesh_changed);
            root.AppendChild(layout);

            return_code.InnerText = this.ReturnCode.ToString();
            nodes.InnerText = this.Nodes;
            simples.InnerText = this.Simples;
            volume.InnerText = this.Volume;
            electrodes.InnerText = this.Electrodes;
            layout.InnerText = this.layout;

            if (this.output_mesh == MeshFormat.Invalid)
                output_format.InnerText = "-1";
            if (this.output_mesh == MeshFormat.NETGEN)
                output_format.InnerText = "0";
            if (this.output_mesh == MeshFormat.WinRECO)
                output_format.InnerText = "1";

            mesh_changed.InnerText = (this.mesh_changed ? "1" : "0");


            doc.Save(this.path);
        }
    }


}
