using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Threading;


public class MainApp : MonoBehaviour 
{
    private Thread _t1;
    SerialPort stram = new SerialPort("COM4",9600);
    SerialDataReceivedEventHandler handler;
    float secondsCount;


    // Use this for initialization
    void Start () {
       
     
        OpenSerial();
    }

    public void OpenSerial() {
        try
        {
            handler = new SerialDataReceivedEventHandler(Stream_DataReceived);
            //  stram.BaudRate = 9600;
            //  stram.DataBits = 8;
            //  stram.StopBits = StopBits.One;
            //   stram.Parity = Parity.None;
            //  stram.PortName = "COM4";
            //stram.Parity = Parity.None;
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
            else
            {
                Debug.Log("Stream Not Open!!!");
            }

        }
        catch (System.Exception)
        {
            Debug.Log("Error Opening Serial Port:");
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

    bool wasVideoActivated = false;
    public VideoType newVid = VideoType.NotActiveMovie;

    int state = 0;
    private void Update()
    {
      //  Debug.Log("secondsCount =  " + secondsCount);
        secondsCount += Time.deltaTime;
        if (secondsCount >= 2 && state == 0) {
            state = 1;
            Debug.Log("UnprotectedStartMovie");
            video.GlobalVid.SetVideo(VideoType.ExplanationMovie);
        }
        if (secondsCount >= 40 && state == 1)
        {
            state = 2;
            Debug.Log("ProtectedMovie");
            video.GlobalVid.SetVideo(VideoType.ProtectedMovie);
        }
        if (secondsCount >= 60 && state == 2)
        {
            state = 3;
            Debug.Log("ExplanationMovie");
            video.GlobalVid.SetVideo(VideoType.UnprotectedStartMovie);
        }
        if (secondsCount >= 120 && state == 3)
        {
            state = 4;
            Debug.Log("ExplanationMovie");
            video.GlobalVid.SetVideo(VideoType.ExplanationMovie);
        }
        if (wasVideoActivated) {
            wasVideoActivated = false;
            video.GlobalVid.SetVideo(newVid);
            
        }
        
    }

    public void ChangeVido(int vid) {
        //NotActiveMovie = 0
        //ExplanationMovie = 1
        //ProtectedMovie = 2
        //UnprotectedStartMovie = 3
        //UprotectedEndMovie = 4
       switch (vid) {
            case 0: {
                    newVid = VideoType.NotActiveMovie;
                    break;
                }
            case 1:
                {
                    newVid = VideoType.ExplanationMovie;
                    break;
                }
            case 2:
                {
                    newVid = VideoType.ProtectedMovie;
                    break;
                }
            case 3:
                {
                    newVid = VideoType.UnprotectedStartMovie;
                    break;
                }
            case 4:
                {
                    newVid = VideoType.UprotectedEndMovie;
                    break;
                }
            default: {
                    newVid = VideoType.NotActiveMovie;
                    break;
                }

        }
        wasVideoActivated = true;
        
    }
    public void OnDestroy()
    {
        try
        {
            stram.Close();
            _t1.Abort();

        }
        catch (System.Exception)
        {
            Debug.Log("Error On Destroy Serial Port:");
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
               //     Debug.Log("input from ard:" + ans);
                   if (ans != null)
                   {
                        int intAns = int.Parse(ans);
                    //   Debug.Log("input from ard:" + ans);
                    //    Debug.Log("input from ard after parse:" + intAns);
                        ChangeVido(intAns);
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
