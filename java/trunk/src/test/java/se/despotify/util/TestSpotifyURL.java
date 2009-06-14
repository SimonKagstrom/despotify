package se.despotify.util;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.Connection;
import se.despotify.exceptions.DespotifyException;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.media.*;

/**
 *
 * @since 2009-apr-20 20:31:16
 */
public class TestSpotifyURL extends DespotifyClientTest {

  @Test
  public void testURLtransformer() throws Exception {

    SpotifyURL.URLtransformer transformer = new SpotifyURL.URLtransformer();

    assertEquals("spotify:track:123", SpotifyURL.match("http://open.spotify.com/track/123", transformer));
    assertEquals("spotify:album:123", SpotifyURL.match("http://open.spotify.com/album/123", transformer));
    assertEquals("spotify:artist:123", SpotifyURL.match("http://open.spotify.com/artist/123", transformer));
    assertEquals("spotify:user:kent.finell:playlist:123", SpotifyURL.match("http://open.spotify.com/user/kent.finell/playlist/123", transformer));

    assertEquals("http://open.spotify.com/track/123", SpotifyURL.match("spotify:track:123", transformer));
    assertEquals("http://open.spotify.com/album/123", SpotifyURL.match("spotify:album:123", transformer));
    assertEquals("http://open.spotify.com/artist/123", SpotifyURL.match("spotify:artist:123", transformer));
    assertEquals("http://open.spotify.com/user/kent.finell/playlist/123", SpotifyURL.match("spotify:user:kent.finell:playlist:123", transformer));

    try {
      SpotifyURL.match("spotify:moo:bar", transformer);
      fail();
    } catch (DespotifyException e) {

    }

  }

  @Test
  public void testBrowseTrack() {
    Connection connection = manager.getManagedConnection();
    SpotifyURL.browse("spotify:track:7lF0U328NdKSIPXEOWEpea",  new MemoryStore(), connection).accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {
      }
    });
    connection.close();
  }

  @Test
  public void testBrowseArtist() {
    Connection connection = manager.getManagedConnection();
    SpotifyURL.browse("spotify:artist:0WjkBDqno4HbjwNDqyMgVa", new MemoryStore(), connection).accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
      }
    });
    connection.close();
  }

  @Test
  public void testBrowseAlbum() {
    Connection connection = manager.getManagedConnection();
    SpotifyURL.browse("spotify:album:6XOpVcNWQD7kXDjtrWM968", new MemoryStore(), connection).accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
      }
    });
    connection.close();
  }

  @Test
  public void testBrowsePlaylist() {
    Connection connection = manager.getManagedConnection();
    SpotifyURL.browse("spotify:user:kent.finell:playlist:6wvPFkLGKOVl1v3qRJD6HX", new MemoryStore(), connection).accept(new VisitorAdapter() {
      @Override
      public void visit(Playlist playlist) {
      }
    });
    connection.close();
  }

}
