using System.Diagnostics;
using System.Text.Json;

namespace MonDesigner
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
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

        private void SerializePokemon()
        {

        }
    }
}