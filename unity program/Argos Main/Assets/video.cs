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
    public VideoPlayer videoPlayer;
    public VideoSource videoSource;
    public List<VideoClip> videosToPlay;
    //Audio
    public AudioSource audioSource;

    public static video GlobalVid;
    public Coroutine runningVido;



    void Start()
    {
        GlobalVid = this;
        //Application.runInBackground = true;
        //Disable Play on Awake for both Video and Audio
        videoPlayer.playOnAwake = false;
        audioSource.playOnAwake = false;

        //We want to play from video clip not from url
        videoPlayer.source = VideoSource.VideoClip;

        //Set Audio Output to AudioSource
        videoPlayer.audioOutputMode = VideoAudioOutputMode.AudioSource;

        //Assign the Audio from Video to AudioSource to be played
        videoPlayer.EnableAudioTrack(0, true);
        videoPlayer.SetTargetAudioSource(0, audioSource);
    }

    public void SetVideo(VideoType v, bool fade) {
        
        if (runningVido != null)
        {
            if (videoPlayer.isPlaying)
            {
                if (fade)
                {
                    StartCoroutine(fadeoutVideo(v));
                }
                else {
                    RunMovie(v);

                }
                
            }
        }
        else {
            RunMovie(v);
        }

    }

    void RunMovie(VideoType v) {
        if(runningVido != null)
        {
           // videoPlayer.isLooping = false;
            StopCoroutine(runningVido);
        }
        videoPlayer.Stop();
        runningVido = StartCoroutine(playVideo(v));
    }

    bool fadeoutDone = false;
    public IEnumerator fadeoutVideo(VideoType v) {

        float a = 1.0f;
        while ( a >= 0f) {
            float colo = videoPlayer.targetCameraAlpha;
            colo = a;
            videoPlayer.targetCameraAlpha = colo;
            a -= Time.deltaTime;
            yield return null;
        }


        if (a <= 0.0f)
        {
            
            RunMovie(v);
        }
        


    }



    public IEnumerator playVideo(VideoType vid) {
        Debug.Log(vid + " movie #" +(int)vid + " played");
        
        //Set video To Play then prepare Audio to prevent Buffering
        videoPlayer.clip = videosToPlay[(int)vid];
        videoPlayer.Prepare();

        //Wait until video is prepared
        while (!videoPlayer.isPrepared)
        {
           // Debug.Log("Preparing Video");
            yield return null;
        }

        //   Debug.Log("Done Preparing Video");
        // Skip the first 100 frames.
        //  videoPlayer.frame = 1700;
        videoPlayer.isLooping = true;
        /// Static Replay Movie No activity
        if (vid == VideoType.NotActiveMovie)
        {
            // Restart from beginning when done.
           // videoPlayer.isLooping = true;
        }
        /// Static Replay Movie ransom  
        else if (vid == VideoType.UnprotectedStartMovie)
        {
            // Restart from beginning when done.
            videoPlayer.loopPointReached += VideoRansomStartMovie;
        }
        else if (vid == VideoType.UprotectedEndMovie)
        {
            // Restart from beginning when done.
            
            videoPlayer.loopPointReached += VideoRansomReplayReached;
           // videoPlayer.isLooping = true;
        }
        else if (vid == VideoType.ProtectedMovie || vid == VideoType.ExplanationMovie)
        {
            // Restart from beginning when done.
            videoPlayer.loopPointReached += VideoEndReached;
        }

        //Play Video
        videoPlayer.Play();

        //Play Sound
        audioSource.Play();
       // videoPlayer.targetCameraAlpha = 1f;
        //  Debug.Log("Playing Video");
        while (videoPlayer.isPlaying)
        {
            if (videoPlayer.targetCameraAlpha <= 1f) {
                float a = videoPlayer.targetCameraAlpha;
                a += Time.deltaTime;
                videoPlayer.targetCameraAlpha = a;
                if (a >= 0.99) {
                    videoPlayer.targetCameraAlpha = 1f;
                }
                
               // yield return null;
            }
            
            if (Mathf.FloorToInt((float)videoPlayer.time) >= 10)
            {
                if (!testRun)
                {
                   // testRun = true;
                   // Debug.Log("Started Video");
                   // SetVideo(1);
                }
                

            }
           // Debug.LogWarning("Video Time: " + Mathf.FloorToInt((float)videoPlayer.time));
            yield return null;
        }


        //videoPlayer.Stop();
    }

    bool testRun = false;
    
    public void VideoEndReached(VideoPlayer vp) {

        Debug.Log("end point normal reached");
        SetVideo(VideoType.NotActiveMovie,true);

    }

    int ransomMovieTimesPlayed = 1;
    int ransomMovieMaxTimesPlayed = 2;

    public void VideoRansomReplayReached(VideoPlayer vp)
    {
        if (ransomMovieTimesPlayed >= ransomMovieMaxTimesPlayed)
        {
            ransomMovieTimesPlayed = 0;
            Debug.Log("end point Ransom reached at max times and starting inactive loop");
            SetVideo(VideoType.NotActiveMovie, true);
        }
        else {
           // SetVideo(VideoType.UprotectedEndMovie, false);
            Debug.Log("end point Ransom reached at times : " + ransomMovieTimesPlayed);
            ransomMovieTimesPlayed++;
        }
        

    }

    public void VideoRansomStartMovie(VideoPlayer vp)
    {
        Debug.Log("end point unprotected reached");
        SetVideo(VideoType.UprotectedEndMovie,false);
    }


}
