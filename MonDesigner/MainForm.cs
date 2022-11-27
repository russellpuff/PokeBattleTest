using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity.Core.Common.CommandTrees.ExpressionBuilder;
using System.Data.SQLite;
using System.Diagnostics;
using System.Media;
using System.Text.RegularExpressions;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        // ----------------------------------------------------PRIVATES---------------------------------------------------------------
        private record Mon(string Who, int DexID, int Level, string Form, int Nature, int Move1, int Move2, int Move3, int Move4,
            int HPEV, int AtkEV, int DefEV, int SpAtkEV, int SpDefEV, int SpdEV);
        private record CustomTemplate(string Who, string Type1, string Type2, int HP, int ATK, int DEF, int SPATK, int SPDEF, int SPD);
        private bool noEvent, noTypeEdit1, noTypeEdit2;
        private int[] playerMoves;
        private int[] rivalMoves;
        private int[] playerEVs; // These are necessary to maintain the EV cap programmatically. 
        private int[] rivalEVs; // 
        private SQLiteConnection db;
        // ---------------------------------------------------------------------------------------------------------------------------
        #region Initialize
        public MainForm()
        {
            InitializeComponent();
            FixBrokenProgressBars();
            string f = @"URI=file:" + Application.StartupPath + "resource/species.db"; // This form will only ever access species.db 
            db = new SQLiteConnection(f);
            LoadPokemonComboBoxes();
            LoadNatureComboBoxes();
            noEvent = false;
            noTypeEdit1 = noTypeEdit2 = true;
            playerMoves = new int[4] { 0, 0, 0, 0};
            rivalMoves = new int[4] { 0, 0, 0, 0 };
            playerEVs = new int[6];
            rivalEVs = new int[6];
        }
        private void LoadPokemonComboBoxes()
        {
            try
            {
                string cmd = "SELECT NAME from SPECIES where DEX_ID <= 1010 ORDER BY DEX_ID";
                SQLiteDataReader dr = GenerateDataReader(cmd);
                while (dr.Read())
                { 
                    mfPokemon1ComboBox.Items.Add(dr.GetString(0));
                    mfPokemon2ComboBox.Items.Add(dr.GetString(0));
                }
                dr.Close();
            }
            catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message;
                var result = MessageBox.Show(msg, "DB Error", MessageBoxButtons.RetryCancel);
                if (result == DialogResult.Retry) { Application.Restart(); Environment.Exit(0); }
                else { Application.Exit(); }
            }
        }

        private void LoadNatureComboBoxes()
        {
            try
            {
                string cmd = "SELECT NAME, INCREASED_STAT, DECREASED_STAT from NATURE";
                SQLiteDataReader dr = GenerateDataReader(cmd);
                while (dr.Read())
                {
                    string n = dr.GetString(0) + " (+" + dr.GetString(1) + ", -" + dr.GetString(2) + ")";
                    mfNature1ComboBox.Items.Add(n);
                    mfNature2ComboBox.Items.Add(n);
                }
                dr.Close();

            }
            catch (SQLiteException ex) 
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message;
                var result = MessageBox.Show(msg, "DB Error", MessageBoxButtons.RetryCancel);
                if (result == DialogResult.Retry) { Application.Restart(); Environment.Exit(0); }
                else { Application.Exit(); }
            }
        }
        #endregion
        #region PokemonChanged
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
            string bx = box.Name == "mfPokemon1ComboBox" || box.Name == "mfForm1ComboBox" ? "1" : "2"; // The key required to make the dictionaries work.
            int dexID = box.Name.Contains('1') ? mfPokemon1ComboBox.SelectedIndex + 1 : mfPokemon2ComboBox.SelectedIndex + 1;

            try
            {
                var typeresult = PokemonChangedSetTypes(box, bx, dexID);
                PokemonChangedSetStats(box, bx, dexID, typeresult.Item1);
                if (box.Name.Contains("Pokemon")) { PokemonChangedSetDefaults(bx, dexID, typeresult.Item2); }

            } catch (SQLiteException ex)
            {
                string msg = "Encountered a problem working with species.db: " + ex.Message +
                    "\nThe Pokemon was not loaded, but you can try again by reselecting it from the list.";
                MessageBox.Show(msg, "DB Error", MessageBoxButtons.OK);
                db.Close(); // If an exception is thrown, the datareader that threw it vanishes leaving the db open. 
                return;
            }
        }

        // This method reads the SPECIES or SPECIES_OVERRIDE table, sets the types, and also grabs an overrideID or default form for later use.
        private Tuple<string, string> PokemonChangedSetTypes(ComboBox box, string bx, int dexID)
        {
            Dictionary<string, ComboBox> cbs = new()
            {
                { "cbt11", mfType11ComboBox },
                { "cbt21", mfType21ComboBox },
                { "cbt12", mfType12ComboBox },
                { "cbt22", mfType22ComboBox },
            };

            string cmd = box.Name.Contains("Form") && box.SelectedIndex != 0 ? // If a Form combobox sent this, make sure the user isn't switching back to the default form.
                    "SELECT * from SPECIES_OVERRIDE where FORM_NAME = \"" + (bx == "1" ? mfForm1ComboBox : mfForm2ComboBox).Text + "\"" :
                    "SELECT * from SPECIES where DEX_ID = " + dexID.ToString();
            SQLiteDataReader dr = GenerateDataReader(cmd);

            dr.Read();
            string overrideId = dr.GetInt32(0).ToString(); // This will fill overrideID with a usable value if species override was queried, otherwise it's filled with junk that doesn't get used. 
            string defaultForm = dr.GetString(2);
            // set type comboboxes
            cbs["cbt1" + bx].SelectedIndex = cbs["cbt1" + bx].Items.IndexOf(dr.GetString(3));
            cbs["cbt2" + bx].SelectedIndex = cbs["cbt2" + bx].Items.IndexOf(dr.GetString(4));
            dr.Close();

            return new(overrideId, defaultForm);
        }

        private void PokemonChangedSetStats(ComboBox box, string bx, int dexID, string overrideId)
        {
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

            SQLiteDataReader? dr = null;
            string cmd;
            bool changeStats = true;
            if (box.Name.Contains("Form") && box.SelectedIndex != 0)
            {
                cmd = "SELECT HP, ATTACK, DEFENSE, SP_ATTACK, SP_DEFENSE, SPEED from STATS_OVERRIDE where OVERRIDE_ID = " + overrideId;
                try { dr = GenerateDataReader(cmd); }
                catch { changeStats = false; } // Exception thrown means query had no row means there are no stats to change. 
            }
            else
            {
                cmd = "SELECT HP, ATTACK, DEFENSE, SP_ATTACK, SP_DEFENSE, SPEED from BASE_STATS where DEX_ID = " + dexID.ToString();
                dr = GenerateDataReader(cmd);
            }

            if (changeStats && dr != null)
            {
                // set base stats
                dr.Read();
                pbs["pbhp" + bx].Value = dr.GetInt32(0);
                pbs["pbat" + bx].Value = dr.GetInt32(1);
                pbs["pbde" + bx].Value = dr.GetInt32(2);
                pbs["pbsa" + bx].Value = dr.GetInt32(3);
                pbs["pbsd" + bx].Value = dr.GetInt32(4);
                pbs["pbsp" + bx].Value = dr.GetInt32(5);

                lbs["lbhp" + bx].Text = "Hp: " + dr.GetInt32(0).ToString();
                lbs["lbat" + bx].Text = "Atk: " + dr.GetInt32(1).ToString();
                lbs["lbde" + bx].Text = "Def: " + dr.GetInt32(2).ToString();
                lbs["lbsa" + bx].Text = "SpAtk: " + dr.GetInt32(3).ToString();
                lbs["lbsd" + bx].Text = "SpDef: " + dr.GetInt32(4).ToString();
                lbs["lbsp" + bx].Text = "Spd: " + dr.GetInt32(5).ToString();

                pbs["pbhp" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(0));
                pbs["pbat" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(1));
                pbs["pbde" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(2));
                pbs["pbsa" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(3));
                pbs["pbsd" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(4));
                pbs["pbsp" + bx].ForeColor = ChooseProgressBarColor(dr.GetInt32(5));
                dr.Close();
            }
        }

        // This method fires when a Pokemon combobox is changed, loading the forms and default evs/moves. 
        private void PokemonChangedSetDefaults(string bx, int dexID, string defaultForm)
        {
            Dictionary<string, NumericUpDown> nms = new()
            {
                {"nmhp1", mfHP1EVNumUpDown },
                {"nmat1", mfAtk1EVNumUpDown },
                {"nmde1", mfDef1EVNumUpDown },
                {"nmsa1", mfSpAtk1EVNumUpDown },
                {"nmsd1", mfSpDef1EVNumUpDown },
                {"nmsp1", mfSpd1EVNumUpDown },
                {"nmhp2", mfHP2EVNumUpDown },
                {"nmat2", mfAtk2EVNumUpDown },
                {"nmde2", mfDef2EVNumUpDown },
                {"nmsa2", mfSpAtk2EVNumUpDown },
                {"nmsd2", mfSpDef2EVNumUpDown },
                {"nmsp2", mfSpd2EVNumUpDown }
            };
            Dictionary<string, Button> bts = new()
            {
                {"btm11", mfMove11Button },
                {"btm21", mfMove21Button },
                {"btm31", mfMove31Button },
                {"btm41", mfMove41Button },
                {"btm12", mfMove12Button },
                {"btm22", mfMove22Button },
                {"btm32", mfMove32Button },
                {"btm42", mfMove42Button },
            };

            (bx == "1" ? mfForm1ComboBox : mfForm2ComboBox).Items.Clear();
            (bx == "1" ? mfForm1ComboBox : mfForm2ComboBox).Items.Add(defaultForm);

            string cmd = "SELECT FORM_NAME from SPECIES_OVERRIDE where DEX_ID = " + dexID.ToString();
            try
            {
                SQLiteDataReader dr = GenerateDataReader(cmd);
                while (dr.Read()) { (bx == "1" ? mfForm1ComboBox : mfForm2ComboBox).Items.Add(dr.GetString(0)); }
                dr.Close();
            } catch (SQLiteException) { } // If query results in no rows, that means this pokemon has no other forms. 

            noEvent = true; // Block triggering this event programmatically. 
            (bx == "1" ? mfForm1ComboBox : mfForm2ComboBox).SelectedIndex = 0;
            noEvent = false;

            nms["nmhp" + bx].Value = nms["nmat" + bx].Value = nms["nmde" + bx].Value =
                    nms["nmsa" + bx].Value = nms["nmsd" + bx].Value = nms["nmsp" + bx].Value = 85;
            bts["btm1" + bx].Text = bts["btm2" + bx].Text = bts["btm3" + bx].Text = bts["btm4" + bx].Text = "[Select Move]";

            if (bx == "1") { playerMoves = new int[4] { 0, 0, 0, 0 }; }
            else { rivalMoves = new int[4] { 0, 0, 0, 0 }; }
        }
        #endregion
        #region TypeComboBoxChange
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
            type ??= "For some reason the compiler thinks this can be null so I set it as nullable to shut it up.";
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

        // These two events are here to correct some behavior that cuases the comboboxes to be highlighted when touched.
        private void TypeComboBox_MouseDown(object sender, MouseEventArgs e)
        {
            ComboBox cb = (ComboBox)sender;
            if (!noTypeEdit1 && cb.Name.Contains('1')) { return; }
            if (!noTypeEdit2 && cb.Name.Contains('2')) { return; }
            this.ActiveControl = null;
        }

        private void TypeComboBox_DropDownClosed(object sender, EventArgs e)
        { this.ActiveControl = null; }
        #endregion
        #region MiscEvents
        private void ProgressBars_MouseClick(object sender, EventArgs e)
        {
            ColorProgressBar pb = (ColorProgressBar)sender;
            // If custom template isn't checked, you can't change any values. Simple as. 
            if (pb.Name.Contains('1')) { if (!mfCustomForm1CheckBox.Checked) { return; } }
            else if (!mfCustomForm2CheckBox.Checked) { return; }

            string whichStat = "Set ";
            whichStat += pb.Name.Contains('1') ? "player " : "rival ";
            whichStat += pb.Name switch
            {
                _ when pb.Name.Contains("mfHP") => "HP",
                _ when pb.Name.Contains("mfAtk") => "Atk",
                _ when pb.Name.Contains("mfDef") => "Def",
                _ when pb.Name.Contains("mfSpAtk") => "SpAtk",
                _ when pb.Name.Contains("mfSpDef") => "SpDef",
                _ when pb.Name.Contains("mfSpd") => "Spd",
                _ => "idk lol"
            };
            whichStat += " to:";

            StatChangeForm scf = new(whichStat);
            var result = scf.ShowDialog();
            if (result == DialogResult.Cancel) { return; }
            int ns = scf.NewStat;

            Tuple<ColorProgressBar, Label> toBeChanged = whichStat switch
            {// Since the label identifying what's being set is customized, I can use its text to determine what should be changed. 
                "Set player HP to:" => new Tuple<ColorProgressBar, Label>(mfHP1ProgressBar, mfHP1Label),
                "Set player Atk to:" => new Tuple<ColorProgressBar, Label>(mfAtk1ProgressBar, mfAtk1Label),
                "Set player Def to:" => new Tuple<ColorProgressBar, Label>(mfDef1ProgressBar, mfDef1Label),
                "Set player SpAtk to:" => new Tuple<ColorProgressBar, Label>(mfSpAtk1ProgressBar, mfSpAtk1Label),
                "Set player SpDef to:" => new Tuple<ColorProgressBar, Label>(mfSpDef1ProgressBar, mfSpDef1Label),
                "Set player Spd to:" => new Tuple<ColorProgressBar, Label>(mfSpd1ProgressBar, mfSpd1Label),
                "Set rival HP to:" => new Tuple<ColorProgressBar, Label>(mfHP2ProgressBar, mfHP2Label),
                "Set rival Atk to:" => new Tuple<ColorProgressBar, Label>(mfAtk2ProgressBar, mfAtk2Label),
                "Set rival Def to:" => new Tuple<ColorProgressBar, Label>(mfDef2ProgressBar, mfDef2Label),
                "Set rival SpAtk to:" => new Tuple<ColorProgressBar, Label>(mfSpAtk2ProgressBar, mfSpAtk2Label),
                "Set rival SpDef to:" => new Tuple<ColorProgressBar, Label>(mfSpDef2ProgressBar, mfSpDef2Label),
                _ => new Tuple<ColorProgressBar, Label>(mfSpd2ProgressBar, mfSpd2Label) // set rival speed, need a default case.
            };

            toBeChanged.Item1.Value = ns;
            toBeChanged.Item1.ForeColor = ChooseProgressBarColor(ns);
            toBeChanged.Item2.Text = ns switch
            {
                _ when whichStat.Contains("HP") => "HP: ",
                _ when whichStat.Contains("Atk") => "Atk: ",
                _ when whichStat.Contains("Def") => "Def: ",
                _ when whichStat.Contains("SpAtk") => "SpAtk: ",
                _ when whichStat.Contains("SpDef") => "SpDef: ",
                _ when whichStat.Contains("Spd") => "Spd: ",
                _ => "NULL"
            };
            toBeChanged.Item2.Text += ns.ToString();
        }
        private void EVNumUpDown_ValueChanged(object sender, EventArgs e)
        {
            NumericUpDown ns = (NumericUpDown)sender;
            List<NumericUpDown> num = new();
            if (ns.Name.Contains('1'))
            {
                num.Add(mfHP1EVNumUpDown);
                num.Add(mfAtk1EVNumUpDown);
                num.Add(mfDef1EVNumUpDown);
                num.Add(mfSpAtk1EVNumUpDown);
                num.Add(mfSpDef1EVNumUpDown);
                num.Add(mfSpd1EVNumUpDown);
            } else
            {
                num.Add(mfHP2EVNumUpDown);
                num.Add(mfAtk2EVNumUpDown);
                num.Add(mfDef2EVNumUpDown);
                num.Add(mfSpAtk2EVNumUpDown);
                num.Add(mfSpDef2EVNumUpDown);
                num.Add(mfSpd2EVNumUpDown);
            }

            int idx = num.FindIndex(x => x == ns);
            int total = 0;
            foreach (NumericUpDown n in num) { total += (int)n.Value; }
            if (total > 510)
            {
                SystemSounds.Hand.Play();
                ns.Value = ns.Name.Contains('1') ? playerEVs[idx] : rivalEVs[idx];
            } else if (ns.Name.Contains('1')) { playerEVs[idx] = (int)ns.Value; }
            else { rivalEVs[idx] = (int)ns.Value; }
        }

        private void CustomTemplateCheckBox_CheckChanged(object sender, EventArgs e)
        {
            CheckBox cb = (CheckBox)sender;
            if (cb.Checked)
            { // Don't want the event to trigger for this, causes beeg problems with a -1 index.
                noEvent = true;
                (cb.Name.Contains('1') ? mfForm1ComboBox : mfForm2ComboBox).SelectedIndex = -1;
                (cb.Name.Contains('1') ? mfForm1ComboBox : mfForm2ComboBox).Enabled = false;
                noEvent = false;
                if (cb.Name.Contains('1')) { noTypeEdit1 = false; } else { noTypeEdit2 = false; } // The above trick doesn't work here for some reason.
                (cb.Name.Contains('1') ? mfType11ComboBox : mfType12ComboBox).DropDownStyle =
                    (cb.Name.Contains('1') ? mfType21ComboBox : mfType22ComboBox).DropDownStyle = ComboBoxStyle.DropDownList;
            } else
            { // I do want the event to trigger for this to clear out any custom template stuff that was discarded. 
                (cb.Name.Contains('1') ? mfForm1ComboBox : mfForm2ComboBox).SelectedIndex = 0;
                (cb.Name.Contains('1') ? mfForm1ComboBox : mfForm2ComboBox).Enabled = true;
                if (cb.Name.Contains('1')) { noTypeEdit1 = true; } else { noTypeEdit2 = true; } // The above trick doesn't work here for some reason.
                (cb.Name.Contains('1') ? mfType11ComboBox : mfType12ComboBox).DropDownStyle =
                    (cb.Name.Contains('1') ? mfType21ComboBox : mfType22ComboBox).DropDownStyle = ComboBoxStyle.Simple;
            }
        }

        private void MoveButton_Click(object sender, EventArgs e)
        {
            // This method launches another window that helps the user decide what move they want. 
            MoveForm vf = new();
            var result = vf.ShowDialog();
            if (result == DialogResult.Cancel) { return; } // The user x'd out instead of accepting the move.

            Dictionary<string, Button> btn = new()
            {
                { "mfMove11Button", mfMove11Button},
                { "mfMove21Button", mfMove21Button},
                { "mfMove31Button", mfMove31Button},
                { "mfMove41Button", mfMove41Button},
                { "mfMove12Button", mfMove12Button},
                { "mfMove22Button", mfMove22Button},
                { "mfMove32Button", mfMove32Button},
                { "mfMove42Button", mfMove42Button}
            };
            Button b = (Button)sender;

            btn[b.Name].Text = vf.MoveName;
            if (b.Name[7] == '1') { playerMoves[b.Name[6] - '1'] = vf.MoveID; }
            else { rivalMoves[b.Name[6] - '1'] = vf.MoveID; }
        }

        private void EVSetDefaultButton_Click(object sender, EventArgs e)
        {
            Button but = (Button)sender;
            Dictionary<string, NumericUpDown> nms = new()
            {
                {"nmhp1", mfHP1EVNumUpDown },
                {"nmat1", mfAtk1EVNumUpDown },
                {"nmde1", mfDef1EVNumUpDown },
                {"nmsa1", mfSpAtk1EVNumUpDown },
                {"nmsd1", mfSpDef1EVNumUpDown },
                {"nmsp1", mfSpd1EVNumUpDown },
                {"nmhp2", mfHP2EVNumUpDown },
                {"nmat2", mfAtk2EVNumUpDown },
                {"nmde2", mfDef2EVNumUpDown },
                {"nmsa2", mfSpAtk2EVNumUpDown },
                {"nmsd2", mfSpDef2EVNumUpDown },
                {"nmsp2", mfSpd2EVNumUpDown }
            };
            nms["nmhp" + but.Name[14]].Value = nms["nmat" + but.Name[14]].Value = nms["nmde" + but.Name[14]].Value =
                nms["nmsa" + but.Name[14]].Value = nms["nmsd" + but.Name[14]].Value = nms["nmsp" + but.Name[14]].Value = 85;
        }
        #endregion
        #region SaveExit
        private void SaveFilesOnlyButton_Click(object sender, EventArgs e)
        {
            if (SerializePokemon())
            { MessageBox.Show("Successfully saved.", "Save success", MessageBoxButtons.OK, MessageBoxIcon.Information); }
        }

        private void RunBattleButton_Click(object sender, EventArgs e)
        {
            // put code for saving the prepared mons and getting ready for PokeBattleTest
            if (!SerializePokemon()) { return; }
            Process pbt = new();
            pbt.StartInfo.FileName = Application.StartupPath + "PokeBattleTest.exe";
            pbt.Start();
            this.Hide();
            pbt.WaitForExit();
            this.Show();
        }

        private bool SerializePokemon()
        {
            if (!mfPokemon1ComboBox.Items.Contains(mfPokemon1ComboBox.Text) || 
                !mfPokemon2ComboBox.Items.Contains(mfPokemon2ComboBox.Text))
            {
                MessageBox.Show("You need a valid Pokemon selection in both boxes before you can do this.", 
                    "Serialize", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            if (!playerMoves.Any(x => x != 0) || !rivalMoves.Any(x => x != 0))
            {
                MessageBox.Show("Both Pokemon require at least one valid move.", "Serialize", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            string pForm = mfForm1ComboBox.SelectedIndex switch
            {
                -1 => "custom",
                0 => "default",
                _ => mfForm1ComboBox.Text
            };
            string oForm = mfForm2ComboBox.SelectedIndex switch
            {
                -1 => "custom",
                0 => "default",
                _ => mfForm2ComboBox.Text
            };

            int pNature = SerializeGetNatureID(mfNature1ComboBox.Text.Split(' ')[0]);
            int rNature = SerializeGetNatureID(mfNature2ComboBox.Text.Split(' ')[0]);

            try
            {
                Mon pMon = new("Player", 
                    mfPokemon1ComboBox.SelectedIndex + 1, 
                    (int)mfLevel1NumUpDown.Value, 
                    pForm, pNature, 
                    playerMoves[0], playerMoves[1], playerMoves[2], playerMoves[3],
                    (int)mfHP1EVNumUpDown.Value, 
                    (int)mfAtk1EVNumUpDown.Value, 
                    (int)mfDef1EVNumUpDown.Value,
                    (int)mfSpAtk1EVNumUpDown.Value, 
                    (int)mfSpDef1EVNumUpDown.Value, 
                    (int)mfSpd1EVNumUpDown.Value);

                Mon rMon = new("Rival", 
                    mfPokemon2ComboBox.SelectedIndex + 1, 
                    (int)mfLevel2NumUpDown.Value, 
                    oForm, rNature, 
                    rivalMoves[0], rivalMoves[1], rivalMoves[2], rivalMoves[3],
                    (int)mfHP2EVNumUpDown.Value, 
                    (int)mfAtk2EVNumUpDown.Value, 
                    (int)mfDef2EVNumUpDown.Value,
                    (int)mfSpAtk2EVNumUpDown.Value, 
                    (int)mfSpDef2EVNumUpDown.Value, 
                    (int)mfSpd2EVNumUpDown.Value);

                CustomTemplate pTem, rTem;
                if (mfCustomForm1CheckBox.Checked)
                {
                    pTem = new("Player", mfType11ComboBox.Text, mfType21ComboBox.Text,
                        mfHP1ProgressBar.Value, mfAtk1ProgressBar.Value, mfDef1ProgressBar.Value,
                        mfSpAtk1ProgressBar.Value, mfSpDef1ProgressBar.Value, mfSpd1ProgressBar.Value);
                } else { pTem = new ("Player", "", "", 0, 0, 0, 0, 0, 0); }
                if (mfCustomForm2CheckBox.Checked)
                {
                    rTem = new("Rival", mfType12ComboBox.Text, mfType22ComboBox.Text,
                        mfHP2ProgressBar.Value, mfAtk2ProgressBar.Value, mfDef2ProgressBar.Value,
                        mfSpAtk2ProgressBar.Value, mfSpDef2ProgressBar.Value, mfSpd2ProgressBar.Value);
                }
                else { rTem = new("Rival", "", "", 0, 0, 0, 0, 0, 0); }

                SerializeWriteToFile(pMon, "pMon");
                SerializeWriteToFile(rMon, "rMon");
                SerializeWriteToFile(pTem, "pTem");
                SerializeWriteToFile(rTem, "rTem");
            }
            catch (Exception e)
            {
                string msg = "Encountered an unexpected problem trying to save your Pokemon: " + e.Message;
                MessageBox.Show(msg, "Save error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return true;
        }

        private int SerializeGetNatureID(string nature) 
        {
            try
            {
                string cmd = "SELECT NATURE_ID FROM NATURE WHERE NAME = \"" + nature + "\"";
                SQLiteDataReader dr = GenerateDataReader(cmd);
                dr.Read();
                int ret = dr.GetInt32(0);
                dr.Close();
                return ret;

            } catch (SQLiteException)
            {
                string error = "Could get Nature ID from species.db. If nothing else goes wrong " +
                    "with the save process, the nature will be set to Hardy (ID 0).";
                MessageBox.Show(error, "Serialize", MessageBoxButtons.OK, MessageBoxIcon.Error);
                db.Close();
                return 0;
            }
        }

        private static void SerializeWriteToFile(object o, string which)
        {
            var monJson = JsonConvert.SerializeObject(o);
            string format = JValue.Parse(monJson).ToString(Newtonsoft.Json.Formatting.Indented);
            string path = Application.StartupPath + "resource/" + which switch
            {
                "pMon" => "p-pkmn",
                "rMon" => "r-pkmn",
                "pTem" => "p-template",
                "rTem" => "r-template",
                _ => "NULL"
            } + ".json";
            if (File.Exists(path)) { File.Delete(path); }
            File.WriteAllText(path, format);
        }
        #endregion
        #region SQLite
        private SQLiteDataReader GenerateDataReader(string cmdtxt)
        {
            db.Open();
            using var command = new SQLiteCommand(db);
            command.CommandText = cmdtxt;
            SQLiteDataReader dr = command.ExecuteReader(CommandBehavior.CloseConnection);
            if (!dr.HasRows) { throw new SQLiteException("The query gave a result without any rows."); }
            return dr;
        }
        #endregion
    }
}