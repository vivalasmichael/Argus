using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Threading;


public class MainApp : MonoBehaviour 
{
    private Thread _t1;
    SerialPort stram = new SerialPort("COM11",9600);
    SerialDataReceivedEventHandler handler;
    
    // Use this for initialization
    void Start () {
        video.GlobalVid.SetVideo(0);
        handler = new SerialDataReceivedEventHandler(Stream_DataReceived);
      //  stram.BaudRate = 9600;
      //  stram.DataBits = 8;
      //  stram.StopBits = StopBits.One;
     //   stram.Parity = Parity.None;
      //  stram.PortName = "COM4";
        //   stram.Parity = Parity.None;
      //  stram.StopBits = StopBits.One;
        stram.DtrEnable = true;
       stram.Handshake = Handshake.None;
    //   stram.RtsEnable = true;
        stram.DataReceived += handler;

        stram.Open();
          if (stram.IsOpen)
          {
              Debug.Log("Stream Open!!!");
            _t1 = new Thread(SerialRun);
            _t1.Start();
        }
          else {
              Debug.Log("Stream Not Open!!!");
          }
       

    }

    public void Stream_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        Debug.Log("input from ard:" );
        if (stram.IsOpen)
        {
            string s = stram.ReadLine();//reads the serialport buffer
            Debug.Log("input from ard:" + s);
        }
    }

 
    public void SerialRun() {
        try
        {
            while (true)
            {
                if (stram.IsOpen)
                {
                    string ans = stram.ReadLine();
                    
                    if (ans != null)
                    {
                        int intAns = int.Parse(ans);
                        Debug.Log("input from ard:" + ans);
                        Debug.Log("input from ard after parse:" + intAns);
                    }

                }
            }
           
        }
        catch (System.Exception)
        {
            Debug.Log("Error:");
        }

    }

  
}
