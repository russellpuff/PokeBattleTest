using System.Drawing.Drawing2D;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        public class ColorProgressBar : ProgressBar // Overloads ProgressBar to provide a color changing effect.
        {
            public ColorProgressBar()
            {
                this.SetStyle(ControlStyles.UserPaint, true);
            }

            protected override void OnPaintBackground(PaintEventArgs pevent)
            {
                // None... Helps control the flicker.
            }

            protected override void OnPaint(PaintEventArgs e)
            {
                const int inset = 2; // A single inset value to control the sizing of the inner rect.

                using (Image offscreenImage = new Bitmap(this.Width, this.Height))
                {
                    using (Graphics offscreen = Graphics.FromImage(offscreenImage))
                    {
                        Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);

                        if (ProgressBarRenderer.IsSupported)
                            ProgressBarRenderer.DrawHorizontalBar(offscreen, rect);

                        rect.Inflate(new Size(-inset, -inset)); // Deflate inner rect.
                        rect.Width = (int)(rect.Width * ((double)this.Value / this.Maximum));
                        if (rect.Width == 0) rect.Width = 1; // Can't draw rec with width of 0.

                        LinearGradientBrush brush = new LinearGradientBrush(rect, this.BackColor, this.ForeColor, LinearGradientMode.Vertical);
                        offscreen.FillRectangle(brush, inset, inset, rect.Width, rect.Height);

                        e.Graphics.DrawImage(offscreenImage, 0, 0);
                    }
                }
            }
        }

        private void FixBrokenProgressBars()
        {
            // Designer ate my auto-generated code.
            this.mfHP1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfAtk1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfDef1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpAtk1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpDef1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpd1ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfHP2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfAtk2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfDef2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpAtk2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpDef2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfSpd2ProgressBar = new MonDesigner.MainForm.ColorProgressBar();
            this.mfPlayerTableLayout.Controls.Add(this.mfHP1ProgressBar, 1, 10);
            this.mfPlayerTableLayout.Controls.Add(this.mfAtk1ProgressBar, 1, 11);
            this.mfPlayerTableLayout.Controls.Add(this.mfDef1ProgressBar, 1, 12);
            this.mfPlayerTableLayout.Controls.Add(this.mfSpAtk1ProgressBar, 1, 13);
            this.mfPlayerTableLayout.Controls.Add(this.mfSpDef1ProgressBar, 1, 14);
            this.mfPlayerTableLayout.Controls.Add(this.mfSpd1ProgressBar, 1, 15);
            this.mfRivalTableLayout.Controls.Add(this.mfHP2ProgressBar, 1, 10);
            this.mfRivalTableLayout.Controls.Add(this.mfAtk2ProgressBar, 1, 11);
            this.mfRivalTableLayout.Controls.Add(this.mfDef2ProgressBar, 1, 12);
            this.mfRivalTableLayout.Controls.Add(this.mfSpAtk2ProgressBar, 1, 13);
            this.mfRivalTableLayout.Controls.Add(this.mfSpDef2ProgressBar, 1, 14);
            this.mfRivalTableLayout.Controls.Add(this.mfSpd2ProgressBar, 1, 15);
#nullable disable
            // 
            // mfHP1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfHP1ProgressBar, 3);
            this.mfHP1ProgressBar.Location = new System.Drawing.Point(96, 298);
            this.mfHP1ProgressBar.Maximum = 255;
            this.mfHP1ProgressBar.Minimum = 1;
            this.mfHP1ProgressBar.Name = "mfHP1ProgressBar";
            this.mfHP1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfHP1ProgressBar.TabIndex = 9;
            this.mfHP1ProgressBar.Value = 1;
            this.mfHP1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfAtk1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfAtk1ProgressBar, 3);
            this.mfAtk1ProgressBar.Location = new System.Drawing.Point(96, 328);
            this.mfAtk1ProgressBar.Maximum = 255;
            this.mfAtk1ProgressBar.Minimum = 1;
            this.mfAtk1ProgressBar.Name = "mfAtk1ProgressBar";
            this.mfAtk1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfAtk1ProgressBar.TabIndex = 16;
            this.mfAtk1ProgressBar.Value = 1;
            this.mfAtk1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfDef1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfDef1ProgressBar, 3);
            this.mfDef1ProgressBar.Location = new System.Drawing.Point(96, 358);
            this.mfDef1ProgressBar.Maximum = 255;
            this.mfDef1ProgressBar.Minimum = 1;
            this.mfDef1ProgressBar.Name = "mfDef1ProgressBar";
            this.mfDef1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfDef1ProgressBar.TabIndex = 17;
            this.mfDef1ProgressBar.Value = 1;
            this.mfDef1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpAtk1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfSpAtk1ProgressBar, 3);
            this.mfSpAtk1ProgressBar.Location = new System.Drawing.Point(96, 388);
            this.mfSpAtk1ProgressBar.Maximum = 255;
            this.mfSpAtk1ProgressBar.Minimum = 1;
            this.mfSpAtk1ProgressBar.Name = "mfSpAtk1ProgressBar";
            this.mfSpAtk1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpAtk1ProgressBar.TabIndex = 18;
            this.mfSpAtk1ProgressBar.Value = 1;
            this.mfSpAtk1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpDef1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfSpDef1ProgressBar, 3);
            this.mfSpDef1ProgressBar.Location = new System.Drawing.Point(96, 418);
            this.mfSpDef1ProgressBar.Maximum = 255;
            this.mfSpDef1ProgressBar.Minimum = 1;
            this.mfSpDef1ProgressBar.Name = "mfSpDef1ProgressBar";
            this.mfSpDef1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpDef1ProgressBar.TabIndex = 19;
            this.mfSpDef1ProgressBar.Value = 1;
            this.mfSpDef1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpd1ProgressBar
            // 
            this.mfPlayerTableLayout.SetColumnSpan(this.mfSpd1ProgressBar, 3);
            this.mfSpd1ProgressBar.Location = new System.Drawing.Point(96, 448);
            this.mfSpd1ProgressBar.Maximum = 255;
            this.mfSpd1ProgressBar.Minimum = 1;
            this.mfSpd1ProgressBar.Name = "mfSpd1ProgressBar";
            this.mfSpd1ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpd1ProgressBar.TabIndex = 20;
            this.mfSpd1ProgressBar.Value = 1;
            this.mfSpd1ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfHP2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfHP2ProgressBar, 3);
            this.mfHP2ProgressBar.Location = new System.Drawing.Point(96, 298);
            this.mfHP2ProgressBar.Maximum = 255;
            this.mfHP2ProgressBar.Minimum = 1;
            this.mfHP2ProgressBar.Name = "mfHP2ProgressBar";
            this.mfHP2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfHP2ProgressBar.TabIndex = 9;
            this.mfHP2ProgressBar.Value = 1;
            this.mfHP2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfAtk2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfAtk2ProgressBar, 3);
            this.mfAtk2ProgressBar.Location = new System.Drawing.Point(96, 328);
            this.mfAtk2ProgressBar.Maximum = 255;
            this.mfAtk2ProgressBar.Minimum = 1;
            this.mfAtk2ProgressBar.Name = "mfAtk2ProgressBar";
            this.mfAtk2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfAtk2ProgressBar.TabIndex = 16;
            this.mfAtk2ProgressBar.Value = 1;
            this.mfAtk2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfDef2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfDef2ProgressBar, 3);
            this.mfDef2ProgressBar.Location = new System.Drawing.Point(96, 358);
            this.mfDef2ProgressBar.Maximum = 255;
            this.mfDef2ProgressBar.Minimum = 1;
            this.mfDef2ProgressBar.Name = "mfDef2ProgressBar";
            this.mfDef2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfDef2ProgressBar.TabIndex = 17;
            this.mfDef2ProgressBar.Value = 1;
            this.mfDef2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpAtk2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfSpAtk2ProgressBar, 3);
            this.mfSpAtk2ProgressBar.Location = new System.Drawing.Point(96, 388);
            this.mfSpAtk2ProgressBar.Maximum = 255;
            this.mfSpAtk2ProgressBar.Minimum = 1;
            this.mfSpAtk2ProgressBar.Name = "mfSpAtk2ProgressBar";
            this.mfSpAtk2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpAtk2ProgressBar.TabIndex = 18;
            this.mfSpAtk2ProgressBar.Value = 1;
            this.mfSpAtk2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpDef2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfSpDef2ProgressBar, 3);
            this.mfSpDef2ProgressBar.Location = new System.Drawing.Point(96, 418);
            this.mfSpDef2ProgressBar.Maximum = 255;
            this.mfSpDef2ProgressBar.Minimum = 1;
            this.mfSpDef2ProgressBar.Name = "mfSpDef2ProgressBar";
            this.mfSpDef2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpDef2ProgressBar.TabIndex = 19;
            this.mfSpDef2ProgressBar.Value = 1;
            this.mfSpDef2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
            // 
            // mfSpd2ProgressBar
            // 
            this.mfRivalTableLayout.SetColumnSpan(this.mfSpd2ProgressBar, 3);
            this.mfSpd2ProgressBar.Location = new System.Drawing.Point(96, 448);
            this.mfSpd2ProgressBar.Maximum = 255;
            this.mfSpd2ProgressBar.Minimum = 1;
            this.mfSpd2ProgressBar.Name = "mfSpd2ProgressBar";
            this.mfSpd2ProgressBar.Size = new System.Drawing.Size(146, 24);
            this.mfSpd2ProgressBar.TabIndex = 20;
            this.mfSpd2ProgressBar.Value = 1;
            this.mfSpd2ProgressBar.Click += new System.EventHandler(this.ProgressBars_MouseClick);
#nullable enable
        }
    }
}