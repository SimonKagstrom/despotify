package se.despotify;

import se.despotify.client.player.Player;
import se.despotify.client.player.PlaybackListener;
import se.despotify.client.protocol.command.media.LoadTracks;
import se.despotify.domain.media.Track;
import org.junit.Test;

/**
 * @since 2009-jun-05 04:06:34
 */
public class TestPlayer extends DespotifyClientTest {

  @Test
  public void testNothing() {
    // foo
  }

  public static void main(String[] args) throws Exception {

    // todo this test should be automated to check for data, test pause et c.

    TestPlayer despotify = new TestPlayer();

    despotify.setUp();

    Connection connection = despotify.manager.getManagedConnection();

    Player player = new Player(connection);

    new LoadTracks(despotify.store, despotify.defaultTracks).send(connection);
    
    player.play(despotify.defaultTracks[0], new PlaybackListener(){
      @Override
      public void playbackStarted(Track track) {
        //To change body of implemented methods use File | Settings | File Templates.
      }

      @Override
      public void playbackStopped(Track track) {
        //To change body of implemented methods use File | Settings | File Templates.
      }

      @Override
      public void playbackResumed(Track track) {
        //To change body of implemented methods use File | Settings | File Templates.
      }

      @Override
      public void playbackPosition(Track track, int position) {
        //To change body of implemented methods use File | Settings | File Templates.
      }

      @Override
      public void playbackFinished(Track track) {
        //To change body of implemented methods use File | Settings | File Templates.
      }
    });

    Thread.sleep(10000);
    player.stop();
    despotify.tearDown();

  }

}
