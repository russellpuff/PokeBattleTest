namespace MonDesigner
{
    partial class MainForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mfRunBattleButton = new System.Windows.Forms.Button();
            this.mfJokePictureBox = new System.Windows.Forms.PictureBox();
            this.mfJokeLabel = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.mfJokePictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // mfRunBattleButton
            // 
            this.mfRunBattleButton.Location = new System.Drawing.Point(12, 461);
            this.mfRunBattleButton.Name = "mfRunBattleButton";
            this.mfRunBattleButton.Size = new System.Drawing.Size(122, 26);
            this.mfRunBattleButton.TabIndex = 0;
            this.mfRunBattleButton.Text = "Run PokeBattleTest";
            this.mfRunBattleButton.UseVisualStyleBackColor = true;
            this.mfRunBattleButton.Click += new System.EventHandler(this.RunBattleButton_Click);
            // 
            // mfJokePictureBox
            // 
            this.mfJokePictureBox.Image = global::MonDesigner.Properties.Resources.joke;
            this.mfJokePictureBox.Location = new System.Drawing.Point(12, 124);
            this.mfJokePictureBox.Name = "mfJokePictureBox";
            this.mfJokePictureBox.Size = new System.Drawing.Size(369, 277);
            this.mfJokePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.mfJokePictureBox.TabIndex = 1;
            this.mfJokePictureBox.TabStop = false;
            // 
            // mfJokeLabel
            // 
            this.mfJokeLabel.Font = new System.Drawing.Font("Segoe Print", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.mfJokeLabel.Location = new System.Drawing.Point(12, 9);
            this.mfJokeLabel.Name = "mfJokeLabel";
            this.mfJokeLabel.Size = new System.Drawing.Size(369, 112);
            this.mfJokeLabel.TabIndex = 2;
            this.mfJokeLabel.Text = "\"You seriously made a completely separate application in a different language jus" +
    "t to make template designing a little easier?\"\r\nMe:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe UI", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.label1.Location = new System.Drawing.Point(461, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(161, 30);
            this.label1.TabIndex = 3;
            this.label1.Text = "Player Pokemon";
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.label3.Location = new System.Drawing.Point(387, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(1, 478);
            this.label3.TabIndex = 5;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1188, 499);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.mfJokeLabel);
            this.Controls.Add(this.mfJokePictureBox);
            this.Controls.Add(this.mfRunBattleButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Mon Designer";
            ((System.ComponentModel.ISupportInitialize)(this.mfJokePictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Button mfRunBattleButton;
        private PictureBox mfJokePictureBox;
        private Label mfJokeLabel;
        private Label label1;
        private Label label3;
    }
}