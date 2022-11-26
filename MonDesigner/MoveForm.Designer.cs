namespace MonDesigner
{
    partial class MoveForm
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
            this.vfTypeComboBox = new System.Windows.Forms.ComboBox();
            this.vfCategoryComboBox = new System.Windows.Forms.ComboBox();
            this.vfDescriptionTextBox = new System.Windows.Forms.TextBox();
            this.vfPPLabel = new System.Windows.Forms.Label();
            this.vfPowerLabel = new System.Windows.Forms.Label();
            this.vfAccuracyLabel = new System.Windows.Forms.Label();
            this.vfChooseMoveButton = new System.Windows.Forms.Button();
            this.vfMovesListBox = new System.Windows.Forms.ListBox();
            this.vfNoMoveButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // vfTypeComboBox
            // 
            this.vfTypeComboBox.FormattingEnabled = true;
            this.vfTypeComboBox.Items.AddRange(new object[] {
            "Normal",
            "Fighting",
            "Flying",
            "Poison",
            "Ground",
            "Rock",
            "Bug",
            "Ghost",
            "Steel",
            "Fire",
            "Water",
            "Grass",
            "Electric",
            "Psychic",
            "Ice",
            "Dragon",
            "Dark",
            "Fairy"});
            this.vfTypeComboBox.Location = new System.Drawing.Point(12, 12);
            this.vfTypeComboBox.Name = "vfTypeComboBox";
            this.vfTypeComboBox.Size = new System.Drawing.Size(138, 23);
            this.vfTypeComboBox.TabIndex = 1;
            this.vfTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.TypeComboBox_SelectedIndexChanged);
            // 
            // vfCategoryComboBox
            // 
            this.vfCategoryComboBox.FormattingEnabled = true;
            this.vfCategoryComboBox.Items.AddRange(new object[] {
            "Physical",
            "Special",
            "Status"});
            this.vfCategoryComboBox.Location = new System.Drawing.Point(12, 41);
            this.vfCategoryComboBox.Name = "vfCategoryComboBox";
            this.vfCategoryComboBox.Size = new System.Drawing.Size(138, 23);
            this.vfCategoryComboBox.TabIndex = 2;
            this.vfCategoryComboBox.SelectedIndexChanged += new System.EventHandler(this.CategoryComboBox_SelectedIndexChanged);
            // 
            // vfDescriptionTextBox
            // 
            this.vfDescriptionTextBox.Location = new System.Drawing.Point(156, 106);
            this.vfDescriptionTextBox.Multiline = true;
            this.vfDescriptionTextBox.Name = "vfDescriptionTextBox";
            this.vfDescriptionTextBox.ReadOnly = true;
            this.vfDescriptionTextBox.Size = new System.Drawing.Size(232, 178);
            this.vfDescriptionTextBox.TabIndex = 3;
            // 
            // vfPPLabel
            // 
            this.vfPPLabel.AutoSize = true;
            this.vfPPLabel.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.vfPPLabel.Location = new System.Drawing.Point(156, 41);
            this.vfPPLabel.Name = "vfPPLabel";
            this.vfPPLabel.Size = new System.Drawing.Size(35, 21);
            this.vfPPLabel.TabIndex = 4;
            this.vfPPLabel.Text = "PP: ";
            // 
            // vfPowerLabel
            // 
            this.vfPowerLabel.AutoSize = true;
            this.vfPowerLabel.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.vfPowerLabel.Location = new System.Drawing.Point(156, 12);
            this.vfPowerLabel.Name = "vfPowerLabel";
            this.vfPowerLabel.Size = new System.Drawing.Size(56, 21);
            this.vfPowerLabel.TabIndex = 5;
            this.vfPowerLabel.Text = "Power:";
            // 
            // vfAccuracyLabel
            // 
            this.vfAccuracyLabel.AutoSize = true;
            this.vfAccuracyLabel.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.vfAccuracyLabel.Location = new System.Drawing.Point(156, 70);
            this.vfAccuracyLabel.Name = "vfAccuracyLabel";
            this.vfAccuracyLabel.Size = new System.Drawing.Size(75, 21);
            this.vfAccuracyLabel.TabIndex = 6;
            this.vfAccuracyLabel.Text = "Accuracy:";
            // 
            // vfChooseMoveButton
            // 
            this.vfChooseMoveButton.Location = new System.Drawing.Point(297, 13);
            this.vfChooseMoveButton.Name = "vfChooseMoveButton";
            this.vfChooseMoveButton.Size = new System.Drawing.Size(93, 23);
            this.vfChooseMoveButton.TabIndex = 7;
            this.vfChooseMoveButton.Text = "Choose Move";
            this.vfChooseMoveButton.UseVisualStyleBackColor = true;
            this.vfChooseMoveButton.Click += new System.EventHandler(this.ChooseMoveButton_Click);
            // 
            // vfMovesListBox
            // 
            this.vfMovesListBox.FormattingEnabled = true;
            this.vfMovesListBox.ItemHeight = 15;
            this.vfMovesListBox.Location = new System.Drawing.Point(12, 70);
            this.vfMovesListBox.Name = "vfMovesListBox";
            this.vfMovesListBox.Size = new System.Drawing.Size(138, 214);
            this.vfMovesListBox.TabIndex = 8;
            this.vfMovesListBox.SelectedIndexChanged += new System.EventHandler(this.MoveList_SelectedIndexChanged);
            // 
            // vfNoMoveButton
            // 
            this.vfNoMoveButton.Location = new System.Drawing.Point(322, 68);
            this.vfNoMoveButton.Name = "vfNoMoveButton";
            this.vfNoMoveButton.Size = new System.Drawing.Size(65, 23);
            this.vfNoMoveButton.TabIndex = 9;
            this.vfNoMoveButton.Text = "No Move";
            this.vfNoMoveButton.UseVisualStyleBackColor = true;
            this.vfNoMoveButton.Click += new System.EventHandler(this.NoMoveButton_Click);
            // 
            // MoveForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(399, 297);
            this.Controls.Add(this.vfNoMoveButton);
            this.Controls.Add(this.vfMovesListBox);
            this.Controls.Add(this.vfChooseMoveButton);
            this.Controls.Add(this.vfAccuracyLabel);
            this.Controls.Add(this.vfPowerLabel);
            this.Controls.Add(this.vfPPLabel);
            this.Controls.Add(this.vfDescriptionTextBox);
            this.Controls.Add(this.vfCategoryComboBox);
            this.Controls.Add(this.vfTypeComboBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MoveForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Move Selector";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MoveForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private ComboBox vfTypeComboBox;
        private ComboBox vfCategoryComboBox;
        private TextBox vfDescriptionTextBox;
        private Label vfPPLabel;
        private Label vfPowerLabel;
        private Label vfAccuracyLabel;
        private Button vfChooseMoveButton;
        private ListBox vfMovesListBox;
        private Button vfNoMoveButton;
    }
}