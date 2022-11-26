using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Data;
using System.Data.SQLite;
using System.Diagnostics;
using System.Media;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        private record Mon(string Who, int DexID, int Level, string Form, int Move1, int Move2, int Move3, int Move4,
            int HPEV, int AtkEV, int DefEV, int SpAtkEV, int SpDefEV, int SpdEV);
        private record CustomTemplate(string Who, string Type1, string Type2, int HP, int ATK, int DEF, int SPATK, int SPDEF, int SPD);
        private readonly List<string> names;
        private bool noEvent, noTypeEdit1, noTypeEdit2;
        private int[] playerMoves;
        private int[] opponentMoves;
        private int[] playerEVs;
        private int[] opponentEVs; // These are necessary to maintain the EV cap programmatically. 
        public MainForm()
        {
            InitializeComponent();
            FixBrokenProgressBars();
            names = new();
            LoadPokemonComboBoxes();
            noEvent = false;
            noTypeEdit1 = noTypeEdit2 = true;
            playerMoves = new int[4] { 0, 0, 0, 0};
            opponentMoves = new int[4] { 0, 0, 0, 0 };
            playerEVs = new int[6];
            opponentEVs = new int[6];
            mfDefaultEVRadio.Checked = true;
        }
        private void LoadPokemonComboBoxes()
        {
            try
            {
                string f = @"URI=file:" + Application.StartupPath + "resource/species.db";
                using var speciesdb = new SQLiteConnection(f);
                speciesdb.Open();
                using var command = new SQLiteCommand(speciesdb);
                command.CommandText = "SELECT NAME from SPECIES where DEX_ID <= 1010"; // All the way up to gen 9 baybee.
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
            int dexID = box.Name.Contains('1') ? mfPokemon1ComboBox.SelectedIndex + 1 : mfPokemon2ComboBox.SelectedIndex + 1;

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
                if (box.Name.Contains("Form") && box.SelectedIndex != 0)
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

        private void ProgressBars_MouseClick(object sender, EventArgs e)
        {
            ColorProgressBar pb = (ColorProgressBar)sender;
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
        // end segment

        private void EVRadio_CheckChanged(object sender, EventArgs e)
        {
            // Only need to bind this event to one of the two radiobuttons since it fires for both checked and unchecked and there's only two.
            RadioButton rb = (RadioButton)sender;

            if (rb.Checked) {
                mfEVTableLayout.SendToBack();
                mfJokePictureBox.Visible = true;
                mfJokeLabel.Text = "\"You seriously made a completely separate application in a different " +
                    "language just to make template designing a little easier?\"\r\nMe:\r\n";
            }
            else { 
                mfEVTableLayout.BringToFront();
                mfJokePictureBox.Visible = false;
                mfJokeLabel.Text = "Wasteful white space.";
            }
            mfEVTableLayout.Visible = !rb.Checked;

            // Set all to default values regardless of checked status. 
            mfHP1EVNumUpDown.Value = mfAtk1EVNumUpDown.Value = mfDef1EVNumUpDown.Value =
                mfSpAtk1EVNumUpDown.Value = mfSpDef1EVNumUpDown.Value = mfSpd1EVNumUpDown.Value =
                mfHP2EVNumUpDown.Value = mfAtk2EVNumUpDown.Value = mfDef2EVNumUpDown.Value =
                mfSpAtk2EVNumUpDown.Value = mfSpDef2EVNumUpDown.Value = mfSpd2EVNumUpDown.Value = 85;
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
                ns.Value = ns.Name.Contains('1') ? playerEVs[idx] : opponentEVs[idx];
            } else if (ns.Name.Contains('1')) { playerEVs[idx] = (int)ns.Value; }
            else { opponentEVs[idx] = (int)ns.Value; }
        }

        private void CustomTemplateCheckBox_CheckChanged(object sender, EventArgs e)
        {
            CheckBox cb = (CheckBox)sender;
            mfCustomStatLabel.Visible = mfCustomStatNumUpDown.Visible = mfChangeCustomStatButton.Visible = false;
            mfCustomStatNumUpDown.Value = 1;
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

        private void ChangeCustomStatButton_Click(object sender, EventArgs e)
        {
            Dictionary<string, Tuple<ColorProgressBar, Label>> stats = new()
            { // Since the label identifying what's being set is customized, I can use its text to determine what should be changed. 
                {"Set player HP to:", new Tuple<ColorProgressBar, Label>(mfHP1ProgressBar, mfHP1Label)},
                {"Set player Atk to:", new Tuple<ColorProgressBar, Label>(mfAtk1ProgressBar, mfAtk1Label)},
                {"Set player Def to:", new Tuple<ColorProgressBar, Label>(mfDef1ProgressBar, mfDef1Label)},
                {"Set player SpAtk to:", new Tuple<ColorProgressBar, Label>(mfSpAtk1ProgressBar, mfSpAtk1Label)},
                {"Set player SpDef to:", new Tuple<ColorProgressBar, Label>(mfSpDef1ProgressBar, mfSpDef1Label)},
                {"Set player Spd to:", new Tuple<ColorProgressBar, Label>(mfSpd1ProgressBar, mfSpd1Label)},
                {"Set opponent HP to:", new Tuple<ColorProgressBar, Label>(mfHP2ProgressBar, mfHP2Label)},
                {"Set opponent Atk to:", new Tuple<ColorProgressBar, Label>(mfAtk2ProgressBar, mfAtk2Label)},
                {"Set opponent Def to:", new Tuple<ColorProgressBar, Label>(mfDef2ProgressBar, mfDef2Label)},
                {"Set opponent SpAtk to:", new Tuple<ColorProgressBar, Label>(mfSpAtk2ProgressBar, mfSpAtk2Label)},
                {"Set opponent SpDef to:", new Tuple<ColorProgressBar, Label>(mfSpDef2ProgressBar, mfSpDef2Label)},
                {"Set opponent Spd to:", new Tuple<ColorProgressBar, Label>(mfSpd2ProgressBar, mfSpd2Label)},
            };

            stats[mfCustomStatLabel.Text].Item1.Value = (int)mfCustomStatNumUpDown.Value;
            stats[mfCustomStatLabel.Text].Item1.ForeColor = ChooseProgressBarColor((int)mfCustomStatNumUpDown.Value);
            stats[mfCustomStatLabel.Text].Item2.Text = stats switch
            {
                _ when mfCustomStatLabel.Text.Contains("HP") => "HP: ",
                _ when mfCustomStatLabel.Text.Contains("Atk") => "Atk: ",
                _ when mfCustomStatLabel.Text.Contains("Def") => "Def: ",
                _ when mfCustomStatLabel.Text.Contains("SpAtk") => "SpAtk: ",
                _ when mfCustomStatLabel.Text.Contains("SpDef") => "SpDef: ",
                _ when mfCustomStatLabel.Text.Contains("Spd") => "Spd: ",
                _ => "NULL"
            };
            stats[mfCustomStatLabel.Text].Item2.Text += mfCustomStatNumUpDown.Value.ToString();

            mfCustomStatLabel.Visible = mfCustomStatNumUpDown.Visible = mfChangeCustomStatButton.Visible = false;
            mfCustomStatNumUpDown.Value = 1;
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
            else { opponentMoves[b.Name[6] - '1'] = vf.MoveID; }
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

            if (!playerMoves.Any(x => x != 0) || !opponentMoves.Any(x => x != 0))
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

            try
            {
                List<Mon> mons = new()
                { // Junky and dense, but this is just pulling from all the relevant sections. 
                    new Mon("Player", mfPokemon1ComboBox.SelectedIndex + 1, (int)mfLevel1NumUpDown.Value, pForm, 
                    playerMoves[0], playerMoves[1], playerMoves[2], playerMoves[3],
                    (int)mfHP1EVNumUpDown.Value, (int)mfAtk1EVNumUpDown.Value, (int)mfDef1EVNumUpDown.Value,
                    (int)mfSpAtk1EVNumUpDown.Value, (int)mfSpDef1EVNumUpDown.Value, (int)mfSpd1EVNumUpDown.Value),

                    new Mon("Opponent", mfPokemon2ComboBox.SelectedIndex + 1, (int)mfLevel2NumUpDown.Value, oForm, 
                    opponentMoves[0], opponentMoves[1], opponentMoves[2], opponentMoves[3],
                    (int)mfHP2EVNumUpDown.Value, (int)mfAtk2EVNumUpDown.Value, (int)mfDef2EVNumUpDown.Value,
                    (int)mfSpAtk2EVNumUpDown.Value, (int)mfSpDef2EVNumUpDown.Value, (int)mfSpd2EVNumUpDown.Value)
                };
                var monJson = JsonConvert.SerializeObject(mons);
                string monFrmt = JValue.Parse(monJson).ToString(Newtonsoft.Json.Formatting.Indented);
                string path = Application.StartupPath + "resource/pkmn.json";
                if (File.Exists(path)) { File.Delete(path); }
                File.WriteAllText(path, monFrmt);

                List<CustomTemplate> ctem = new();
                if (mfCustomForm1CheckBox.Checked)
                {
                    ctem.Add(new CustomTemplate("Player", mfType11ComboBox.Text, mfType21ComboBox.Text, 
                        mfHP1ProgressBar.Value, mfAtk1ProgressBar.Value, mfDef1ProgressBar.Value, 
                        mfSpAtk1ProgressBar.Value, mfSpDef1ProgressBar.Value, mfSpd1ProgressBar.Value));
                } else { ctem.Add(new CustomTemplate("Player", "", "", 0, 0, 0, 0, 0, 0)); }
                if (mfCustomForm2CheckBox.Checked)
                {
                    ctem.Add(new CustomTemplate("Player", mfType12ComboBox.Text, mfType22ComboBox.Text,
                        mfHP2ProgressBar.Value, mfAtk2ProgressBar.Value, mfDef2ProgressBar.Value,
                        mfSpAtk2ProgressBar.Value, mfSpDef2ProgressBar.Value, mfSpd2ProgressBar.Value));
                }
                else { ctem.Add(new CustomTemplate("Opponent", "", "", 0, 0, 0, 0, 0, 0)); }

                var temJson = JsonConvert.SerializeObject(ctem);
                string temFrmt = JValue.Parse(temJson).ToString(Newtonsoft.Json.Formatting.Indented);
                path = Application.StartupPath + "resource/pkmntemplate.json";
                if (File.Exists(path)) { File.Delete(path); }
                File.WriteAllText(path, temFrmt);
            }
            catch (Exception e)
            {
                string msg = "Encountered an unexpected problem trying to save your Pokemon: " + e.Message;
                MessageBox.Show(msg, "Save error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return true;
        }
    }
}