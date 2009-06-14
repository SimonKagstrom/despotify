package se.despotify.client.player;

import se.despotify.domain.media.Track;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.exceptions.DespotifyException;
import se.despotify.Connection;

/**
 * Consumer access to playing tunes on sound card in a background thread.
 *
 * @since 2009-jun-05 02:46:17
 */
public class Player {

  private Connection connection;

  public Player(Connection connection) {
    this.connection = connection;
    this.volume = 1.0f;

  }

  public Connection getConnection() {
    return connection;
  }

  private ChannelPlayer player;
  private float volume = 1f;

  /**
   * Play a track in a background thread.
   *
   * @param track    A {@link se.despotify.domain.media.Track} object identifying the track to be played.
   * @param listener event listener
   */
  public void play(Track track, PlaybackListener listener) {
    /* Create channel callback */
    ChannelCallback callback = new ChannelCallback();

    /* Send play request (token notify + AES key). */
    try {
      connection.getProtocol().sendPlayRequest(callback, track);
    }
    catch (DespotifyException e) {
      return;
    }

    /* Get AES key. */
    byte[] key = callback.getData("play response");

    /* Create channel player. */
    player = new ChannelPlayer(connection.getProtocol(), track, key, listener);
    player.volume(volume);

    /* Start playing. */
    this.play();
  }

  /**
   * Start playing or resume current track.
   */
  public void play() {
    if (this.player != null) {
      this.player.play();
    }
  }

  /**
   * Pause playback of current track.
   */
  public void pause() {
    if (this.player != null) {
      this.player.stop();
    }
  }

  /**
   * Stop playback of current track.
   */
  public void stop() {
    if (this.player != null) {
      this.player.close();

      this.player = null;
    }
  }

  /**
   * Get length of current track.
   *
   * @return Length in seconds or -1 if not available.
   */
  public int length() {
    if (this.player != null) {
      return this.player.length();
    }

    return -1;
  }

  /**
   * Get playback position of current track.
   *
   * @return Playback position in seconds or -1 if not available.
   */
  public int position() {
    if (this.player != null) {
      return this.player.position();
    }

    return -1;
  }

  /**
   * Get volume.
   *
   * @return A value from 0.0 to 1.0.
   */
  public float volume() {
    if (this.player != null) {
      return this.player.volume();
    }

    return -1;
  }

  /**
   * Set volume.
   *
   * @param volume A value from 0.0 to 1.0.
   */
  public void volume(float volume) {
    this.volume = volume;

    if (this.player != null) {
      this.player.volume(this.volume);
    }
  }
}
