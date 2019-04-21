using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace RWMeshGen
{
    public partial class AboutBox : Form
    {
        public AboutBox()
        {
            this.InitializeComponent();
            this.label2.Text = string.Format(this.label2.Text, Application.ProductVersion);
        }
    }
}
