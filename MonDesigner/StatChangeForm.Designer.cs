namespace MonDesigner
{
    partial class StatChangeForm
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
            this.scfChangeStatLabel = new System.Windows.Forms.Label();
            this.scfTableLayout = new System.Windows.Forms.TableLayoutPanel();
            this.scfChangeStatNumUpDown = new System.Windows.Forms.NumericUpDown();
            this.scfChangeStatButton = new System.Windows.Forms.Button();
            this.scfTableLayout.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.scfChangeStatNumUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // scfChangeStatLabel
            // 
            this.scfChangeStatLabel.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.scfTableLayout.SetColumnSpan(this.scfChangeStatLabel, 2);
            this.scfChangeStatLabel.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.scfChangeStatLabel.Location = new System.Drawing.Point(20, 2);
            this.scfChangeStatLabel.Name = "scfChangeStatLabel";
            this.scfChangeStatLabel.Size = new System.Drawing.Size(216, 25);
            this.scfChangeStatLabel.TabIndex = 0;
            this.scfChangeStatLabel.Text = "Change {who} {stat} to";
            // 
            // scfTableLayout
            // 
            this.scfTableLayout.ColumnCount = 2;
            this.scfTableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.scfTableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.scfTableLayout.Controls.Add(this.scfChangeStatLabel, 0, 0);
            this.scfTableLayout.Controls.Add(this.scfChangeStatNumUpDown, 0, 1);
            this.scfTableLayout.Controls.Add(this.scfChangeStatButton, 1, 1);
            this.scfTableLayout.Location = new System.Drawing.Point(12, 12);
            this.scfTableLayout.Name = "scfTableLayout";
            this.scfTableLayout.RowCount = 2;
            this.scfTableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 29F));
            this.scfTableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.scfTableLayout.Size = new System.Drawing.Size(256, 65);
            this.scfTableLayout.TabIndex = 1;
            // 
            // scfChangeStatNumUpDown
            // 
            this.scfChangeStatNumUpDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.scfChangeStatNumUpDown.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.scfChangeStatNumUpDown.Location = new System.Drawing.Point(54, 32);
            this.scfChangeStatNumUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.scfChangeStatNumUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.scfChangeStatNumUpDown.Name = "scfChangeStatNumUpDown";
            this.scfChangeStatNumUpDown.Size = new System.Drawing.Size(71, 29);
            this.scfChangeStatNumUpDown.TabIndex = 1;
            this.scfChangeStatNumUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // scfChangeStatButton
            // 
            this.scfChangeStatButton.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.scfChangeStatButton.Location = new System.Drawing.Point(131, 32);
            this.scfChangeStatButton.Name = "scfChangeStatButton";
            this.scfChangeStatButton.Size = new System.Drawing.Size(75, 29);
            this.scfChangeStatButton.TabIndex = 2;
            this.scfChangeStatButton.Text = "Change";
            this.scfChangeStatButton.UseVisualStyleBackColor = true;
            this.scfChangeStatButton.Click += new System.EventHandler(this.ChangeStatButton_Click);
            // 
            // StatChangeForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(278, 84);
            this.Controls.Add(this.scfTableLayout);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "StatChangeForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Change Stat";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.StatChangeForm_FormClosing);
            this.scfTableLayout.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.scfChangeStatNumUpDown)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private Label scfChangeStatLabel;
        private TableLayoutPanel scfTableLayout;
        private NumericUpDown scfChangeStatNumUpDown;
        private Button scfChangeStatButton;
    }
}