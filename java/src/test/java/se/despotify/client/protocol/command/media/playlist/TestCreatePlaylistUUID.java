package se.despotify.client.protocol.command.media.playlist;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.util.Hex;

import java.util.Random;

/**
 * @since 2009-apr-24 09:03:13
 */
public class TestCreatePlaylistUUID extends DespotifyClientTest {

  @Test
  public void test() throws Exception {

    long seed = System.currentTimeMillis();
    Random random = new Random(seed);

    byte[] playlistNameBytes = new byte[8];
    random.nextBytes(playlistNameBytes);
    String playlistName = "despotify_TestCreatePlaylistUUID_" + Hex.toHex(playlistNameBytes);


    byte[] UUID = new ReserveRandomPlaylistUUID(store, user, playlistName, false).send(connection.getProtocol());
    assertNotNull(UUID);
    assertEquals(16, UUID.length);
    
  }

}
