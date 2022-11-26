using System.Configuration;
using System.Data.SQLite;
using System.Diagnostics.CodeAnalysis;

namespace MonDesigner
{
    public partial class MoveForm : Form
    {
        public int MoveID;
        public string MoveName;
        bool closeOK;
        bool noEvent;
        public MoveForm()
        {
            InitializeComponent();
            MoveID = 0;
            MoveName = "";
            closeOK = noEvent = false;
            vfTypeComboBox.SelectedIndex = 0;
            vfCategoryComboBox.SelectedIndex = 0;
            vfMovesListBox.SelectedIndex = 0;
        }

        private void MoveForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.DialogResult = closeOK ? DialogResult.OK : DialogResult.Cancel;
        }

        private void NoMoveButton_Click(object sender, EventArgs e) 
        { closeOK = true; MoveName = ""; this.Close(); }

        private void ChooseMoveButton_Click(object sender, EventArgs e)
        {
            if (vfMovesListBox.SelectedIndex == -1) { return; }
#nullable disable
            MoveName = vfMovesListBox.SelectedItem.ToString();
#nullable enable
            closeOK = true;
            this.Close();
        }

        private void TypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        { // This event secretly does nothing but reset the category combobox. Category does the real work when its event fires. 
            if (vfCategoryComboBox.SelectedIndex == 0)
            { CategoryComboBox_SelectedIndexChanged(null, null);
            } else { vfCategoryComboBox.SelectedIndex = 0; }
        }

        private void CategoryComboBox_SelectedIndexChanged(object? sender, EventArgs? e)
        {
            try
            {
                noEvent = true;
                vfMovesListBox.Items.Clear();
                noEvent = false;
                string f = @"URI=file:" + Application.StartupPath + "resource/move.db";
                using var speciesdb = new SQLiteConnection(f);
                speciesdb.Open();
                using var command = new SQLiteCommand(speciesdb);
                command.CommandText = "select NAME from MOVE WHERE TYPE = \"" + vfTypeComboBox.Text.ToUpper() +
                    "\" AND CATEGORY = \"" + vfCategoryComboBox.Text.ToUpper() + "\"";
                SQLiteDataReader dr = command.ExecuteReader(System.Data.CommandBehavior.CloseConnection);
                if(!dr.HasRows) { throw new SQLiteException("The query gave a result without any rows."); }
                while (dr.Read()) { vfMovesListBox.Items.Add(dr.GetString(0)); }
                vfMovesListBox.Refresh();
                dr.Close();
                if (vfMovesListBox.Items.Count > 0) { vfMovesListBox.SelectedIndex = 0; }
            }
            catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message +
                    "\nThe window will now close. Try re-opening it after the problem is fixed.";
                MessageBox.Show(msg, "DB Error", MessageBoxButtons.OK);
                this.Close();
                return;
            }
        }

        private void MoveList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (noEvent) { return; }
            try
            {
                string f = @"URI=file:" + Application.StartupPath + "resource/move.db";
                using var movedb = new SQLiteConnection(f);
                movedb.Open();
                using var command = new SQLiteCommand(movedb);
                command.CommandText = "SELECT MOVE_ID, POWER_POINTS, POWER, ACCURACY, ACCURACY_MUTABLE, PRIORITY, DESCRIPTION " +
                    "from MOVE WHERE name = \"" + vfMovesListBox.SelectedItem.ToString() + "\"";
                SQLiteDataReader dr = command.ExecuteReader(System.Data.CommandBehavior.CloseConnection);
                if (!dr.HasRows) { throw new SQLiteException("The query gave a result without any rows."); }

                dr.Read();
                vfPowerLabel.Text = "Power: " + (dr.GetInt32(2) > 0 ? dr.GetInt32(2).ToString() : "—");
                vfPPLabel.Text = "PP: " + dr.GetInt32(1).ToString();
                vfAccuracyLabel.Text = "Accuracy: " + (dr.GetBoolean(4) ? dr.GetInt32(3).ToString() : "—");

                vfDescriptionTextBox.Text = "";
                if (dr.GetInt32(5) != 0)
                { vfDescriptionTextBox.Text = "Priority of " + (dr.GetInt32(5) > 0 ? "+" : "") 
                        + dr.GetInt32(5).ToString() + ". "; 
                }
                vfDescriptionTextBox.Text += dr[6].GetType() == typeof(DBNull) ? "" : dr.GetString(6);
                MoveID = dr.GetInt32(0);
                dr.Close();
            }
            catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message +
                    "\nThe window will now close. Try re-opening it after the problem is fixed.";
                MessageBox.Show(msg, "DB Error", MessageBoxButtons.OK);
                this.Close();
                return;
            }
        }
    }
}
