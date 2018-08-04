using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Video;

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
    }

    public void SetVideo(int v) {
        if (runningVido != null) {
            StopCoroutine(runningVido);
        }
        
        runningVido = StartCoroutine(playVideo(v));

    }
 



    public IEnumerator playVideo(int vid) {

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

        //Set video To Play then prepare Audio to prevent Buffering
        videoPlayer.clip = videosToPlay[vid];
        videoPlayer.Prepare();

        //Wait until video is prepared
        while (!videoPlayer.isPrepared)
        {
            Debug.Log("Preparing Video");
            yield return null;
        }

        Debug.Log("Done Preparing Video");


        //Play Video
        videoPlayer.Play();

        //Play Sound
        audioSource.Play();

        Debug.Log("Playing Video");
        while (videoPlayer.isPlaying)
        {
           // Debug.LogWarning("Video Time: " + Mathf.FloorToInt((float)videoPlayer.time));
            yield return null;
        }

        Debug.Log("Done Playing Video");
    }

   
}
