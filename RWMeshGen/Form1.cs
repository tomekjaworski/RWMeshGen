using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace RWMeshGen
{
    public partial class Form1 : Form
    {
        public bool is_generated;
        float[] ring_radiuses;
        List<Element> elements;

        List<Node> node_list;
        public Node[][] nodes;
        int selected_element;


        public Form1()
        {
            this.InitializeComponent();
            this.is_generated = false;

            this.numRadius.Value = Properties.Settings.Default.__radius;
            this.numDensity.Value = Properties.Settings.Default.__density;
            this.elements = new List<Element>();
            this.selected_element = 0;
        }

        Pen pen_black = new Pen(Color.Black, 1);

        float radius, density;
        int number_of_rings;


        private void button1_Click(object sender, EventArgs e)
        {
            this.radius = (float)this.numRadius.Value;
            this.density = (float)this.numDensity.Value;

            this.number_of_rings = (int)Math.Ceiling(this.radius/ this.density);
            PointF center = new PointF(this.radius, this.radius);
            this.nodes = new Node[this.number_of_rings + 1][];
            this.node_list = new List<Node>();

            this.ring_radiuses= new float[this.number_of_rings+1];

            // węzeł w środku
            this.nodes[0] = new Node[1];
            this.AddNode(0, 0, 0);

            // węzły na kolejnych pierścieniach
            for (int ring_no = 1; ring_no <= this.number_of_rings; ring_no++)
            {
                float rad = (this.radius / (float)this.number_of_rings) * (float)ring_no;
                this.ring_radiuses[ring_no] = rad;
                float len = 2.0f * (float)Math.PI * rad;

                int nodes_on_ring = (int)Math.Floor(len / this.density);
                this.nodes[ring_no] = new Node[nodes_on_ring];

                float _2PI = 2.0f * (float)Math.PI;
                for (int inode = 0; inode < nodes_on_ring; inode++)
                {
                    float angle = _2PI * ((float)inode / (float)nodes_on_ring);
                    this.AddNode(ring_no, rad, angle);
                }

            }

            // łączenie węzłów
            // najpierw połącz węzeł środku
            this.elements.Clear();
            Node center_node = this.nodes[0][0];
            for (int inode = 0; inode < this.nodes[1].Length; inode++)
            {
                Node n1 = this.GetNode(1, inode);
                Node n2 = this.GetNode(1, inode+1);
                this.elements.Add(new Element(center_node, n1, n2)); 
            }

            // teraz reszta
            
            for (int iring = 1; iring < this.number_of_rings; iring++)
            {
                int r = 0; // numer węzła na pierścieniu i
                int r1=0; // numer węzła na pierścieniu i+1
                while ((r < this.nodes[iring].Length) || (r1 < this.nodes[iring + 1].Length))
                {
                    Node n1 = this.GetNode(iring, r);
                    Node n2 = this.GetNode(iring + 1, r1);

                    Node n3a = this.GetNode(iring, r + 1);
                    Node n3b = this.GetNode(iring + 1, r1 + 1);

                    float d1 = Node.Distance(n1, n3b);
                    float d2 = Node.Distance(n2, n3a);

                    if (d1 < d2)
                    {
                        this.elements.Add(new Element(n1, n2, n3b));
                        r1++;
                    }
                    else
                    {
                        this.elements.Add(new Element(n1, n2, n3a)); ;
                        r++;
                    }
                }

                
            }

            this.is_generated = true;
            this.selected_element = 0;
            this.panel1.Invalidate();

        }

        private void AddNode(int ring_number, float radius, float angle)
        {
            for (int i = 0; i < this.nodes[ring_number].Length; i++)
                if (this.nodes[ring_number][i] == null)
                {
                    Node n = new Node(ring_number, radius, angle);
                    n.id = this.node_list.Count + 1;
                    this.nodes[ring_number][i] = n;
                    this.node_list.Add(n);
                    return;
                }

            throw new Exception("To many nodes on ring");
        }

        private Node GetNode(int ring_number, int node_number)
        {
            return this.nodes[ring_number][node_number % (this.nodes[ring_number].Length)];
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            if (!this.is_generated)
            {
                e.Graphics.FillRectangle(SystemBrushes.Control, new Rectangle(Point.Empty, this.panel1.Size));
                return;
            }

            Graphics g = e.Graphics;
            PointF center = new PointF(this.panel1.Size.Width / 2.0f, this.panel1.Size.Height / 2.0f);
            SizeF size = new SizeF(this.panel1.Size);

            float diameter = 2.0f * this.radius;
            float new_scale_x = (size.Width - 2 * 4) / diameter;
            float new_scale_y = (size.Height - 2 * 4) / diameter;

            g.FillRectangle(Brushes.White, new Rectangle(Point.Empty, this.panel1.Size));
            

            Matrix m = new Matrix();
            m.Translate(0, size.Height);
            m.Scale(1.0f, -1.0f);
            m.Translate(0, 2);
            m.Translate(4, 4);
            float scale = Math.Max(Math.Min(new_scale_x, new_scale_y), 0.1f);
            m.Scale(scale,scale);
            g.Transform = m;

            this.pen_black.Width = 1 / scale;
            
            ////
            g.DrawLine(Pens.Silver, 0, 0, diameter, 0);
            g.DrawLine(Pens.Silver, 0, 0, 0, diameter);

            ////
            /*
            g.DrawEllipse(Pens.Gray, 0, 0, diameter, diameter);

            for (int i = 1; i <= this.number_of_rings; i++)
            {
                float rad = this.ring_radiuses[i];
                g.DrawEllipse(this.pen_black, radius - rad, radius - rad, 2 * rad, 2 * rad);
            }
            */
            /*
            float pw = 1.5f / scale;
            for (int i = 0; i <= this.number_of_rings; i++)
            {
                PointF[] points = this.nodes[i];
                if (points == null)
                    continue;

                for (int j = 0; j < points.Length; j++)
                {
                    PointF p = points[j];
                    g.FillRectangle(Brushes.Red, p.X-pw, p.Y-pw, pw*2,pw*2);
                }
            }*/

            for (int i = 0; i < this.elements.Count; i++)
            {
                Element elem = this.elements[i];
                PointF[] pf = elem.ToPointFArray(this.radius, this.radius);
                float ratio = elem.GetEdgeRatio();
                Color ratio_color = Color.FromArgb((int)(255.0f * ratio), (int)(255.0f * ratio), (int)(255.0f * ratio));
                SolidBrush sb = new SolidBrush(ratio_color);

                if (i == this.selected_element)
                    g.FillPolygon(Brushes.Olive, pf);
                else
                    g.FillPolygon(sb, pf);
                g.DrawPolygon(this.pen_black, pf);
                sb.Dispose();

                //PointF mc = new PointF((pf[0].X + pf[1].X + pf[2].X) / 3.0f, (pf[0].Y + pf[1].Y + pf[2].Y) / 3.0f);
                //string s = elem.GetEdgeRatio().ToString("N2");
                //SizeF ssize = g.MeasureString(s, this.panel1.Font);
                //g.DrawString(s, this.panel1.Font, Brushes.Black, mc.X - ssize.Width / 2, mc.Y - ssize.Height / 2);
            }



            Node[] narr = this.node_list.ToArray();
            float pw = 1.5f / scale;

            for (int i = 0; i < narr.Length; i++)
            {
                Node n = narr[i];
                g.FillRectangle(Brushes.Red, this.radius + n.x - pw, this.radius + n.y - pw, pw * 2, pw * 2);
            }

        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            this.panel1.Invalidate();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Properties.Settings.Default.__radius = this.numRadius.Value;
            Properties.Settings.Default.__density = this.numDensity.Value;
            Properties.Settings.Default.Save();
        }

        private void btnButtonLeft_Click(object sender, EventArgs e)
        {
            if (sender == this.btnButtonLeft)
            {
                if (this.selected_element > 0)
                    this.selected_element--;
            }else

            if (this.selected_element < this.elements.Count - 1)
                this.selected_element++;

            this.panel1.Invalidate();
            this.label4.Text = this.elements[this.selected_element].GetEdgeRatio().ToString("N4");
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }

    public class Node
    {
        public int ring;
        public float x, y;
        public int id;
        private float radius;

        public Node(int ring, float radius, float angle)
        {
            this.ring = ring;
            this.radius = radius;
            this.SetAngle(angle);

        }

        public void SetAngle(float angle)
        {
            this.x = (float)Math.Cos(angle) * this.radius;
            this.y = (float)Math.Sin(angle) * this.radius;
        }


        public override string ToString()
        {
            return this.id.ToString();
        }

        public PointF ToPointF(float xoffset, float yoffset)
        {
            return new PointF(xoffset+this.x, yoffset+this.y);
        }

        public static float Distance(Node n1, Node n2)
        {
            float dx = n1.x-n2.x;
            float dy = n1.y-n2.y;
            return (float)Math.Sqrt(dx * dx + dy * dy);
        }
    }


    public class Element
    {
        public Node node1;
        public Node node2;
        public Node node3;

        public Element(Node n1, Node n2, Node n3)
        {
            this.node1 = n1;
            this.node2 = n2;
            this.node3 = n3;
        }

        public float GetEdgeRatio()
        {
            float d12x = (this.node1.x - this.node2.x);
            float d12y = (this.node1.y - this.node2.y);
            float d23x = (this.node2.x - this.node3.x);
            float d23y = (this.node2.y - this.node3.y);
            float d31x = (this.node3.x - this.node1.x);
            float d31y = (this.node3.y - this.node1.y);

            float d12 = (d12x * d12x + d12y * d12y);
            float d23 = (d23x * d23x + d23y * d23y);
            float d31 = (d31x * d31x + d31y * d31y);

            float d1 = (d12 > d23) ? d23 / d12 : d12 / d23;
            float d2 = (d31 > d12) ? d12 / d31 : d31 / d12;

            return Math.Abs(Math.Min(d1, d2));
        }


        public override string ToString()
        {
            return string.Format("{0} {1} {2} Equilateral={3:N3}", this.node1, this.node2, this.node3, this.GetEdgeRatio());
        }

        public PointF[] ToPointFArray(float xoffset, float yoffset)
        {
            return new PointF[]
            {
                this.node1.ToPointF(xoffset,yoffset),
                this.node2.ToPointF(xoffset,yoffset),
                this.node3.ToPointF(xoffset,yoffset)
            };
        }
    }
}
