package se.despotify.client.protocol.command.media;

import se.despotify.DespotifyClientTest;
import se.despotify.domain.media.Result;
import org.junit.Test;

/**
 * @author kalle
 * @since 2009-jun-05 03:53:57
 */
public class TestSearch extends DespotifyClientTest {

  @Test
  public void test() throws Exception {
    Result result = (Result)manager.send(new Search(store, "Johnny Cash"));
    assertTrue(result.getTotalTracks() > 2000);
    assertEquals(100, result.getTracks().size());
    // todo assert a bit. at least we know there was no exception.
    System.currentTimeMillis();
  }
}
