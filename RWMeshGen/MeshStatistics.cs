using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace RWMeshGen
{
    public partial class MeshStatistics : Form
    {
        public int _spatial_nodes;
        public int _spatial_elements;
        public int _plain_nodes;
        public int _plain_elements;
        public TimeSpan _time_taken;

        public MeshStatistics()
        {
            this.InitializeComponent();
        }

        protected override void OnShown(EventArgs e)
        {
            this.lblPlainNodes.Text = this._plain_nodes.ToString("###,###,###,##0");
            this.lblPlainTriangles.Text = this._plain_elements.ToString("###,###,###,##0");
            this.lblSpatialNodes.Text = this._spatial_nodes.ToString("###,###,###,##0");
            this.lblSpatialElements.Text = this._spatial_elements.ToString("###,###,###,##0");
            this.lblTotalTime.Text = this._time_taken.ToString();

            base.OnShown(e);
        }

        private void lblSpatialElements_Click(object sender, EventArgs e)
        {

        }

        private void label7_Click(object sender, EventArgs e)
        {

        }
    }
}
