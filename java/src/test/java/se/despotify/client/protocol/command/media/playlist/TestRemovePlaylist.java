package se.despotify.client.protocol.command.media.playlist;

import se.despotify.DespotifyClientTest;
import se.despotify.domain.media.Playlist;
import org.junit.Test;

/**
 * @since 2009-apr-28 01:59:33
 */
public class TestRemovePlaylist extends DespotifyClientTest {    

  @Test
  public void test() throws Exception {

    String playlistName = randomPlaylistName();

    manager.send(new LoadUserPlaylists(store, user));
    int originalSize = user.getPlaylists().getItems().size();

    Playlist playlist = (Playlist)manager.send(new CreatePlaylist(store, user, playlistName, false));
    assertEquals(originalSize + 1, user.getPlaylists().getItems().size());
    assertTrue(user.getPlaylists().getItems().contains(playlist));

    manager.send(new RemovePlaylistFromUser(store, user, playlist));
    assertEquals(originalSize, user.getPlaylists().getItems().size());
    assertFalse(user.getPlaylists().getItems().contains(playlist));

    reset();

    manager.send(new LoadUserPlaylists(store, user));
    assertEquals(originalSize, user.getPlaylists().getItems().size());
    for (Playlist playlist2 : user.getPlaylists()) {
      assertNotSame(playlist.getByteUUID(), playlist2.getByteUUID());
    }
    

    // todo can we still load it?

  }


}
