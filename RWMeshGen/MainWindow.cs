using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Globalization;

namespace RWMeshGen
{
    public partial class MainWindow : Form
    {
        double shield_distance;
        double inner_wall_distance;
        double wall_thickness;
        double electrodes_shield_distance;
        double outer_wall_distance;
        double height, vertical_sparsity;

        double s0v, s1v, s2v, s3v, s0r, s1r, s2r, s3r;
        double material_inner, material_tube, material_outer;
        bool auto_update;

        //double electrodes_ring_radius;
        int electrodes_count;
        int nodes_per_electrode;



        Geometry.MeshGeneratorWrapper mesh_generator;
        CapMeshVisualiser cap_visualiser;


        public MainWindow()
        {
            this.InitializeComponent();
            this.OnChangeSensorParameters(null, null);
            this.mesh_generator = null;
            this.cap_visualiser = null;

            this.RestoreValues();
            UpdateGUIEvent(null, null);
        }

        private void OnChangeSensorParameters(object sender, EventArgs e)
        {
            if (this.chkS1Lock.Checked && (sender == this.edtS1))
                return;

            this.shield_distance = (double)this.edtShieldDistance.Value;
            this.inner_wall_distance = (double)this.edtInnerRadius.Value;
            this.wall_thickness = (double)this.edtWallThickness.Value;
            this.height = (double)this.edtHeight.Value;
            this.vertical_sparsity = (double)this.edtVerticalSparsity.Value;

            this.outer_wall_distance = this.inner_wall_distance + this.wall_thickness;
            this.electrodes_shield_distance = this.shield_distance - this.outer_wall_distance;

            if (this.chkS1Lock.Checked)
            {
                this.edtS1.Enabled = false;
                this.edtS1.Value = this.edtS0.Value;
            }
            else
                this.edtS1.Enabled = true;

            this.s0v = (double)this.edtS0.Value;
            this.s1v = (double)this.edtS1.Value;
            this.s2v = (double)this.edtS2.Value;
            this.s3v = (double)this.edtS3.Value;
            double perc = (double)this.edtPercent.Value / 100.0D;

            this.s0r = 0;
            this.s1r = this.inner_wall_distance * perc;
            this.s2r = this.outer_wall_distance;
            this.s3r = this.shield_distance;

            this.material_inner = (double)this.edtInnterMaterial.Value;
            this.material_tube  =(double)this.edtTubeMaterial.Value;
            this.material_outer=(double)this.edtOuterMaterial.Value;

            this.lblRA.Text = string.Format("{0:N3}mm", this.s1r);
            this.lblRA.ForeColor = (this.s1r >= this.s2r) ? Color.Red : Color.Black;
            
            this.lblElectrodesDistance.Text = string.Format("{0:N3}mm", this.outer_wall_distance);
            this.lblElectrodesDistance.ForeColor = (this.outer_wall_distance < 0) ? Color.Red : Color.Black;
            this.lblElectrodesShieldDistance.Text = string.Format("{0:N3}mm", this.electrodes_shield_distance);
            this.lblElectrodesShieldDistance.ForeColor = (this.electrodes_shield_distance < 0) ? Color.Red : Color.Black;

            double s12_tan = Math.Atan((this.s2v - this.s1v) / (this.s2r - this.s1r));
            s12_tan = ((s12_tan * 360.0D) / (2 * Math.PI));
            this.lblAlpha1.Text = string.Format("{0:N3}\xB0", s12_tan);

            double s23_tan = Math.Atan((this.s3v - this.s2v) / (this.s3r - this.s2r));
            s23_tan = ((s23_tan * 360.0D) / (2 * Math.PI));
            this.lblAlpha2.Text = string.Format("{0:N3}\xB0", -s23_tan);

            // pozycja elektrod
            this.electrodes_count = (int)this.edtNumberOfElectrodes.Value;
            this.nodes_per_electrode = (int)this.edtNodesPerElectrode.Value;

            if (this.auto_update)
                this.btnShowMeshCap_Click(null, null);
        }


        private void btnBrowse_Click(object sender, EventArgs e)
        {
            using (FolderBrowserDialog dlg = new FolderBrowserDialog())
            {
                dlg.Description = "Choose destination directory for the generated mesh";
                dlg.RootFolder = Environment.SpecialFolder.MyComputer;
                dlg.SelectedPath = this.edtPath.Text;
                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                this.edtPath.Text = dlg.SelectedPath;
            }
        }

        private void Form2_FormClosed(object sender, FormClosedEventArgs e)
        {
            this.SaveValues();
        }

        private void SaveValues()
        {
            Properties.Settings.Default.shield_distance = this.edtShieldDistance.Value;
            Properties.Settings.Default.wall_thickness = this.edtWallThickness.Value;
            Properties.Settings.Default.sensor_inner_radius = this.edtInnerRadius.Value;
            Properties.Settings.Default.vertical_sparsity = this.edtVerticalSparsity.Value;
            Properties.Settings.Default.height = this.edtHeight.Value;
            Properties.Settings.Default.s1_percent = this.edtPercent.Value;

            Properties.Settings.Default.material_inner = this.edtInnterMaterial.Value;
            Properties.Settings.Default.material_outer = this.edtOuterMaterial.Value;
            Properties.Settings.Default.material_tube = this.edtTubeMaterial.Value;

            Properties.Settings.Default.s0 = this.edtS0.Value;
            Properties.Settings.Default.s1 = this.edtS1.Value;
            Properties.Settings.Default.s2 = this.edtS2.Value;
            Properties.Settings.Default.s3 = this.edtS3.Value;

            Properties.Settings.Default.s1_lock = this.chkS1Lock.Checked;
            Properties.Settings.Default.generate_spatial = this.chk3D.Checked;
            Properties.Settings.Default.generate_materials = this.chkGenerateMaterials.Checked;
            Properties.Settings.Default.generate_complete_model = this.chkGenerateCompleteModel.Checked;
            Properties.Settings.Default.generate_vtk = this.chkGenerateVTKFiles.Checked;

            Properties.Settings.Default.paths = new string[this.edtPath.Items.Count];
            this.edtPath.Items.CopyTo(Properties.Settings.Default.paths, 0);
            Properties.Settings.Default.path_last = this.edtPath.Text;
            Properties.Settings.Default.description = this.edtDescription.Text;
            Properties.Settings.Default.clean_dir = this.chkCleanDir.Checked;

            Properties.Settings.Default.distr_active = this.chkGeneratePlainElectrodes.Checked;
            Properties.Settings.Default.distr_count = (int)this.edtNumberOfElectrodes.Value;
            Properties.Settings.Default.distr_nodes = (int)this.edtNodesPerElectrode.Value;

            Properties.Settings.Default.Save();
        }

        private void RestoreValues()
        {
            //Properties.Settings.Default.shield_distance = 160;
            bool attempt = false;
            while (true)
            {
                try
                {
                    this.edtShieldDistance.Value = Properties.Settings.Default.shield_distance;
                    this.edtWallThickness.Value = Properties.Settings.Default.wall_thickness;
                    this.edtInnerRadius.Value = Properties.Settings.Default.sensor_inner_radius;
                    this.edtVerticalSparsity.Value = Properties.Settings.Default.vertical_sparsity;
                    this.edtHeight.Value = Properties.Settings.Default.height;
                    this.edtPercent.Value = Properties.Settings.Default.s1_percent;

                    this.edtInnterMaterial.Value = Properties.Settings.Default.material_inner;
                    this.edtOuterMaterial.Value = Properties.Settings.Default.material_outer;
                    this.edtTubeMaterial.Value = Properties.Settings.Default.material_tube;

                    this.edtS0.Value = Properties.Settings.Default.s0;
                    this.edtS1.Value = Properties.Settings.Default.s1;
                    this.edtS2.Value = Properties.Settings.Default.s2;
                    this.edtS3.Value = Properties.Settings.Default.s3;


                    //this.chk2D.Checked = Properties.Settings.Default.is_2D;
                    this.chk3D.Checked = Properties.Settings.Default.generate_spatial;
                    this.chkS1Lock.Checked = Properties.Settings.Default.s1_lock;
                    this.chkGenerateMaterials.Checked = Properties.Settings.Default.generate_materials;
                    this.chkGenerateCompleteModel.Checked = Properties.Settings.Default.generate_complete_model;
                    this.chkGenerateVTKFiles.Checked = Properties.Settings.Default.generate_vtk;

                    if (Properties.Settings.Default.paths == null)
                        Properties.Settings.Default.paths = new string[0];
                    this.edtPath.Items.Clear();
                    this.edtPath.Items.AddRange(Properties.Settings.Default.paths);
                    this.edtPath.Text = Properties.Settings.Default.path_last;
                    this.edtDescription.Text = Properties.Settings.Default.description;
                    this.chkCleanDir.Checked = Properties.Settings.Default.clean_dir;

                    this.chkGeneratePlainElectrodes.Checked = Properties.Settings.Default.distr_active;
                    this.edtNumberOfElectrodes.Value = Properties.Settings.Default.distr_count;
                    this.edtNodesPerElectrode.Value = Properties.Settings.Default.distr_nodes;
                    //this.rbPlacementRo.Checked = Properties.Settings.Default._______distr_ro;
                    //this.rbPlacementRs.Checked = Properties.Settings.Default._______distr_rs;

                    return;
                }
                catch(Exception e)
                {
                    if (!attempt)
                    {
                        attempt = true;
                        Properties.Settings.Default.Reset();
                        continue;
                    }

                    MessageBox.Show("Configuration loader exception:\n" + e.Message, Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                   
                }
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnShowMeshCap_Click(object sender, EventArgs e)
        {
            this.SetupMeshParameters();

            if ((this.s3r < this.s2r) && this.chkGenerateCompleteModel.Checked)
            {
                MessageBox.Show("Incorrect distances values", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            this.mesh_generator.GeneratePlainMesh();

            if (this.chkGeneratePlainElectrodes.Enabled && this.chkGeneratePlainElectrodes.Checked)
                this.mesh_generator.GeneratePlainElectrodesDistribution();

            if (this.cap_visualiser == null)
            {
                this.cap_visualiser = new CapMeshVisualiser(this.mesh_generator, this);
                this.cap_visualiser.chkAutoUpdate.Checked = this.auto_update;
            }
            if (sender != null)
                if (!this.cap_visualiser.Visible)
                    this.cap_visualiser.Show();
            
            this.cap_visualiser.UpdateVisualisation();
        }

        /// <summary>
        /// dfdf
        /// </summary>
        private void SetupMeshParameters()
        {
            if (this.mesh_generator == null)
                this.mesh_generator = new Geometry.MeshGeneratorWrapper();
            else
                this.mesh_generator.Cleanup();

            double[] rpositions = new double[] { this.s0r, this.s1r, this.s2r, this.s3r };
            double[] svalues = new double[] { this.s0v, this.s1v, this.s2v, this.s3v };
            double[] rings = new double[] { 0.0D, this.inner_wall_distance, this.outer_wall_distance, this.shield_distance };
            double[] materials = new double[] { this.material_inner, this.material_tube, this.material_outer, 0.00D };

            if (!this.chkGenerateCompleteModel.Checked)
            {
                rpositions = new double[] { this.s0r, this.s1r, this.s2r, this.s2r };
                svalues = new double[] { this.s0v, this.s1v, this.s2v, this.s2v };
                rings = new double[] { 0.0D, this.inner_wall_distance, this.outer_wall_distance, -1 }; ;
                materials = new double[] { this.material_inner, this.material_tube, 0,0 };
            }

            this.mesh_generator.SetSparseFunctionPoints(rpositions, svalues);
            this.mesh_generator.SetRingsAndMaterials(rings, materials);
            this.mesh_generator.SetSpatialParameters(this.height, this.vertical_sparsity);
            if (this.chkGenerateCompleteModel.Enabled && this.chkGenerateCompleteModel.Checked)
                this.mesh_generator.SetPlainElectrodesDistributionParameters(this.outer_wall_distance, this.shield_distance, this.electrodes_count, this.nodes_per_electrode);
            else
                this.mesh_generator.SetPlainElectrodesDistributionParameters(this.outer_wall_distance, -1.0, this.electrodes_count, this.nodes_per_electrode);
        }

        private void btnGenerateData_Click(object sender, EventArgs e)
        {
            //if (this.chkGenerateVTKFiles.Checked && !this.chk3D.Checked)
            //{
            //    MessageBox.Show("Save Mesh as VTK file is not available for plain meshes", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Information);
            //    return;
            //}



            bool old = this.auto_update;
            this.OnChangeSensorParameters(null, null);
            this.auto_update = old;

            if ((this.s3r < this.s2r) && this.chkGenerateCompleteModel.Checked)
            {
                MessageBox.Show("Incorrect distances values", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            Program.KIS.Clear();
            string mesh_path = this.edtPath.Text;
            this.btnGenerateData.Enabled = false;

            using (StatusWindow sw = new StatusWindow("Meshing process", "Preparing data"))
                try
                {
                    Directory.CreateDirectory(mesh_path);
                    if (this.chkCleanDir.Checked)
                    {
                        List<string> files = new List<string>();
                        files.AddRange(Directory.GetFiles(mesh_path, "*.txt", SearchOption.TopDirectoryOnly));
                        files.AddRange(Directory.GetFiles(mesh_path, "*.vtk", SearchOption.TopDirectoryOnly));
                        foreach (string f in files)
                            File.Delete(f);

                    }
                    string nodes_file, complete_simples_file, complete_mat_file, vtk_complete_file;
                    string electr_simples_file, electr_mat_file, vtk_electr_file;
                    string reco_simples_file, reco_mat_file, vtk_reco_file;
                    string vtk_shield_file, vtk_electrodes_file;
                    string distr_file, xml_filename;

                    XmlDocument doc = new XmlDocument();
                    doc.AppendChild(doc.CreateXmlDeclaration("1.0", string.Empty, "yes"));
                    XmlNode xml_mesh = doc.AppendChild(doc.CreateElement("mesh"));
                    XmlElement xml_format = xml_mesh.AppendChild(doc.CreateElement("format")) as XmlElement;
                    XmlElement xml_is_complete = xml_mesh.AppendChild(doc.CreateElement("is_complete")) as XmlElement;
                    XmlElement xml_type = xml_mesh.AppendChild(doc.CreateElement("type")) as XmlElement;
                    XmlElement xml_geometry = xml_mesh.AppendChild(doc.CreateElement("geometry")) as XmlElement;
                    XmlElement xml_nodes = xml_geometry.AppendChild(doc.CreateElement("nodes")) as XmlElement;
                    XmlElement xml_complete = xml_geometry.AppendChild(doc.CreateElement("complete")) as XmlElement;
                    XmlElement xml_electrodes = xml_geometry.AppendChild(doc.CreateElement("electrodes")) as XmlElement;
                    XmlElement xml_image = xml_geometry.AppendChild(doc.CreateElement("image")) as XmlElement;
                    xml_complete.Attributes.Append(doc.CreateAttribute("radius"));
                    xml_electrodes.Attributes.Append(doc.CreateAttribute("radius"));
                    xml_image.Attributes.Append(doc.CreateAttribute("radius"));

                    xml_filename = Path.Combine(mesh_path, "m_geometry.rwmg");
                    xml_format.InnerText = "rwmeshgen";
                    
                    // plik węzłów, wspólny dla różnych siatek
                    nodes_file = Path.Combine(mesh_path, "m_nodes.txt");
                    xml_nodes.InnerText = Path.GetFileName(nodes_file);

                    // complete, electr i reco - do matlaba
                    complete_simples_file = Path.Combine(mesh_path, "m_simples_complete.txt");
                    complete_mat_file = Path.Combine(mesh_path, "m_material_complete.txt");
                    electr_simples_file = Path.Combine(mesh_path, "m_simples_elect.txt");
                    electr_mat_file = Path.Combine(mesh_path, "m_material_elect.txt");
                    reco_simples_file = Path.Combine(mesh_path, "m_simples_reco.txt");
                    reco_mat_file = Path.Combine(mesh_path, "m_material_reco.txt");

                    // complete, electr, reco - do vtk
                    vtk_complete_file = Path.Combine(mesh_path, "mesh_complete.vtk");
                    vtk_electr_file = Path.Combine(mesh_path, "mesh_elec.vtk");
                    vtk_reco_file = Path.Combine(mesh_path, "mesh_reco.vtk");

                    // shield, electrodes surface - do vtk
                    vtk_shield_file = Path.Combine(mesh_path, "surf_shld.vtk");
                    vtk_electrodes_file = Path.Combine(mesh_path, "surf_elec.vtk");

                    // rozkład elektrod czujnika 2D
                    distr_file = Path.Combine(mesh_path, "m_electr.txt");
                    
                    if (!this.chkGenerateMaterials.Checked)
                    {
                        complete_mat_file = null;
                        electr_mat_file = null;
                        reco_mat_file = null;
                    }

                    Program.KIS.HasMaterials = this.chkGenerateMaterials.Checked;
                    Program.KIS.IsComplete = this.chkGenerateCompleteModel.Checked;

                    xml_is_complete.InnerText = this.chkGenerateCompleteModel.Checked.ToString();
                    if (this.chkGenerateCompleteModel.Checked)
                    {
                        // model kompletny
                        Program.KIS.ElectrodeSurfaceRadius = this.outer_wall_distance;
                        Program.KIS.OuterScreenSurfaceRadius = this.shield_distance;

                        xml_complete.InnerText = Path.GetFileName(complete_simples_file);
                        xml_electrodes.InnerText = Path.GetFileName(electr_simples_file);
                        xml_image.InnerText = Path.GetFileName(reco_simples_file);

                        xml_complete.Attributes["radius"].Value = this.shield_distance.ToString("N4", CultureInfo.InvariantCulture);
                        xml_electrodes.Attributes["radius"].Value = this.outer_wall_distance.ToString("N4", CultureInfo.InvariantCulture);
                        xml_image.Attributes["radius"].Value = this.inner_wall_distance.ToString("N4", CultureInfo.InvariantCulture);
                    }
                    else
                    {
                        // model winreco
                        Program.KIS.ElectrodeSurfaceRadius = this.outer_wall_distance;
                        Program.KIS.OuterScreenSurfaceRadius = 0;

                        xml_electrodes.InnerText = Path.GetFileName(electr_simples_file);
                        xml_complete.Attributes["radius"].Value = ((double)0).ToString("N4", CultureInfo.InvariantCulture);
                        xml_electrodes.Attributes["radius"].Value = this.outer_wall_distance.ToString("N4", CultureInfo.InvariantCulture);
                        xml_image.Attributes["radius"].Value = ((double)0).ToString("N4", CultureInfo.InvariantCulture);
                    }


                    DateTime start_time = DateTime.Now;
                    this.SetupMeshParameters();

                    if (this.chk3D.Checked)
                    {
                        Program.KIS.Mode = TomoKisStudioXml.MeshType.Mode3D;

                        sw.SetStatus("Spatial mesh generation in progress...");
                        this.mesh_generator.GenerateSpatialMesh();

                        if (this.chkGenerateCompleteModel.Checked)
                        {
                            // model kompletny
                            sw.SetStatus("Saving spatial model data (complete mesh)...");
                            this.mesh_generator.SaveSpatialData(nodes_file, complete_simples_file, complete_mat_file);
                            Program.KIS.NodesFile = nodes_file;
                            Program.KIS.CompleteSimplexesFile = complete_simples_file;
                            Program.KIS.CompleteMaterialsFile = complete_mat_file;

                            sw.SetStatus("Saving spatial model data (reconstruction mesh + electrodes mesh)...");
                            this.mesh_generator.SaveSpatialElectrodesData(String.Empty, electr_simples_file, electr_mat_file);
                            Program.KIS.ElectrodesSimplexesFile = electr_simples_file;
                            Program.KIS.ElectrodesMaterialsFile = electr_mat_file;

                            sw.SetStatus("Saving spatial model data (reconstruction mesh)...");
                            this.mesh_generator.SaveSpatialReconstructionData(String.Empty, reco_simples_file, reco_mat_file);
                            Program.KIS.ReconstructionSimplexesFile = reco_simples_file;
                            Program.KIS.ReconstructionMaterialsFile = reco_mat_file;

                        }
                        else
                        {
                            // model WinRECO
                            sw.SetStatus("Saving spatial model data (complete mesh)...");
                            //this.mesh_generator.SaveSpatialData(nodes_file, complete_simples_file, complete_mat_file);
                            //Program.Kis.NodesFile = nodes_file;
                            //Program.Kis.CompleteSimplexesFile = complete_simples_file;
                            //Program.Kis.CompleteMaterialsFile = complete_mat_file;

                            // 16.02.2010: pomysł Radka
                            this.mesh_generator.SaveSpatialElectrodesData(nodes_file, reco_simples_file, reco_mat_file);
                            Program.KIS.NodesFile = nodes_file;
                            Program.KIS.ReconstructionSimplexesFile = reco_simples_file;
                            Program.KIS.ReconstructionMaterialsFile = reco_mat_file;

                        }

                        if (this.chkGenerateVTKFiles.Checked)
                        {
                            if (this.chkGenerateCompleteModel.Checked)
                            {
                                // model complete
                                sw.SetStatus("Saving spatial model data (complete mesh) to VTK file...");
                                this.mesh_generator.SaveSpatialDataVTK(vtk_complete_file, vtk_shield_file, vtk_electrodes_file, this.chkGenerateMaterials.Checked);
                                sw.SetStatus("Saving spatial model data (reconstruction mesh + electrodes mesh) to VTK file...");
                                this.mesh_generator.SaveSpatialElectrodesDataVTK(vtk_electr_file, this.chkGenerateMaterials.Checked);
                                sw.SetStatus("Saving spatial model data (reconstruction mesh) to VTK file...");
                                this.mesh_generator.SaveSpatialReconstructionDataVTK(vtk_reco_file, this.chkGenerateMaterials.Checked);
                            }
                            else
                            {
                                // model winreco
                                //sw.SetStatus("Saving spatial model data (complete mesh) to VTK file...");
                                //this.mesh_generator.SaveSpatialDataVTK(vtk_complete_file, vtk_shield_file, vtk_electrodes_file, this.chkGenerateMaterials.Checked);

                                // 16.02.2010: pomysł Radka
                                sw.SetStatus("Saving spatial model data (reconstruction mesh + electrodes mesh) to VTK file...");
                                this.mesh_generator.SaveSpatialElectrodesDataVTK(vtk_reco_file, this.chkGenerateMaterials.Checked);

                            }
                        }
                    }
                    else
                    {
                        Program.KIS.Mode = TomoKisStudioXml.MeshType.Mode2D;

                        sw.SetStatus("Plain mesh generation in progress...");
                        this.mesh_generator.GeneratePlainMesh();


                        if (this.chkGeneratePlainElectrodes.Checked)
                        {
                            // generacja rozladu elektrod
                            this.mesh_generator.GeneratePlainElectrodesDistribution();
                            this.mesh_generator.SaveDistribution(distr_file);
                            Program.KIS.PlainDistribution = distr_file;
                        }

                        if (this.chkGenerateCompleteModel.Checked)
                        {
                            // model kompletny
                            sw.SetStatus("Saving plain model data (complete mesh)...");
                            this.mesh_generator.SavePlainData(nodes_file, complete_simples_file, complete_mat_file);
                            Program.KIS.NodesFile = nodes_file;
                            Program.KIS.CompleteSimplexesFile = complete_simples_file;
                            Program.KIS.CompleteMaterialsFile = complete_mat_file;

                            sw.SetStatus("Saving plain model data (reconstruction mesh)...");
                            this.mesh_generator.SavePlainElectrodesData(String.Empty, electr_simples_file, electr_mat_file);
                            Program.KIS.ElectrodesSimplexesFile = electr_simples_file;
                            Program.KIS.ElectrodesMaterialsFile = electr_mat_file;

                            sw.SetStatus("Saving plain model data (reconstruction mesh)...");
                            this.mesh_generator.SavePlainReconstructionData(String.Empty, reco_simples_file, reco_mat_file);
                            Program.KIS.ReconstructionSimplexesFile = reco_simples_file;
                            Program.KIS.ReconstructionMaterialsFile = reco_mat_file;
                        }
                        else
                        {
                            // model winreco
                            //sw.SetStatus("Saving plain model data (complete mesh)...");
                            //this.mesh_generator.SavePlainData(nodes_file, complete_simples_file, complete_mat_file);
                            //Program.Kis.NodesFile = nodes_file;
                            //Program.Kis.CompleteSimplexesFile = complete_simples_file;
                            //Program.Kis.CompleteMaterialsFile = complete_mat_file;

                            // 16.02.2010: pomysł Radka
                            sw.SetStatus("Saving plain model data (reconstruction mesh)...");
                            this.mesh_generator.SavePlainElectrodesData(nodes_file, reco_simples_file, reco_mat_file);
                            Program.KIS.NodesFile = nodes_file;
                            Program.KIS.ReconstructionSimplexesFile = reco_simples_file;
                            Program.KIS.ReconstructionMaterialsFile = reco_mat_file;
                        }
                    }

                    xml_type.InnerText = Program.KIS.Mode.ToString(); // 2d czy 3d?
                    doc.Save(xml_filename);

                    if (!this.edtPath.Items.Contains(this.edtPath.Text))
                        this.edtPath.Items.Add(this.edtPath.Text);
                     
                    sw.Close();
                    TimeSpan ts = DateTime.Now - start_time;

                    this.ShowMeshStatistics(ts);
                }
                catch (Exception ex)
                {
                    sw.Close();
                    Program.KIS.Clear();
                    MessageBox.Show("An exception has occured during mesh generation process:\n" + ex.Message, Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

            Program.KIS.ReturnCode = 1;
            Program.KIS.Save();
            
            this.btnGenerateData.Enabled = true;
        }

        private void ShowMeshStatistics(TimeSpan ts)
        {
            using (MeshStatistics dlg = new MeshStatistics())
            {
                dlg._time_taken = ts;
                dlg._plain_elements = this.mesh_generator.GetNumberOfPlainElements();
                dlg._plain_nodes = this.mesh_generator.GetNumberOfPlainNodes();
                dlg._spatial_elements = this.mesh_generator.GetNumberOfSpatialElements();
                dlg._spatial_nodes = this.mesh_generator.GetNumberOfSpatialNodes();

                dlg.ShowDialog();
            }
        }


        internal void SetVisualisationAutoUpdate(bool p)
        {
            this.auto_update = p;
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {

        }

        private void mnuMainMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            if ((e.ClickedItem as ToolStripMenuItem).HasDropDownItems)
                return;

            if (e.ClickedItem == this.mnuAbout)
            {
                using (AboutBox dlg = new AboutBox())
                    dlg.ShowDialog();
                return;
            }


            MessageBox.Show("This function is currently not implemented", Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void UpdateGUIEvent(object sender, EventArgs e)
        {
            this.edtHeight.Enabled = this.chk3D.Checked;
            this.edtVerticalSparsity.Enabled = this.chk3D.Checked;
            this.groupBox7.Enabled = !this.chk3D.Checked;
            this.chkGenerateVTKFiles.Enabled = this.chk3D.Checked;

            this.edtInnterMaterial.Enabled = this.chkGenerateMaterials.Checked;
            this.edtTubeMaterial.Enabled = this.chkGenerateMaterials.Checked;
            this.edtOuterMaterial.Enabled = this.chkGenerateMaterials.Checked && this.chkGenerateCompleteModel.Checked;

            if (this.chkGeneratePlainElectrodes.Checked)
            {
                this.edtNumberOfElectrodes.Enabled = true;
                this.edtNodesPerElectrode.Enabled = true;
            }
            else
            {
                this.edtNumberOfElectrodes.Enabled = false;
                this.edtNodesPerElectrode.Enabled = false;
            }

            this.edtShieldDistance.Enabled = this.chkGenerateCompleteModel.Checked;
            this.edtS3.Enabled = this.chkGenerateCompleteModel.Checked;
            this.label4.Enabled = this.chkGenerateCompleteModel.Checked;
            this.lblElectrodesShieldDistance.Enabled = this.chkGenerateCompleteModel.Checked;
            this.label12.Enabled = this.chkGenerateCompleteModel.Checked;
            this.lblAlpha2.Enabled = this.chkGenerateCompleteModel.Checked;


        }
        
    }
}
