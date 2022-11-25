using System.Data;
using System.Data.Entity.Core.Objects;
using System.Data.SQLite;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Drawing.Drawing2D;
using System.Text.Json;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        private record Mon(string DexID, int Level, bool HasTemplate, bool CustomTemplate, int Move1, int Move2, int Move3, int Move4);
        private record CustomTemplate(string Type1, string Type2, int HP, int ATK, int DEF, int SPATK, int SPDEF, int SPD);
        private readonly List<string> names;
        public MainForm()
        {
            InitializeComponent();
            names = new();
            LoadPokemonComboBoxes();
        }
        private void LoadPokemonComboBoxes()
        {
            try
            {
                string f = @"URI=file:" + Application.StartupPath + "resource/species.db";
                using var speciesdb = new SQLiteConnection(f);
                speciesdb.Open();
                using var command = new SQLiteCommand(speciesdb);
                command.CommandText = "SELECT NAME from SPECIES where DEX_ID <= 905";
                SQLiteDataReader dr = command.ExecuteReader(CommandBehavior.CloseConnection); // Set to close the db connection when the reader is closed. 
                if (!dr.HasRows) { throw new SQLiteException("The query gave a result without any rows."); }

                while (dr.Read())
                {
                    names.Add(dr.GetString(1)); // This SHOULD load them in order by DEX_ID
                    mfPokemon1ComboBox.Items.Add(dr.GetString(0));
                    mfPokemon2ComboBox.Items.Add(dr.GetString(0));
                }
                dr.Close();
                if (names == null) { throw new SQLiteException("The application failed to save the query results."); }
            }
            catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message;
                var result = MessageBox.Show(msg, "DB Error", MessageBoxButtons.RetryCancel);
                if (result == DialogResult.Retry) { Application.Restart(); Environment.Exit(0); }
                else { Application.Exit(); }
            }
        }

        private void RunBattleButton_Click(object sender, EventArgs e)
        {
            // put code for saving the prepared mons and getting ready for PokeBattleTest
            SerializePokemon();

            Process pbt = new();
            pbt.StartInfo.FileName = Application.StartupPath + "PokeBattleTest.exe";
            pbt.Start();
            this.Hide();
            pbt.WaitForExit();
            this.Show();
        }

        private void PokemonComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = (ComboBox)sender;

            /*
             * 
             * YOU ARE HERE
             * CURRENTLY NEED TO DIFFERENTIATE BETWEEN WHICH SIDE IS BEING UPDATED
             * 
             * 
             */
        }

        private void SerializePokemon()
        {
            
        }

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
    }
}