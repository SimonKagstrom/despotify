package se.despotify.client.protocol.command.media.playlist;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.domain.media.Playlist;
import se.despotify.domain.media.PlaylistContainer;

/**
 *
 * @since 2009-apr-22 03:00:06
 */
public class TestLoadUserPlaylists extends DespotifyClientTest {

  @Test
  public void testGetPlaylists() throws Exception {

    manager.send(new LoadUserPlaylists(store, user));
    PlaylistContainer playlists  = user.getPlaylists();
    assertNotNull(playlists);
    assertEquals(username, playlists.getId());
    for (Playlist playlist : playlists) {      
      assertEquals(16, playlist.getByteUUID().length);
      testGetPlaylist(playlist);
    }


  }


  private void testGetPlaylist(Playlist playlist) throws Exception {


    assertNull(playlist.getName());

    System.out.println("asserting playlist " + playlist.toString());

    manager.send(new LoadPlaylist(store, playlist));

    assertNotNull(playlist.getName());

    System.err.println(playlist.getName() + "\t" + playlist.getSpotifyURI());

    assertNotNull(playlist);
    assertNotNull(playlist.getByteUUID());
    assertNotNull(playlist.getName());
    assertNotNull(playlist.getAuthor());
    assertNotNull(playlist.isCollaborative());
    assertNotNull(playlist.getChecksum());

  }




}
