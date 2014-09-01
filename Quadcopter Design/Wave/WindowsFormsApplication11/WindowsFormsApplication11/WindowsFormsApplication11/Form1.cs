using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace WindowsFormsApplication11
{
    
    public partial class Form1 : Form
    {
        
        SerialPort mySerialPort = new SerialPort();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.comboBox1.Items.AddRange(SerialPort.GetPortNames());
            this.comboBox1.SelectedIndex = 0;

            this.comboBox2.SelectedIndex = 0;

            this.comboBox3.SelectedIndex = 0;

            this.trackBar1.ValueChanged += new EventHandler(trackBar1_ValueChanged);

            this.mySerialPort.DataReceived += new SerialDataReceivedEventHandler(mySerialPort_DataReceived);
        }

        void mySerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                while(mySerialPort.BytesToRead > 0)
                {
                    if(SerialPortStruct.receiveCount < SerialPortStruct.receiveBuffer.Length)
                    {
                        SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount++] = Convert.ToByte(mySerialPort.ReadByte());    
                        if((SerialPortStruct.receiveCount >= 50) &&
                           ((SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount - 50] == 0x7F) ||
                            (SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount - 49] == 0x7F) ||
                            (SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount - 48] == 0xFE)))
                        {
                            int tmp = 0;
                            for (int i = 0; i < 49; i++)
                            {
                                tmp += SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount - 50 + i];
                                tmp %= 256;
                            }
                            if(tmp == SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount - 1])
                            {
                                if (SerialPortStruct.valueCount < SerialPortStruct.valueBuffer.GetLength(0))
                                {
                                    if(userControl11.button2.BackColor != Color.Lime) 
                                    {
                                        for (int i = 0; i < 17; i++)
                                        {
                                            int val = (SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount + i * 2 - 47] << 8) +
                                                      SerialPortStruct.receiveBuffer[SerialPortStruct.receiveCount + i * 2 - 46];
                                            if (val > 0x7FFF)
                                            {
                                                val = (~val + 65537) * -1;
                                            }

                                            if (val > SerialPortStruct.valueMax[i])
                                            {
                                                SerialPortStruct.valueMax[i] = val;
                                            }
                                            if (val < SerialPortStruct.valueMin[i])
                                            {
                                                SerialPortStruct.valueMin[i] = val;
                                            }

                                            SerialPortStruct.valueBuffer[SerialPortStruct.valueCount, i] = val;
                                        }
                                        SerialPortStruct.receiveCount = 0;
                                        SerialPortStruct.valueCount++;
                                    }
                                }
                                else
                                {
                                    SerialPortStruct.valueCount = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        SerialPortStruct.receiveCount = 0;
                    }
                }
            }
            catch
            {

            }    
        }

        void trackBar1_ValueChanged(object sender, EventArgs e)
        {
            this.label4.Text = this.trackBar1.Value.ToString();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (this.button1.Text == "Open")
            {
                try
                {
                    this.mySerialPort.PortName = this.comboBox1.Text;
                    this.mySerialPort.BaudRate = Convert.ToInt32(comboBox2.Text);
                    this.mySerialPort.ReadBufferSize = 10000;
                    this.mySerialPort.Close();
                    this.mySerialPort.Open();
                    this.button1.Text = "Close";
                    this.pictureBox1.BackgroundImage = new Bitmap(@"..\..\bmp\ComOpen.bmp");    
                }
                catch
                {
                    MessageBox.Show("老大，就不能选个能用的串口么");
                }                 
            }
            else
            {
                try
                {
                    this.mySerialPort.Close();
                    this.button1.Text = "Open";
                    this.pictureBox1.BackgroundImage = new Bitmap(@"..\..\bmp\ComClose.bmp");
                }
                catch
                {
                    MessageBox.Show("关不了，不知道出啥问题了");
                }
            }
        }

        private void userControl11_Load(object sender, EventArgs e)
        {

        }
    }

    class SerialPortStruct
    {
        private static byte[] receiveBuffer0 = new byte[256];
        public static byte[] receiveBuffer
        {
            set
            {
                receiveBuffer0 = value;
            }

            get
            {
                return receiveBuffer0;
            }
        }

        private static int receiveCount0 = 0;
        public static int receiveCount
        {
            set
            {
                receiveCount0 = value;
            }

            get
            {
                return receiveCount0;
            }
        }

        private static int[,] valueBuffer0 = new int[100000, 32];
        public static int[,] valueBuffer
        {
            set
            {
                valueBuffer0 = value;
            }

            get
            {
                return valueBuffer0;
            }
        }

        private static int valueCount0 = 0;
        public static int valueCount
        {
            set
            {
                valueCount0 = value;
            }

            get
            {
                return valueCount0;
            }
        }

        private static int[] valueMax0 = new int[32];
        public static int[] valueMax
        {
            set
            {
                valueMax0 = value;
            }

            get
            {
                return valueMax0;
            }
        }

        private static int[] valueMin0 = new int[32];
        public static int[] valueMin
        {
            set
            {
                valueMin0 = value;
            }

            get
            {
                return valueMin0;
            }
        }
    }
}
