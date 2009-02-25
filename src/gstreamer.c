#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include <gst/gst.h>

#include "gstapp/gstappsrc.h"
#include "gstapp/gstappbuffer.h"

#include "audio.h"
#include "sndqueue.h"
#include "gstreamer.h"
#include "util.h"

typedef struct gst_private
{
        int state;
        GstElement *pipe;
        GstElement *src;
        GstElement *sink;
        pthread_mutex_t lock;   /* Lock for this struct */
        pthread_cond_t pause;   /* condition signal used for pausing */
        pthread_cond_t end;     /* condition signal used when quiting */
} gst_PRIVATE;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer user_data)
{
    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_EOS:
            {
                                DSFYDEBUG ("%s", "End-of-stream\n");
                break;
            }
        case GST_MESSAGE_ERROR:
            {
                gchar *debug;
                GError *err;

                gst_message_parse_error (msg, &err, &debug);
                g_free (debug);

                                DSFYDEBUG ("%s\n", err->message);
                g_error_free (err);

                break;
            }
        case GST_MESSAGE_INFO:
            {
                gchar *debug;
                GError *err;

                gst_message_parse_info (msg, &err, &debug);
                g_free (debug);

                                DSFYDEBUG ("%s\n", err->message);
                g_error_free (err);

                break;
            }
        case GST_MESSAGE_WARNING:
            {
                gchar *debug;
                GError *err;

                gst_message_parse_warning (msg, &err, &debug);
                g_free (debug);

                                DSFYDEBUG ("%s\n", err->message);
                g_error_free (err);

                break;
            }
                case GST_MESSAGE_STATE_CHANGED:
                        {
                                GstState new;
                                GstState old;
                                GstState pending;

                                gst_message_parse_state_changed(msg, &new, &old, &pending);
                                DSFYDEBUG ("state changed (%d,%d,%d)\n", new, old, pending);

                break;
                        }
        default:
                        DSFYDEBUG ("%s %d\n", __FUNCTION__, GST_MESSAGE_TYPE(msg));
            break;
    }

    return 1;
}

static void gstaudio_free_buffer(void *buffer) {
        g_free(buffer);
}

static void need_data_cb (GstAppSrc * src, guint length, gpointer data) {
        ssize_t r;
        uint8_t * buffer;
    GstBuffer *gstbuf;
        AUDIOCTX * actx = (AUDIOCTX*) data;
        gst_PRIVATE *priv = (gst_PRIVATE *) actx->driverprivate;
        assert (priv != 0);

        if(length == 0 || length > 1024)
                length = 1024;

        buffer = g_new(uint8_t,length);
        r = pcm_read (actx->pcmprivate, (char *) buffer, length, 0, 2, 1, NULL);

        if (r <= 0) {
                if (r == 0)     {
                        gst_app_src_end_of_stream(GST_APP_SRC(priv->src));
                        return;
                }
                DSFYDEBUG ("pcm_read() failed == %lu\n", r);
                exit (-1);
        }

        gstbuf = gst_app_buffer_new (buffer, r, gstaudio_free_buffer, buffer);
    if(gst_app_src_push_buffer(GST_APP_SRC(priv->src), gstbuf) != GST_FLOW_OK)
                DSFYDEBUG ("%s> call to push_buffer failed\n", __FUNCTION__);
}

/*
 * Initialize and get an output device
 *
 */
int gstaudio_init_device (void *unused)
{
        DSFYDEBUG ("%s\n", __FUNCTION__);
        gst_init (0, NULL);
        gst_element_register (NULL, "appsrc", GST_RANK_NONE, GST_TYPE_APP_SRC);

        return 0;
}

/*
 * Prepare for playback by configuring sample rate, channels, ..
 * 
 */
int gstaudio_prepare_device (AUDIOCTX * actx)
{
        gst_PRIVATE *priv;
        DSFYDEBUG ("%s\n", __FUNCTION__);

        priv = (void *) malloc (sizeof (gst_PRIVATE));
        assert (priv != 0);
        
        priv->state = GST_IDLE;

        if (pthread_mutex_init (&priv->lock, 0) != 0) {
                perror ("pthread_mutex_init");
                exit (-1);
        }

        if (pthread_cond_init (&priv->pause, 0) != 0) {
                perror ("pthread_cond_init");
                exit (-1);
        }

        if (pthread_cond_init (&priv->end, 0) != 0) {
                perror ("pthread_cond_init");
                exit (-1);
        }
        
        /* create a new gstreamer pipeline */
        priv->pipe = gst_pipeline_new (NULL);
        g_assert (priv->pipe);
        
        /* create a gstreamer AppSrc
         * note im mostly guessing on the caps values */
        priv->src = gst_element_factory_make ("appsrc", NULL);
        g_assert (priv->src);
        gst_app_src_set_stream_type(GST_APP_SRC(priv->src), GST_APP_STREAM_TYPE_STREAM);
        gst_app_src_set_caps(GST_APP_SRC(priv->src),
                gst_caps_new_simple("audio/x-raw-int",
                                        "channels", G_TYPE_INT, actx->channels,
                                        "rate", G_TYPE_INT, (int) actx->samplerate,
                                        "signed", G_TYPE_BOOLEAN, TRUE,
                                        "endianness", G_TYPE_INT, G_LITTLE_ENDIAN,
                                        "width", G_TYPE_INT, 16,
                                        "depth", G_TYPE_INT, 16,
                                     NULL)
        );
        g_signal_connect(priv->src, "need-data", G_CALLBACK (need_data_cb), actx);
        gst_bin_add (GST_BIN (priv->pipe), priv->src);

        priv->sink = gst_element_factory_make ("autoaudiosink", NULL);
        g_assert (priv->sink);
        gst_bin_add (GST_BIN (priv->pipe), priv->sink);

        if(!gst_element_link (priv->src, priv->sink)) {
                fprintf(stderr,"failed to link gstreamer elements");
                exit(-1);
        }

        GstBus *bus;
        bus = gst_pipeline_get_bus(GST_PIPELINE (priv->pipe));
        gst_bus_add_watch(bus, bus_call, NULL);
        gst_object_unref(bus);

        actx->driverprivate = (void *) priv;
        return 0;
}

int gstaudio_pause (AUDIOCTX * actx)
{
        gst_PRIVATE *priv = (gst_PRIVATE *) actx->driverprivate;
        assert (priv != 0);
        DSFYDEBUG ("%s\n", __FUNCTION__);

        pthread_mutex_lock (&priv->lock);

        priv->state = GST_PAUSED;
        gst_element_set_state (priv->pipe, GST_STATE_PAUSED);

        pthread_mutex_unlock (&priv->lock);

        return (0);
}

int gstaudio_resume (AUDIOCTX * actx)
{

        gst_PRIVATE *priv = (gst_PRIVATE *) actx->driverprivate;
        assert (priv != 0);
        DSFYDEBUG ("%s\n", __FUNCTION__);

        pthread_mutex_lock (&priv->lock);

        priv->state = GST_PLAYING;
        gst_element_set_state (priv->pipe, GST_STATE_PLAYING);

        /* Signal player thread */
        pthread_cond_signal (&priv->pause);
        pthread_mutex_unlock (&priv->lock);

        return (0);
}



int gstaudio_play (AUDIOCTX * actx)
{
        GMainLoop *loop;
        gst_PRIVATE *priv = (gst_PRIVATE *) actx->driverprivate;
        assert (priv != 0);
        DSFYDEBUG ("%s\n", __FUNCTION__);
        
        gst_element_set_state(GST_ELEMENT (priv->pipe), GST_STATE_PLAYING);
        loop = g_main_loop_new(NULL, FALSE);
        for (;;) {
                pthread_mutex_lock (&priv->lock);
                switch(priv->state) {
                case GST_END:
                        pthread_cond_signal (&priv->end);
                        pthread_mutex_unlock (&priv->lock);
                        return 0;       /* exit thread */
                        break;

                case GST_PAUSED:
                        /* Wait for unpause signal */
                        pthread_cond_wait (&priv->pause, &priv->lock);
                        break;

                case GST_PLAYING:
                case GST_IDLE:
                default:
                        break;
                }
                pthread_mutex_unlock (&priv->lock);

                g_main_context_iteration(g_main_loop_get_context(loop), 1);
        }

        /* This will kill the thread */
        return 0;
}

int gstaudio_stop (AUDIOCTX * actx)
{
        gst_PRIVATE *priv = (gst_PRIVATE *) actx->driverprivate;
        assert (priv != 0);
        DSFYDEBUG ("%s\n", __FUNCTION__);

        /* Tell loop thread to exit */
        pthread_mutex_lock (&priv->lock);
        priv->state = GST_END;

        /* Wait for the player thread to end */
        pthread_cond_wait (&priv->end, &priv->lock);
        pthread_mutex_unlock (&priv->lock);

        if (actx->driverprivate)
                free (actx->driverprivate);

        return 0;
}

int gstaudio_free_device (void)
{
        DSFYDEBUG ("%s\n", __FUNCTION__);
        return 0;
}

AUDIODRIVER gstaudio_driver_desc = {
        gstaudio_init_device,
        gstaudio_free_device,

        gstaudio_prepare_device,
        gstaudio_play,
        gstaudio_stop,
        gstaudio_pause,
        gstaudio_resume
}

, *driver = &gstaudio_driver_desc;
