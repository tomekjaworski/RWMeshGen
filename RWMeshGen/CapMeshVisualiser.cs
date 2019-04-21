using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Geometry;
using System.Runtime.InteropServices;
using System.Drawing.Drawing2D;

namespace RWMeshGen
{
    public partial class CapMeshVisualiser : Form
    {
        MeshGeneratorWrapper mesher;
        MainWindow main_window;
        Brush[] gray_brushes;

        public CapMeshVisualiser(MeshGeneratorWrapper mesher, MainWindow main_window)
        {
            this.mesher = mesher;
            this.main_window = main_window;

            this.InitializeComponent();

            this.chkAutoUpdate.Checked = Properties.Settings.Default.auto_update_preview;
            this.cRings.SelectedColor = Properties.Settings.Default.color_fixed_ring;
            this.cLines.SelectedColor = Properties.Settings.Default.color_triangle;
            this.cNodes.SelectedColor = Properties.Settings.Default.color_node;

            this.gray_brushes = new Brush[256];
            for (int i = 0; i < 256; i++)
                this.gray_brushes[i] = new SolidBrush(Color.FromArgb(255 - i, 255 - i, 255 - i));
        }

        public void UpdateVisualisation()
        {
            this.UpdateListView();
            this.UpdatePreview();
        }

        private unsafe void UpdatePreview()
        {
            float radius = (float)this.mesher.GetRadius();
            float sx = (this.pictureBox1.Width - 10) / (radius * 2);
            float sy = (this.pictureBox1.Height - 10) / (radius * 2);
            float scale = Math.Min(sx, sy);

            Bitmap bmp = new Bitmap(this.pictureBox1.Width, this.pictureBox1.Height, System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            using (Graphics g = Graphics.FromImage(bmp))
            {
                g.FillRectangle(Brushes.Black, new Rectangle(Point.Empty, bmp.Size));

                this.DrawTriangles(radius, scale, g, false);
                this.DrawTriangles(radius, scale, g, true);
                this.DrawFixedRings(radius, scale, g);
                //22.02.2010: Radek
                //this.DrawSparseFunction(radius, scale, g);
                this.DrawNodes(radius, scale, bmp);
                this.DrawElectrodes(radius, scale, g);
            }

            this.pictureBox1.Image = bmp;
        }

        unsafe private void DrawTriangles(float radius, float scale, Graphics g, bool draw_lines)
        {
            int node_count = this.mesher.GetNumberOfPlainNodes();
            NodeStruct* node_ptr = this.mesher.GetFirstPlainNode();
            int element_count = this.mesher.GetNumberOfPlainElements();
            ElementStruct* element_ptr = this.mesher.GetFirstPlainElement();

            using (Pen line_pen = new Pen(this.cLines.SelectedColor))
                for (int i = 0; i < element_count; i++, element_ptr++)
                {
                    NodeStruct* n1 = &node_ptr[element_ptr->node1];
                    NodeStruct* n2 = &node_ptr[element_ptr->node2];
                    NodeStruct* n3 = &node_ptr[element_ptr->node3];

                    float x1 = (float)(n1->x * scale + 5);
                    float y1 = (float)((2 * radius - n1->y) * scale + 5);
                    float x2 = (float)(n2->x * scale + 5);
                    float y2 = (float)((2 * radius - n2->y) * scale + 5);
                    float x3 = (float)(n3->x * scale + 5);
                    float y3 = (float)((2 * radius - n3->y) * scale + 5);

                    PointF[] points = new PointF[] { new PointF(x1, y1), new PointF(x2, y2), new PointF(x3, y3) };

                    if (draw_lines)
                    {
                        g.DrawLines(line_pen, points);
                    }
                    else
                    {
                        //Brush b = this.gray_brushes[(byte)(255 * this.mesher.NormalizeMaterial(element_ptr->material))];
                        double m = element_ptr->material / 80.0;
                        m = Math.Max(Math.Min(m, 1), 0); // normalizacja: 0=0, 80=1
                        Brush b = this.gray_brushes[(byte)(255 * m)];

                        g.FillPolygon(b, points);
                    }

                }
        }

        unsafe private void DrawNodes(float radius, float scale, Bitmap bmp)
        {
            int node_count = this.mesher.GetNumberOfPlainNodes();
            NodeStruct* node_ptr = this.mesher.GetFirstPlainNode();
            Color c = this.cNodes.SelectedColor;
            for (int i = 0; i < node_count; i++, node_ptr++)
            {
                float nx = (float)(node_ptr->x * scale + 5);
                float ny = (float)((2 * radius - node_ptr->y) * scale + 5);
                // g.FillRectangle(Brushes.DarkGreen, nx - 1, ny - 1, 3, 3);
                bmp.SetPixel((int)Math.Round(nx), (int)Math.Round(ny), c);
            }
        }

        unsafe private void DrawSparseFunction(float radius, float scale, Graphics g)
        {
            float step = 0.5f;
            float x = step;
            using (Pen p = new Pen(Color.DarkGreen, 1))
                while (x <= radius)
                {
                    float y1 = (float)this.mesher.SparseFunction(x - step);
                    float y2 = (float)this.mesher.SparseFunction(x);
                    //y1 = x - step;
                    //y2 = x;
                    float x1 = (float)((radius + x - step) * scale + 5);
                    float x2 = (float)((radius + x) * scale + 5);
                    y1 = (float)((radius - y1) * scale + 5);
                    y2 = (float)((radius - y2) * scale + 5);
                    g.DrawLine(p, x1, y1, x2, y2);
                    x += step;
                }
        }

        unsafe private void DrawFixedRings(float radius, float scale, Graphics g)
        {
            //SmoothingMode old_mode = g.SmoothingMode;
            //g.SmoothingMode = SmoothingMode.AntiAlias;

            int ring_count = this.mesher.GetNumberOfRings();
            RingStruct* ring_ptr = this.mesher.GetFirstRing();
            using (Pen pen = new Pen(this.cRings.SelectedColor))
                for (int i = 0; i < ring_count; i++)
                {
                    RingStruct* r = &ring_ptr[i];
                    if ((r->radius == 0) || (!r->@fixed))
                        continue;
                    g.DrawEllipse(pen,
                        (float)((radius - r->radius) * scale + 5),
                        (float)((radius - r->radius) * scale + 5),
                        (float)((r->radius * 2) * scale),
                        (float)((r->radius * 2) * scale)
                        );
                }

            //g.SmoothingMode = old_mode;
        }

        unsafe private void DrawElectrodes(float radius, float scale, Graphics g)
        {
            NodeStruct* node_ptr = this.mesher.GetFirstPlainNode();

            SmoothingMode old_mode = g.SmoothingMode;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            using (Pen p = new Pen(Color.Red, 3.0f))
                for (int i = 0; i < this.mesher.GetNumberOfPlainElectrodes(); i++)
            {
                PlainElectrodeEdge* edges = this.mesher.GetFirstPlainElectrodeElement(i);
                int count = this.mesher.GetNumberOfPlainElectrodeElements(i);

                for (int j = 0; j < count; j++)
                {
                    NodeStruct* n1 = &node_ptr[edges[j].node1];
                    NodeStruct* n2 = &node_ptr[edges[j].node2];

                    float x1 = (float)(n1->x * scale + 5);
                    float y1 = (float)((2 * radius - n1->y) * scale + 5);
                    float x2 = (float)(n2->x * scale + 5);
                    float y2 = (float)((2 * radius - n2->y) * scale + 5);
                    g.DrawLine(p, x1, y1, x2, y2);
                }
            }
            g.SmoothingMode = old_mode;

        }

        /*
        private unsafe void UpdatePreview()
        {
            float radius = (float)this.mesher.GetRadius();
            float sx = (this.pictureBox1.Width - 10) / (radius * 2);
            float sy = (this.pictureBox1.Height - 10) / (radius * 2);
            float scale = Math.Min(sx, sy);

            Bitmap bmp = new Bitmap(this.pictureBox1.Width, this.pictureBox1.Height, System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            using (Graphics g = Graphics.FromImage(bmp))
            {
                g.FillRectangle(Brushes.White, new Rectangle(Point.Empty, bmp.Size));

                int node_count = this.mesher.GetNumberOfPlainNodes();
                NodeStruct* node_ptr = this.mesher.GetFirstPlainNode();
                int element_count = this.mesher.GetNumberOfPlainElements();
                ElementStruct* element_ptr = this.mesher.GetFirstPlainElement();
                int ring_count = this.mesher.GetNumberOfRings();
                RingStruct* ring_ptr = this.mesher.GetFirstRing();

                for (int i = 0; i < element_count; i++, element_ptr++)
                {
                    NodeStruct* n1 = &node_ptr[element_ptr->node1];
                    NodeStruct* n2 = &node_ptr[element_ptr->node2];
                    NodeStruct* n3 = &node_ptr[element_ptr->node3];

                    float x1 = (float)(n1->x * scale + 5);
                    float y1 = (float)((2 * radius - n1->y) * scale + 5);
                    float x2 = (float)(n2->x * scale + 5);
                    float y2 = (float)((2 * radius - n2->y) * scale + 5);
                    float x3 = (float)(n3->x * scale + 5);
                    float y3 = (float)((2 * radius - n3->y) * scale + 5);

                    g.DrawLines(Pens.Black, new PointF[] { new PointF(x1, y1), new PointF(x2, y2), new PointF(x3, y3) });
                }

                for (int i = 0; i < ring_count; i++)
                {
                    RingStruct* r = &ring_ptr[i];
                    if ((r->radius == 0) || (!r->@fixed))
                        continue;
                    g.DrawEllipse(Pens.Red,
                        (float)((radius - r->radius) * scale + 5),
                        (float)((radius - r->radius) * scale + 5),
                        (float)((r->radius * 2) * scale),
                        (float)((r->radius * 2) * scale)
                        );
                }
                
                for (int i = 0; i < node_count; i++, node_ptr++)
                {
                    float nx = (float)(node_ptr->x * scale + 5);
                    float ny = (float)((2 * radius - node_ptr->y) * scale + 5);
                    g.FillRectangle(Brushes.DarkGreen, nx - 1, ny - 1, 3, 3);
                }

                // sparse function
                float step = 0.5f;
                float x = step;
                using(Pen p = new Pen(Color.Red, 2))
                    while (x <= radius)
                    {
                        float y1 = (float)this.mesher.SparseFunction(x - step);
                        float y2 = (float)this.mesher.SparseFunction(x);
                        //y1 = x - step;
                        //y2 = x;
                        float x1 = (float)((radius + x - step) * scale + 5);
                        float x2 = (float)((radius + x) * scale + 5);
                        y1 = (float)((radius - y1) * scale + 5);
                        y2 = (float)((radius - y2) * scale + 5);
                        g.DrawLine(p, x1, y1, x2, y2);
                        x += step;
                    }
            }

            this.pictureBox1.Image = bmp;
        }
        */

        private unsafe void UpdateListView()
        {
            this.listView1.Items.Clear();
            int ring_count = this.mesher.GetNumberOfRings();
            RingStruct* rs = this.mesher.GetFirstRing();
            RingStruct* old_rs = this.mesher.GetFirstRing();

            for (int i = 0; i < ring_count; i++, old_rs = rs, rs++)
            {
                int nodes = this.mesher.GetNumberOfNodesOnRing(i);
                ListViewItem li = new ListViewItem(new string[] {
                    rs->radius.ToString("N3"),
                    (rs->radius - old_rs->radius).ToString("N3"),
                    rs->density.ToString("N3"),
                    (rs->density - old_rs->density).ToString("N3"),
                    nodes.ToString()
                });

                if (rs->error)
                    li.ForeColor = Color.Red;
                this.listView1.Items.Add(li);
            }

            // statystyka

            int base_nodes = this.mesher.GetNumberOfPlainNodes();
            int base_triangles = this.mesher.GetNumberOfPlainElements();
            int layers = this.mesher.GetNumberOfSpatialLayers();

            int spatial_nodes = base_nodes * layers;
            int spatial_elements = (base_triangles * 3) * (layers - 1);

            this.lblBaseNodes.Text = base_nodes.ToString("###,###,###,##0");
            this.lblBaseTriangles.Text = base_triangles.ToString("###,###,###,##0");
            this.lblSpatialNodes.Text = spatial_nodes.ToString("###,###,###,##0");
            this.lblStaialElements.Text = spatial_elements.ToString("###,###,###,##0");
            this.lblSpatialLayers.Text = layers.ToString();

        }

        private void CapMeshVisualiser_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.auto_update_preview = this.chkAutoUpdate.Checked;
            Properties.Settings.Default.color_fixed_ring = this.cRings.SelectedColor;
            Properties.Settings.Default.color_triangle = this.cLines.SelectedColor;
            Properties.Settings.Default.color_node = this.cNodes.SelectedColor;
            Properties.Settings.Default.Save();

            e.Cancel = true;
            this.Hide();
        }

        private void chkAutoUpdate_CheckedChanged(object sender, EventArgs e)
        {
            this.main_window.SetVisualisationAutoUpdate(this.chkAutoUpdate.Checked);
        }

        private void OnColorMapChanged(object sender, ColorComboTestApp.ColorChangeArgs e)
        {
            this.UpdatePreview();
        }

        protected override void OnResizeEnd(EventArgs e)
        {
            base.OnResizeEnd(e);
            this.UpdatePreview();
        }

    }
}
