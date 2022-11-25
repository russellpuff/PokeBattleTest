using System.Data;
using System.Data.Entity.Core.Objects;
using System.Data.SqlClient;
using System.Data.SQLite;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Windows.Forms;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        private record Mon(string DexID, int Level, bool HasTemplate, bool CustomTemplate, int Move1, int Move2, int Move3, int Move4);
        private record CustomTemplate(string Type1, string Type2, int HP, int ATK, int DEF, int SPATK, int SPDEF, int SPD);
        private readonly List<string> names;
        bool noEvent;
        public MainForm()
        {
            InitializeComponent();
            FixBrokenProgressBars();
            names = new();
            LoadPokemonComboBoxes();
            noEvent = false;
        }
        private void LoadPokemonComboBoxes()
        {
            try
            {
                string f = @"URI=file:" + Application.StartupPath + "resource/species.db";
                using var speciesdb = new SQLiteConnection(f);
                speciesdb.Open();
                using var command = new SQLiteCommand(speciesdb);
                command.CommandText = "SELECT NAME from SPECIES where DEX_ID <= 1010"; // All the way up to gen 9 baybee
                SQLiteDataReader dr = command.ExecuteReader(CommandBehavior.CloseConnection); // Set to close the db connection when the reader is closed. 
                if (!dr.HasRows) { throw new SQLiteException("The query gave a result without any rows."); }

                while (dr.Read())
                {
                    names.Add(dr.GetString(0)); // This SHOULD load them in order by DEX_ID
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
            if (noEvent) { return; }
            ComboBox box = (ComboBox)sender;
            if (box.SelectedIndex == 979 || box.SelectedIndex == 986)
            {
                string msg = "This pokemon is a placeholder to preserve dexID integrity, " +
                    "as the pokemon with this ID was removed from SV allegedly at the last " +
                    "minute to be in DLC. The application is going to abort the load for this data, " +
                    "you can select something else and it will continue to work as intended.";
                MessageBox.Show(msg, "Notice", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                return;
            }

            const string norow = "The query gave a result without any rows.";
            // These are for helping figure which side to modify. 
            Dictionary<string, ComboBox> cbs = new()
            {
                { "cbt11", mfType11ComboBox },
                { "cbt21", mfType21ComboBox },
                { "cbt12", mfType12ComboBox },
                { "cbt22", mfType22ComboBox },
                { "cbf1", mfForm1ComboBox },
                { "cbf2", mfForm2ComboBox }
            };
            Dictionary<string, Label> lbs = new()
            {
                {"lbhp1", mfHP1Label },
                {"lbat1", mfAtk1Label },
                {"lbde1", mfDef1Label },
                {"lbsa1", mfSpAtk1Label },
                {"lbsd1", mfSpDef1Label },
                {"lbsp1", mfSpd1Label },
                {"lbhp2", mfHP2Label },
                {"lbat2", mfAtk2Label },
                {"lbde2", mfDef2Label },
                {"lbsa2", mfSpAtk2Label },
                {"lbsd2", mfSpDef2Label },
                {"lbsp2", mfSpd2Label },
            };
            Dictionary<string, ColorProgressBar> pbs = new()
            {
                {"pbhp1", mfHP1ProgressBar },
                {"pbat1", mfAtk1ProgressBar },
                {"pbde1", mfDef1ProgressBar },
                {"pbsa1", mfSpAtk1ProgressBar },
                {"pbsd1", mfSpDef1ProgressBar },
                {"pbsp1", mfSpd1ProgressBar },
                {"pbhp2", mfHP2ProgressBar },
                {"pbat2", mfAtk2ProgressBar },
                {"pbde2", mfDef2ProgressBar },
                {"pbsa2", mfSpAtk2ProgressBar },
                {"pbsd2", mfSpDef2ProgressBar },
                {"pbsp2", mfSpd2ProgressBar },
            };

            string bx = box.Name == "mfPokemon1ComboBox" || box.Name == "mfForm1ComboBox" ? "1" : "2"; // The key required to make the above work.
            int dexID = box.SelectedIndex + 1;

            try
            {
                string f = @"URI=file:" + Application.StartupPath + "resource/species.db";
                using var speciesdb = new SQLiteConnection(f);
                speciesdb.Open();
                using var command = new SQLiteCommand(speciesdb);


                command.CommandText = box.Name.Contains("Form") && box.SelectedIndex != 0 ? // If a Form combobox sent this, make sure the user isn't switching back to the default form.
                    "SELECT * from SPECIES_OVERRIDE where FORM_NAME = \"" + cbs["cbf" + bx].Items[cbs["cbf" + bx].SelectedIndex].ToString() + "\"" : 
                    "SELECT * from SPECIES where DEX_ID = " + dexID.ToString();
                SQLiteDataReader dr = command.ExecuteReader();
                if (!dr.HasRows) { throw new SQLiteException(norow); }

                dr.Read();
                string overrideId = dr.GetInt32(0).ToString(); // This will fill overrideID with a usable value if species override was queried, otherwise it's filled with junk that doesn't get used. 
                string defaultForm = dr.GetString(2);
                // set type comboboxes
                cbs["cbt1" + bx].SelectedIndex = cbs["cbt1" + bx].Items.IndexOf(dr.GetString(3));
                cbs["cbt2" + bx].SelectedIndex = cbs["cbt2" + bx].Items.IndexOf(dr.GetString(4));
                dr.Close();

                bool changeStats = true;
                if (box.Name.Contains("Form"))
                {
                    command.CommandText = "SELECT * from STATS_OVERRIDE where OVERRIDE_ID = " + overrideId;
                    dr = command.ExecuteReader();
                    changeStats = dr.HasRows; // no results means there is no stats override
                } else
                {
                    command.CommandText = "SELECT * from BASE_STATS where DEX_ID = " + dexID.ToString();
                    dr = command.ExecuteReader();
                    if (!dr.HasRows) { throw new SQLiteException(norow); }
                }

                if (changeStats)
                {
                    // set base stats
                    dr.Read();
                    pbs["pbhp" + bx].Value = dr.GetInt32(1);
                    pbs["pbat" + bx].Value = dr.GetInt32(2);
                    pbs["pbde" + bx].Value = dr.GetInt32(3);
                    pbs["pbsa" + bx].Value = dr.GetInt32(4);
                    pbs["pbsd" + bx].Value = dr.GetInt32(5);
                    pbs["pbsp" + bx].Value = dr.GetInt32(6);

                    lbs["lbhp" + bx].Text = "Hp: " + dr.GetInt32(1).ToString();
                    lbs["lbat" + bx].Text = "Atk: " + dr.GetInt32(2).ToString();
                    lbs["lbde" + bx].Text = "Def: " + dr.GetInt32(3).ToString();
                    lbs["lbsa" + bx].Text = "SpAtk: " + dr.GetInt32(4).ToString();
                    lbs["lbsd" + bx].Text = "SpDef: " + dr.GetInt32(5).ToString();
                    lbs["lbsp" + bx].Text = "Spd: " + dr.GetInt32(6).ToString();

                    pbs["pbhp" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(1));
                    pbs["pbat" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(2));
                    pbs["pbde" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(3));
                    pbs["pbsa" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(4));
                    pbs["pbsd" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(5));
                    pbs["pbsp" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(6));
                }
                dr.Close();

                // set form combobox
                if (box.Name.Contains("Pokemon"))
                {
                    cbs["cbf" + bx].Items.Clear();
                    cbs["cbf" + bx].Items.Add(defaultForm);
                    command.CommandText = "SELECT FORM_NAME from SPECIES_OVERRIDE where DEX_ID = " + dexID.ToString();
                    dr = command.ExecuteReader();
                    while (dr.Read()) { cbs["cbf" + bx].Items.Add(dr.GetString(0)); }
                    dr.Close();
                    noEvent = true; // Block triggering this event programmatically. 
                    cbs["cbf" + bx].SelectedIndex = 0;
                    noEvent = false;
                }

                speciesdb.Close();

            } catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message +
                    "\nThe Pokemon was not loaded, but you can try again by reselecting it from the list.";
                MessageBox.Show(msg, "DB Error", MessageBoxButtons.OK);
                return;
            }
        }

        private void TypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        { // For flare :)
            ComboBox box = (ComboBox)sender;
            box.BackColor = ChooseTypeBoxColor(box.Items[box.SelectedIndex].ToString());
            switch (box.Items[box.SelectedIndex].ToString())
            {
                case "Normal":
                case "Ground":
                case "Bug":
                case "Steel":
                case "Fire":
                case "Grass":
                case "Electric":
                case "Ice":
                case "Fairy":
                case "NONE":
                    box.ForeColor = Color.Black; break;
                case "Fighting":
                case "Flying":
                case "Poison":
                case "Rock":
                case "Ghost":
                case "Water":
                case "Psychic":
                case "Dragon":
                case "Dark":
                    box.ForeColor = Color.White; break;
            }
        }

        private static Color ChooseProgressBarColor(int val)
        {
            return val switch
            {
                int n when (n >= 0 && n <= 29) => Color.Red,
                int n when (n >= 30 && n <= 59) => Color.Orange,
                int n when (n >= 60 && n <= 89) => Color.Yellow,
                int n when (n >= 90 && n <= 149) => Color.Green,
                int n when (n >= 150 && n <= 255) => Color.DeepSkyBlue,
                _ => Color.White
            };
        }

        private static Color ChooseTypeBoxColor(string? type)
        { 
            type ??= "For some reason the compiler thinks this can be null.";
            return type switch
            {
                "Normal" => Color.FromArgb(168, 168, 120),
                "Fighting" => Color.FromArgb(192, 48, 40),
                "Flying" => Color.FromArgb(168, 144, 240),
                "Poison" => Color.FromArgb(160, 64, 160),
                "Ground" => Color.FromArgb(224, 192, 104),
                "Rock" => Color.FromArgb(184, 160, 56),
                "Bug" => Color.FromArgb(168, 184, 32),
                "Ghost" => Color.FromArgb(112, 88, 152),
                "Steel" => Color.FromArgb(184, 184, 208),
                "Fire" => Color.FromArgb(240, 128, 48),
                "Water" => Color.FromArgb(104, 144, 240),
                "Grass" => Color.FromArgb(120, 200, 80),
                "Electric" => Color.FromArgb(248, 208, 48),
                "Psychic" => Color.FromArgb(248, 88, 136),
                "Ice" => Color.FromArgb(152, 216, 216),
                "Dragon" => Color.FromArgb(112, 56, 248),
                "Dark" => Color.FromArgb(112, 88, 72),
                "Fairy" => Color.FromArgb(238, 153, 172),
                _ => Color.White
            } ;
        }

        private void ProgressBars_MouseClick(object sender, EventArgs e)
        {
            ProgressBar pb = (ProgressBar)sender;
            // If custom template isn't checked, you can't change any values. Simple as. 
            if (pb.Name.Contains('1')) { if (!mfCustomForm1CheckBox.Checked) { return; } }
            else if (!mfCustomForm2CheckBox.Checked) { return; }

            mfCustomStatLabel.Text = "Set ";
            mfCustomStatLabel.Text += pb.Name.Contains('1') ? "player " : "opponent ";
            mfCustomStatLabel.Text += pb.Name switch
            {
                _ when pb.Name.Contains("mfHP") => "HP",
                _ when pb.Name.Contains("mfAtk") => "Atk",
                _ when pb.Name.Contains("mfDef") => "Def",
                _ when pb.Name.Contains("mfSpAtk") => "SpAtk",
                _ when pb.Name.Contains("mfSpDef") => "SpDef",
                _ when pb.Name.Contains("mfSpd") => "Spd",
                _ => "idk lol"
            };
            mfCustomStatLabel.Text += " to:";
            mfCustomStatLabel.Visible = mfCustomStatNumUpDown.Visible = mfChangeCustomStatButton.Visible = true;
        }

        private void CustomTemplateCheckBox_CheckChanged(object sender, EventArgs e)
        {
            mfCustomStatLabel.Visible = mfCustomStatNumUpDown.Visible = mfChangeCustomStatButton.Visible = false;
        }

        private void ChangeCustomStatButton_Click(object sender, EventArgs e)
        {
            Dictionary<string, Label> lbs = new()
            {
                {"mfHP1ProgressBar", mfHP1Label },
                {"mfAtk1ProgressBar", mfAtk1Label },
                {"mfDef1ProgressBar", mfDef1Label },
                {"mfSpAtk1ProgressBar", mfSpAtk1Label },
                {"mfSpDef1ProgressBar", mfSpDef1Label },
                {"mfSpd1ProgressBar", mfSpd1Label },
                {"mfHP2ProgressBar", mfHP2Label },
                {"mfAtk2ProgressBar", mfAtk2Label },
                {"mfDef2ProgressBar", mfDef2Label },
                {"mfSpAtk2ProgressBar", mfSpAtk2Label },
                {"mfSpDef2ProgressBar", mfSpDef2Label },
                {"mfSpd2ProgressBar", mfSpd2Label }
            };
        }

        private void SerializePokemon()
        {
            
        }

    }
}