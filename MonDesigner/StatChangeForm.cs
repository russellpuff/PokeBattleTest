using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MonDesigner
{
    public partial class StatChangeForm : Form
    {
        public int NewStat;
        private bool changeOK;
        public StatChangeForm(string _stat)
        {
            InitializeComponent();
            scfChangeStatLabel.Text = _stat;
            NewStat = 0;
            changeOK = false;
        }

        private void ChangeStatButton_Click(object sender, EventArgs e)
        {
            NewStat = (int)scfChangeStatNumUpDown.Value;
            changeOK = true;
            this.Close();
        }

        private void StatChangeForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.DialogResult = changeOK ? DialogResult.OK : DialogResult.Cancel;
        }
    }
}
