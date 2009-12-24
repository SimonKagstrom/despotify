#ifndef AUDIO_THREAD_H
#define AUDIO_THREAD_H

enum play_state {
    PAUSE,
    PLAY,
    EXIT
};

struct thread_state {
    pthread_t thread;
    pthread_mutex_t thread_mutex;
    pthread_cond_t thread_cond;

    void* audio_device;
    struct despotify_session* session;
    enum play_state state;
};

struct thread_state* thread_init(struct despotify_session* session);
void thread_play(struct thread_state* ts);
void thread_pause(struct thread_state* ts);
void thread_exit(struct thread_state* ts);

#endif
