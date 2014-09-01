using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication11
{
    public partial class UserControl1 : UserControl
    {

        public UserControl1()
        {
            InitializeComponent();

            Timer timer1 = new Timer();
            timer1.Interval = 200;
            timer1.Tick += new EventHandler(timer1_Tick);
            timer1.Start();

            this.DoubleClick += new EventHandler(UserControl1_DoubleClick);
        }

        void timer1_Tick(object sender, EventArgs e)
        {
            this.Invalidate();
        }

        void UserControl1_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                if(this.Parent.ClientRectangle.Height == this.Height)
                {
                    this.Height = this.Parent.ClientRectangle.Height - 152;
                }
                else
                {
                    this.Height = this.Parent.ClientRectangle.Height;
                }
            }
            catch
            { 
            
            }
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            this.Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            try
            {
                int singleNum = 17;
                int xGain = trackBar1.Value * 200;
                PointF[,] pointF = new PointF[singleNum, xGain];  
                
                Pen pen1 = new Pen(Color.Gray, 1f);
                pen1.DashStyle = DashStyle.Custom;
                pen1.DashPattern = new float[] {1f, 4f};

                StringFormat stringFormat = new StringFormat();
                stringFormat.Alignment = StringAlignment.Center;

                int pStart = 0;
                int pEnd = SerialPortStruct.valueCount;
                if(SerialPortStruct.valueCount > xGain)
                {
                    if(button2.BackColor != Color.Lime)
                    {
                        pStart = SerialPortStruct.valueCount - xGain;
                        pEnd = SerialPortStruct.valueCount;
                    }
                    else
                    {
                        pStart = hScrollBar1.Value;
                        pEnd = hScrollBar1.Value + xGain;
                    }
                }

                label1.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 0]) : (0)).ToString();
                label2.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 1]) : (0)).ToString();
                label3.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 2]) : (0)).ToString();
                label4.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 3]) : (0)).ToString();
                label5.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 4]) : (0)).ToString();
                label6.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 5]) : (0)).ToString();
                label7.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 6]) : (0)).ToString();
                label8.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 7]) : (0)).ToString();
                label9.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 8]) : (0)).ToString();
                label10.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 9]) : (0)).ToString();
                label11.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 10]) : (0)).ToString();
                label12.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 11]) : (0)).ToString();
                label13.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 12]) : (0)).ToString();
                label14.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 13]) : (0)).ToString();
                label15.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 14]) : (0)).ToString();
                label16.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 15]) : (0)).ToString();
                label17.Text = ((SerialPortStruct.valueCount > 0) ? (SerialPortStruct.valueBuffer[SerialPortStruct.valueCount - 1, 16]) : (0)).ToString();
                
                int max = 0;
                int min = 0;
                int[] cpbuf = new int[32];
                //
                if (checkBox1.Checked)
                {
                    cpbuf[0]++;
                }
                if (checkBox2.Checked)
                {
                    cpbuf[1]++;
                }
                if (checkBox3.Checked)
                {
                    cpbuf[2]++;
                }
                if (checkBox4.Checked)
                {
                    cpbuf[3]++;
                }
                if (checkBox5.Checked)
                {
                    cpbuf[4]++;
                }
                if (checkBox6.Checked)
                {
                    cpbuf[5]++;
                }
                if (checkBox7.Checked)
                {
                    cpbuf[6]++;
                }
                if (checkBox8.Checked)
                {
                    cpbuf[7]++;
                }
                if (checkBox9.Checked)
                {
                    cpbuf[8]++;
                }
                if (checkBox10.Checked)
                {
                    cpbuf[9]++;
                }
                if (checkBox11.Checked)
                {
                    cpbuf[10]++;
                }
                if (checkBox12.Checked)
                {
                    cpbuf[11]++;
                }
                if (checkBox13.Checked)
                {
                    cpbuf[12]++;
                }
                if (checkBox14.Checked)
                {
                    cpbuf[13]++;
                }
                if (checkBox15.Checked)
                {
                    cpbuf[14]++;
                }
                if (checkBox16.Checked)
                {
                    cpbuf[15]++;
                }
                if (checkBox17.Checked)
                {
                    cpbuf[16]++;
                }
                for (int i = 0; i < 17; i++)
                {
                    if(cpbuf[i] != 0)
                    {
                        if(max < SerialPortStruct.valueMax[i])
                        {
                            max = SerialPortStruct.valueMax[i];
                        }
                        if(min > SerialPortStruct.valueMin[i])
                        {
                            min = SerialPortStruct.valueMin[i];
                        }
                    }
                }
                max = (max > (0 - min)) ? (max) : (0 - min);
                if (max < 100)
                {
                    max = 100;
                }
                else if (max < 1000)
                {
                    max = max - max % 100 + 100;
                }
                else if (max < 10000)
                {
                    max = max - max % 1000 + 1000;
                }
                else
                {
                    max = max - max % 10000 + 10000;
                }

                for (int i = 0; i < xGain; i++)
                {
                    if (pEnd > i)
                    {
                        for (int j = 0; j < singleNum; j++)
                        {
                            pointF[j, i] = new PointF(Convert.ToSingle(this.Width - 40 - 135) * i / xGain + 40 + 1, ((this.Height - 10 - 30 - 12) / 2 + 10) - SerialPortStruct.valueBuffer[pStart + i, j] * ((this.Height - 10 - 30 - 12) / 2) / max);
                        }
                    }
                    else if(i == 0)
                    {
                        for (int j = 0; j < singleNum; j++)
                        {
                            pointF[j, i] = new PointF(40, (this.Height - 10 - 30 - 17) / 2 + 10); 
                        }
                    }
                    else
                    {
                        for (int j = 0; j < singleNum; j++)
                        {
                            pointF[j, i] = new PointF(pointF[j, i - 1].X, pointF[j, i - 1].Y);
                        }
                    }
                }

                //画竖线
                for (int i = 0; i <= 20; i++)
                {
                    e.Graphics.DrawLine((i == 0 || i == 20) ? (Pens.White) : (pen1),
                                        new Point((this.Width - 40 - 135) * i / 20 + 40, 10),
                                        new Point((this.Width - 40 - 135) * i / 20 + 40, this.Height - 30 - 12));
                    if(i >= 20)
                    {
                        continue;
                    }
                    e.Graphics.DrawString((pStart + i * trackBar1.Value * 10).ToString(),
                                          this.Font,
                                          Brushes.White,
                                          new PointF((this.Width - 40 - 135) * i / 20 + 40, this.Height - 35),
                                          stringFormat);
                }

                //画横线
                for (int i = 0; i <= 20; i++)
                {
                    e.Graphics.DrawLine((i == 0 || i == 20) ? (Pens.White) : (pen1),
                                        new Point(40 + 1, (this.Height - 10 - 30 - 12) * i / 20 + 10),
                                        new Point(this.Width - 135, (this.Height - 10 - 30 - 12) * i / 20 + 10));
                    e.Graphics.DrawString(((10 - i) * max / 10).ToString(),
                                          this.Font,
                                          Brushes.White,
                                          new PointF(20, (this.Height - 10 - 30 - 12) * i / 20 + 5),
                                          stringFormat);
                }

                //画曲线
                if (checkBox1.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[0, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox1.ForeColor), pointf);
                }
                if (checkBox2.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[1, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox2.ForeColor), pointf);
                }
                if (checkBox3.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[2, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox3.ForeColor), pointf);
                }
                if (checkBox4.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[3, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox4.ForeColor), pointf);
                }
                if (checkBox5.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[4, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox5.ForeColor), pointf);
                }
                if (checkBox6.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[5, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox6.ForeColor), pointf);
                }
                if (checkBox7.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[6, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox7.ForeColor), pointf);
                }
                if (checkBox8.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[7, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox8.ForeColor), pointf);
                }
                if (checkBox9.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[8, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox9.ForeColor), pointf);
                }
                if (checkBox10.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[9, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox10.ForeColor), pointf);
                }
                if (checkBox11.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[10, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox11.ForeColor), pointf);
                }
                if (checkBox12.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[11, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox12.ForeColor), pointf);
                }
                if (checkBox13.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[12, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox13.ForeColor), pointf);
                }
                if (checkBox14.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[13, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox14.ForeColor), pointf);
                }
                if (checkBox15.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[14, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox15.ForeColor), pointf);
                }
                if (checkBox16.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[15, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox16.ForeColor), pointf);
                }
                if (checkBox17.Checked)
                {
                    PointF[] pointf = new PointF[xGain];
                    for (int i = 0; i < xGain; i++)
                    {
                        pointf[i] = pointF[16, i];
                    }
                    e.Graphics.DrawLines(new Pen(this.checkBox17.ForeColor), pointf);
                }
            }
            catch
            { 
            
            }
        }

        // 清除数据
        private void button1_Click(object sender, EventArgs e)
        {
            SerialPortStruct.valueCount = 0;
            for (int i = 0; i < SerialPortStruct.valueMax.Length; i++)
            {
                SerialPortStruct.valueMax[i] = 0;
                SerialPortStruct.valueMin[i] = 0;
            }
        }

        // 暂停,方便观察
        private void button2_Click(object sender, EventArgs e)
        {
            if(button2.BackColor == Color.Lime)
            {
                button2.BackColor = Color.Cyan;
                hScrollBar1.Enabled = false;
            }
            else
            {
                button2.BackColor = Color.Lime;
                hScrollBar1.Enabled = true;
                hScrollBar1.Maximum = (SerialPortStruct.valueCount > trackBar1.Value * 200) ? (SerialPortStruct.valueCount - trackBar1.Value * 200 + 1) : (1);
                hScrollBar1.Value = hScrollBar1.Maximum - 1;
            }
        }

        //X轴增益改变时调整滚动条位置
        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            if(hScrollBar1.Enabled)
            {
                hScrollBar1.Maximum = (SerialPortStruct.valueCount > trackBar1.Value * 200) ? (SerialPortStruct.valueCount - trackBar1.Value * 200 + 1) : (1);
                hScrollBar1.Value = hScrollBar1.Maximum - 1;
            }
        }
    }
}
