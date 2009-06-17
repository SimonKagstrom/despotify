package se.despotify.client.protocol.command.media.playlist;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.DespotifyClientTest;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.media.Playlist;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;

/**
 *
 * @since 2009-apr-22 03:00:06
 */
public class TestCreatePlaylist extends DespotifyClientTest {

  private static final Logger log = LoggerFactory.getLogger(TestCreatePlaylist.class);

  @Test
  public void testCreatePlaylist() throws Exception {

    String playlistName = randomPlaylistName();

    log.info("\n\n\n\n\n\n\n           create playlist named "+playlistName+"\n\n\n\n\n\n\n\n\n");

    Playlist originalPlaylist = (Playlist)manager.send(new CreatePlaylist(new MemoryStore(), user, playlistName, false));

    log.info("\n\n\n\n\n\n\n           load playlist with UUID "+Hex.toHex(originalPlaylist.getByteUUID())+"\n\n             spotify:user:"+ username + ":playlist:" + SpotifyURI.toURI(Hex.toHex(originalPlaylist.getByteUUID())) +"\n\n\n\n\n\n\n\n\n");

    // brand new store to make sure we dont pick something up from the cache.
    MemoryStore store = new MemoryStore();
    Playlist loadedPlaylist = store.getPlaylist(originalPlaylist.getByteUUID());
    manager.send(new LoadPlaylist(store, loadedPlaylist));

    assertTrue("different stores means different instances", loadedPlaylist != originalPlaylist);
    assertEquals(loadedPlaylist.getChecksum().longValue(), loadedPlaylist.calculateChecksum());
    assertEquals(originalPlaylist.getId(), loadedPlaylist.getId());
    // todo assert the rest


    // todo add playlist at top position
  }



}