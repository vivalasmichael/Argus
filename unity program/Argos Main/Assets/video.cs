using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Video;

public enum VideoType
{
    NotActiveMovie = 0,
    ExplanationMovie = 1,
    ProtectedMovie = 2,
    UnprotectedStartMovie = 3,
    UprotectedEndMovie = 4
};

public class video : MonoBehaviour {
    public List<VideoPlayer> videoPlayer;

    public List<VideoClip> videosToPlay;
    //Audio
    public AudioSource audioSource;
    public VideoSource videoSource;

    public static video GlobalVid;
    public List<Coroutine> runningVido;

    public int moviePlaying  = -1;

    void Start()
    {
        runningVido = new List<Coroutine>();
        GlobalVid = this;
        for (int i = 0; i < 5; i++) {

            StartCoroutine(PreparVideo(i));

        }
     
    }

    public IEnumerator PreparVideo(int i)
    {
        //Application.runInBackground = true;
        //Disable Play on Awake for both Video and Audio
        videoPlayer[i].playOnAwake = false;
        audioSource.playOnAwake = false;

        //We want to play from video clip not from url
        videoPlayer[i].source = VideoSource.VideoClip;

        //Set Audio Output to AudioSource
        videoPlayer[i].audioOutputMode = VideoAudioOutputMode.AudioSource;

        //Assign the Audio from Video to AudioSource to be played
        videoPlayer[i].EnableAudioTrack(0, true);
        videoPlayer[i].SetTargetAudioSource(0, audioSource);

        //Set video To Play then prepare Audio to prevent Buffering
        videoPlayer[i].clip = videosToPlay[i];
        videoPlayer[i].Prepare();

        //Wait until video is prepared
          while (!videoPlayer[i].isPrepared)
           {
       
          yield return null;
          }
        videoPlayer[i].targetCameraAlpha = 0f;
        Debug.Log("Prepared Video " + i);
        //   Debug.Log("Done Preparing Video");
        // Skip the first 100 frames.
        //  videoPlayer.frame = 1700;
     //   videoPlayer[i].isLooping = true;
        /// Static Replay Movie No activity

        if (i == (int)VideoType.NotActiveMovie)
               {
                   // Restart from beginning when done.
                    videoPlayer[i].isLooping = true;
               }
               /// Static Replay Movie ransom  
               else if (i == (int)VideoType.UnprotectedStartMovie)
               {
                   // Restart from beginning when done.
                   videoPlayer[i].loopPointReached += VideoRansomStartMovie;
               }
               else if (i == (int)VideoType.UprotectedEndMovie)
               {
                   // Restart from beginning when done.
                   videoPlayer[i].loopPointReached += VideoRansomReplayReached;
                   // videoPlayer.isLooping = true;
               }
               else if (i == (int)VideoType.ProtectedMovie)
               {
                   // Restart from beginning when done.
                   videoPlayer[i].loopPointReached += VideoEndReachedProtected;
               }
        else if (i == (int)VideoType.ExplanationMovie)
        {
            // Restart from beginning when done.
            videoPlayer[i].loopPointReached += VideoEndReachedExplanation;
        }


    }


    public void SetVideo(VideoType v, bool fade) {
        if(moviePlaying != -1)
        {
            StartCoroutine(fadeoutVideo((VideoType)moviePlaying));
        }
        Coroutine rot = StartCoroutine(playVideo( v ) );
        moviePlaying = (int)v;

       // runningVido[(int)v] = rot;
    }

    bool fadeoutDone = false;
    public IEnumerator fadeoutVideo(VideoType v) {
        Debug.Log((int)v + " movie #" + (int)v + " faiding");
        float a = 1.0f;
        while ( a >= 0f) {
            float colo = videoPlayer[(int)v].targetCameraAlpha;
            colo = a;
            videoPlayer[(int)v].targetCameraAlpha = colo;
            a -= Time.deltaTime * 2;
            yield return null;
        }
        videoPlayer[(int)v].targetCameraAlpha = 0f;
      //  videoPlayer[(int)v].stop();
        videoPlayer[(int)v].Pause();
        audioSource.Pause();


    }



    public IEnumerator playVideo(VideoType vid) {
        Debug.Log(vid + " movie #" +(int)vid + " played");
        videoPlayer[(int)vid].frame = 0;
        while (!videoPlayer[(int)vid].isPrepared)
        {
            yield return null;
        }

        //Play Video
        videoPlayer[(int)vid].Play();

        //Play Sound
        audioSource.Play();
       // videoPlayer.targetCameraAlpha = 1f;
        //  Debug.Log("Playing Video");
        while (videoPlayer[(int)vid].isPlaying)
        {
            if (videoPlayer[(int)vid].targetCameraAlpha <= 1f) {
                float a = videoPlayer[(int)vid].targetCameraAlpha;
                a += Time.deltaTime * 2;
                videoPlayer[(int)vid].targetCameraAlpha = a;
                if (a >= 0.99) {
                    videoPlayer[(int)vid].targetCameraAlpha = 1f;
                }
                
               // yield return null;
            }
            
            if (Mathf.FloorToInt((float)videoPlayer[(int)vid].time) >= 10)
            {
                if (!testRun)
                {
                   // testRun = true;
                   // Debug.Log("Started Video");
                   // SetVideo(1);
                }

                //Debug.LogWarning("Video Time: " + Mathf.FloorToInt((float)videoPlayer[(int)vid].time));
            }

           
            yield return null;
        }


        //videoPlayer.Stop();
    }

    bool testRun = false;
    
        public void VideoEndReachedExplanation(VideoPlayer vp)
    {
       // StartCoroutine(fadeoutVideo(VideoType.ExplanationMovie));
        Debug.Log("end point normal reached");
        SetVideo(VideoType.NotActiveMovie, true);

    }
    public void VideoEndReachedProtected(VideoPlayer vp) {
      //  StartCoroutine(fadeoutVideo(VideoType.ProtectedMovie));
        Debug.Log("end point normal reached");
        SetVideo(VideoType.NotActiveMovie,true);

    }

    int ransomMovieTimesPlayed = 1;
    int ransomMovieMaxTimesPlayed = 1;

    public void VideoRansomReplayReached(VideoPlayer vp)
    {
        if (ransomMovieTimesPlayed >= ransomMovieMaxTimesPlayed)
        {
            ransomMovieTimesPlayed = 0;
            Debug.Log("end point Ransom reached at max times and starting inactive loop");
       //     StartCoroutine(fadeoutVideo(VideoType.UprotectedEndMovie));
            SetVideo(VideoType.NotActiveMovie, true);
        }
        else {
            SetVideo(VideoType.UprotectedEndMovie, false);
            // SetVideo(VideoType.UprotectedEndMovie, false);
            Debug.Log("end point Ransom reached at times : " + ransomMovieTimesPlayed);
            ransomMovieTimesPlayed++;
        }
        

    }

    public void VideoRansomStartMovie(VideoPlayer vp)
    {
        
        Debug.Log("end point unprotected reached");
     //   StartCoroutine(fadeoutVideo(VideoType.UnprotectedStartMovie));
        SetVideo(VideoType.UprotectedEndMovie,false);
    }


}
