cimport despotify

cdef extern from "audio_thread.h":
    cdef struct thread_state:
        pass

    thread_state* thread_init(despotify.despotify_session* session)
    void thread_play(thread_state*)
    void thread_pause(thread_state*)
    void thread_exit(thread_state*)
