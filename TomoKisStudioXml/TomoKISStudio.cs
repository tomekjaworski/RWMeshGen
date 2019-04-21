using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;
using System.Globalization;

namespace TomoKisStudioXml
{
    public enum MeshFormat
    {
        Invalid,
        WinRECO,
        NETGEN,
    }
    public class TomoKISStudio
    {
        private string path;
        XmlDocument doc;

        private MeshFormat mesh_format;
        private string nodes, simples, volume;
        private bool mesg_available;
        private double electr_radius;
        private double screen_radius;
        private string layout_file;

        /// <summary>
        /// A distance (radius) between the center of genenerated geometry and the 3D surface used to build the electrodes' distribution
        /// </summary>
        public double ElectrodeSurfaceRadius
        {
            get { return this.electr_radius; }
        }

        /// <summary>
        /// A distance (radius) between the center of genenerated geometry and the 3D surface used to build the outer screen for sensor
        /// </summary>
        public double OuterScreenSurfaceRadius
        {
            get { return this.screen_radius; }
        }


        public MeshFormat Format
        {
            get { return this.mesh_format; }
        }

        public string LayoutFile
        {
            get { return this.layout_file; }
        }

        public string Nodes
        {
            get
            {
                if ((this.mesh_format == MeshFormat.Invalid) ||(!this.mesg_available))
                    return string.Empty;
                if (this.mesh_format == MeshFormat.WinRECO)
                    return this.nodes;
                return this.volume;
            }
        }

        public string Simples
        {
            get
            {
                if ((this.mesh_format == MeshFormat.Invalid)|| (!this.mesg_available))
                    return string.Empty;
                if (this.mesh_format == MeshFormat.WinRECO)
                    return this.simples;
                return this.volume;
            }
        }

        public string Volume
        {
            get
            {
                if ((this.mesh_format == MeshFormat.Invalid) || (!this.mesg_available) || (this.mesh_format == MeshFormat.WinRECO))
                    return string.Empty;
                return this.volume;
            }
        }

        public bool MeshAvailable
        {
            get { return this.mesg_available; }
        }


        public TomoKISStudio()
        {
            this.path = Internal.GetStorePath("TomoKISStudio.xml");
            this.LoadXML();
        }

        private void LoadXML()
        {
            this.doc = new XmlDocument();
            this.doc.Load(this.path);

            this.mesh_format = MeshFormat.Invalid;
            this.mesg_available = false;

            if (int.Parse(this.doc.SelectSingleNode("TomoKISStudio/Paths/i3DmeshFileType").InnerText, CultureInfo.InvariantCulture) == 1)
                this.mesh_format = MeshFormat.WinRECO;
            if (int.Parse(this.doc.SelectSingleNode("TomoKISStudio/Paths/i3DmeshFileType").InnerText, CultureInfo.InvariantCulture) == 0)
                this.mesh_format = MeshFormat.NETGEN;

            if (this.doc.SelectSingleNode("TomoKISStudio/Paths/d3DMesh_LayoutFile") == null)
                this.layout_file = string.Empty;
            else
            {
                this.layout_file = this.doc.SelectSingleNode("TomoKISStudio/Paths/d3DMesh_LayoutFile").InnerText;
                if (!File.Exists(this.layout_file))
                    this.layout_file = string.Empty;
            }

            this.electr_radius = this.screen_radius = 0;
            try
            {
                this.electr_radius = double.Parse(doc.SelectSingleNode("TomoKISStudio/Paths/d3DMesh_ElecSurfRadius").InnerText, CultureInfo.InvariantCulture);
                this.screen_radius = double.Parse(doc.SelectSingleNode("TomoKISStudio/Paths/d3DMesh_ScreenSurfRadius").InnerText, CultureInfo.InvariantCulture);
            }
            catch
            {
            }

            if (this.mesh_format == MeshFormat.NETGEN)
            {
                this.volume = this.doc.SelectSingleNode("TomoKISStudio/Paths/s3DMeshFileName_Netgen").InnerText;
                if (!File.Exists(this.volume))
                {
                    this.mesg_available = false;
                    return;
                }
                this.mesg_available = true;
                return;
            }

            if (this.mesh_format == MeshFormat.WinRECO)
            {
                this.nodes = this.doc.SelectSingleNode("TomoKISStudio/Paths/s3DMeshFileName_vtx").InnerText;
                this.simples = this.doc.SelectSingleNode("TomoKISStudio/Paths/s3DMeshFileName_smpl_Elec").InnerText;
                if (!File.Exists(this.nodes) || !File.Exists(this.simples))
                {
                    this.mesg_available = false;
                    return;
                }
                this.mesg_available = true;
                return;
            }
        }
    }
}
