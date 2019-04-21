namespace RWMeshGen
{
    partial class MeshStatistics
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.lblSpatialElements = new System.Windows.Forms.Label();
            this.lblSpatialNodes = new System.Windows.Forms.Label();
            this.lblPlainTriangles = new System.Windows.Forms.Label();
            this.lblPlainNodes = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.lblTotalTime = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.button1.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button1.Location = new System.Drawing.Point(141, 282);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "&Close";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label1.ForeColor = System.Drawing.Color.DarkBlue;
            this.label1.Location = new System.Drawing.Point(29, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(313, 24);
            this.label1.TabIndex = 1;
            this.label1.Text = "Mesh has been correctly generated!";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(30, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(88, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Plain mesh (cap):";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(30, 121);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(70, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Spatial mesh:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(42, 73);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(91, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Number of nodes:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(42, 92);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(101, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Number of triangles:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(42, 140);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(91, 13);
            this.label8.TabIndex = 8;
            this.label8.Text = "Number of nodes:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(42, 159);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(104, 13);
            this.label9.TabIndex = 9;
            this.label9.Text = "Number of elements:";
            // 
            // lblSpatialElements
            // 
            this.lblSpatialElements.AutoSize = true;
            this.lblSpatialElements.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.lblSpatialElements.Location = new System.Drawing.Point(159, 159);
            this.lblSpatialElements.Name = "lblSpatialElements";
            this.lblSpatialElements.Size = new System.Drawing.Size(41, 13);
            this.lblSpatialElements.TabIndex = 10;
            this.lblSpatialElements.Text = "label4";
            this.lblSpatialElements.Click += new System.EventHandler(this.lblSpatialElements_Click);
            // 
            // lblSpatialNodes
            // 
            this.lblSpatialNodes.AutoSize = true;
            this.lblSpatialNodes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.lblSpatialNodes.Location = new System.Drawing.Point(159, 140);
            this.lblSpatialNodes.Name = "lblSpatialNodes";
            this.lblSpatialNodes.Size = new System.Drawing.Size(41, 13);
            this.lblSpatialNodes.TabIndex = 11;
            this.lblSpatialNodes.Text = "label7";
            // 
            // lblPlainTriangles
            // 
            this.lblPlainTriangles.AutoSize = true;
            this.lblPlainTriangles.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.lblPlainTriangles.Location = new System.Drawing.Point(159, 92);
            this.lblPlainTriangles.Name = "lblPlainTriangles";
            this.lblPlainTriangles.Size = new System.Drawing.Size(48, 13);
            this.lblPlainTriangles.TabIndex = 12;
            this.lblPlainTriangles.Text = "label10";
            // 
            // lblPlainNodes
            // 
            this.lblPlainNodes.AutoSize = true;
            this.lblPlainNodes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.lblPlainNodes.Location = new System.Drawing.Point(159, 73);
            this.lblPlainNodes.Name = "lblPlainNodes";
            this.lblPlainNodes.Size = new System.Drawing.Size(48, 13);
            this.lblPlainNodes.TabIndex = 13;
            this.lblPlainNodes.Text = "label11";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(30, 187);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 13);
            this.label4.TabIndex = 14;
            this.label4.Text = "Spatial mesh:";
            // 
            // lblTotalTime
            // 
            this.lblTotalTime.AutoSize = true;
            this.lblTotalTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.lblTotalTime.Location = new System.Drawing.Point(159, 206);
            this.lblTotalTime.Name = "lblTotalTime";
            this.lblTotalTime.Size = new System.Drawing.Size(41, 13);
            this.lblTotalTime.TabIndex = 16;
            this.lblTotalTime.Text = "label4";
            this.lblTotalTime.Click += new System.EventHandler(this.label7_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(42, 206);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(115, 13);
            this.label10.TabIndex = 15;
            this.label10.Text = "Total time of operation:";
            // 
            // MeshStatistics
            // 
            this.AcceptButton = this.button1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(356, 317);
            this.Controls.Add(this.lblTotalTime);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblPlainNodes);
            this.Controls.Add(this.lblPlainTriangles);
            this.Controls.Add(this.lblSpatialNodes);
            this.Controls.Add(this.lblSpatialElements);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "MeshStatistics";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Mesh statistics";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label lblSpatialElements;
        private System.Windows.Forms.Label lblSpatialNodes;
        private System.Windows.Forms.Label lblPlainTriangles;
        private System.Windows.Forms.Label lblPlainNodes;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label lblTotalTime;
        private System.Windows.Forms.Label label10;
    }
}