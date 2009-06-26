package se.despotify.client.protocol.command.media;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.util.Timer;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.media.Track;
import se.despotify.domain.media.VisitorAdapter;

import java.util.ArrayList;
import java.util.List;

/**
 * @since 2009-apr-25 16:26:54
 */
public class TestLoadTracks extends DespotifyClientTest {

//  @Test
//  public void testRedirect() throws Exception {
//    Track track = store.getTrack("47d4ae9b03e64bc39b5728925550ee5f");
//    // 1c6794cce5664a51a2bc47f99496a90c good
//    // 47d4ae9b03e64bc39b5728925550ee5f redirected from
//    new LoadTracks(store, track).send(manager);
//    System.currentTimeMillis();
//  }

//  @Test
//  public void testLoadLots() throws Exception {
//
//    manager.getManagedConnection().close();
//
//    MemoryStore store = new MemoryStore();
//    List<Track> tracks = new ArrayList<Track>();
//    Timer timer = new Timer("");
//
//    int runs = 100;
//    int size = 200;
//
//    for (int i = 0; i < runs; i++) {
//      tracks.clear();
//      store.reset();
//      for (int i2 = 0; i2 < size; i2++) {
//        tracks.add(store.getTrack(twoHundredGoodTrackIds[i2]));
//      }
//      timer.start();
//      new LoadTracks(store, tracks).send(manager);
//      timer.stop();
//    }
//
//    System.out.println(timer);
//  }

  @Test
  public void testUnmarshalling() throws Exception {

    new LoadTracks(store, defaultTracks).send(manager);

//    for (Track track : defaultTracks) {
//      MediaTestCaseGenerator.createEqualsTest(track, "track = store.getTrack(\"" + track.getHexUUID() + "\");\n" +
//          "    new LoadTracks(store, track).send(manager);\n" +
//          "    track");
//    }
//    System.out.flush();

    Track track;

    // generated tests

    track = store.getTrack("93f98ea75b4748f797668485a3d01bd0");
    new LoadTracks(store, track).send(manager);
    track.accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {

        assertEquals("93f98ea75b4748f797668485a3d01bd0", track.getId());
        assertEquals("spotify:track:4vdV2Eua6RkUoUM51jdH56", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4vdV2Eua6RkUoUM51jdH56", track.getHttpURL());
        assertEquals("One", track.getTitle());
        assertTrue(track.getCover().getId().matches(hex40));
        assertEquals(2, track.getFiles().size());
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(231200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(20, track.getTrackNumber().intValue());
        assertEquals(2005, track.getYear().intValue());

        assertEquals("d00d9e7b82894fb8851a109c82568eb5", track.getArtist().getId());
        assertEquals("spotify:artist:6kACVPfCOnqzgfEF5ryl0x", track.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6kACVPfCOnqzgfEF5ryl0x", track.getArtist().getHttpURL());
        assertEquals("Johnny Cash", track.getArtist().getName());
        assertNull(track.getArtist().getPopularity());
        assertNull(track.getArtist().getPortrait());
        // TODO: track.getArtist().getSimilarArtists();

        assertEquals("spotify:album:05BIC4TZptbiQoF03QhojS", track.getAlbum().getSpotifyURI());
        assertEquals("http://open.spotify.com/album/05BIC4TZptbiQoF03QhojS", track.getAlbum().getHttpURL());
        assertEquals("The Legend Of Johnny Cash", track.getAlbum().getName());
        assertNull(track.getAlbum().getCover());
        assertEquals("02f8df4ad52d449caca8c6a25d2eca08", track.getAlbum().getId());
        assertNull(track.getAlbum().getPopularity());
      }
    });
    track = store.getTrack("cf2cd530980e450d855977ba0a80ec6e");
    new LoadTracks(store, track).send(manager);
    track.accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {

        assertEquals("cf2cd530980e450d855977ba0a80ec6e", track.getId());
        assertEquals("spotify:track:6iVTOPCmpABvG9jDZ2JozY", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6iVTOPCmpABvG9jDZ2JozY", track.getHttpURL());
        assertEquals("Two", track.getTitle());
        assertTrue(track.getCover().getId().matches(hex40));
        assertEquals(1, track.getFiles().size());
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(158293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertEquals(2007, track.getYear().intValue());

        assertEquals("4f9873e19e5a4b4096c216c98bcdb010", track.getArtist().getId());
        assertEquals("spotify:artist:2qc41rNTtdLK0tV3mJn2Pm", track.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/2qc41rNTtdLK0tV3mJn2Pm", track.getArtist().getHttpURL());
        assertEquals("Ryan Adams", track.getArtist().getName());
        assertNull(track.getArtist().getPopularity());
        assertNull(track.getArtist().getPortrait());
        // TODO: track.getArtist().getSimilarArtists();

        assertEquals("spotify:album:2mLIJwfgNPGjpuKaN7njPm", track.getAlbum().getSpotifyURI());
        assertEquals("http://open.spotify.com/album/2mLIJwfgNPGjpuKaN7njPm", track.getAlbum().getHttpURL());
        assertEquals("Easy Tiger", track.getAlbum().getName());
        assertNull(track.getAlbum().getCover());
        assertEquals("4dc7cec0b8e441daaef85f46a915c7d4", track.getAlbum().getId());
        assertNull(track.getAlbum().getPopularity());
      }
    });
    track = store.getTrack("fc1f1b5860f04a739971fcad9c1cd634");
    new LoadTracks(store, track).send(manager);
    track.accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {

        assertEquals("fc1f1b5860f04a739971fcad9c1cd634", track.getId());
        assertEquals("spotify:track:7FKhuZtIPchBVNIhFnNL5W", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7FKhuZtIPchBVNIhFnNL5W", track.getHttpURL());
        assertEquals("Three", track.getTitle());
        assertTrue(track.getCover().getId().matches(hex40));
        assertEquals(1, track.getFiles().size());
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(229066l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertEquals(1994, track.getYear().intValue());

        assertEquals("db614c7060fc47baa7be732d88ae446d", track.getArtist().getId());
        assertEquals("spotify:artist:6FXMGgJwohJLUSr5nVlf9X", track.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6FXMGgJwohJLUSr5nVlf9X", track.getArtist().getHttpURL());
        assertEquals("Massive Attack", track.getArtist().getName());
        assertNull(track.getArtist().getPopularity());
        assertNull(track.getArtist().getPortrait());
        // TODO: track.getArtist().getSimilarArtists();

        assertEquals("spotify:album:5CnZjFfPDmxOX7KnWLLqpC", track.getAlbum().getSpotifyURI());
        assertEquals("http://open.spotify.com/album/5CnZjFfPDmxOX7KnWLLqpC", track.getAlbum().getHttpURL());
        assertEquals("Protection", track.getAlbum().getName());
        assertNull(track.getAlbum().getCover());
        assertEquals("b8a09d31b4994b79a01f966b86cb9394", track.getAlbum().getId());
        assertNull(track.getAlbum().getPopularity());
      }
    });
    track = store.getTrack("7093f50c9ecf428eb780348c076f9f7f");
    new LoadTracks(store, track).send(manager);
    track.accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {

        assertEquals("7093f50c9ecf428eb780348c076f9f7f", track.getId());
        assertEquals("spotify:track:3qqKWUVfiLMrNPacFRzTzh", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3qqKWUVfiLMrNPacFRzTzh", track.getHttpURL());
        assertEquals("Four", track.getTitle());
        assertTrue(track.getCover().getId().matches(hex40));
        assertEquals(1, track.getFiles().size());
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(240226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertEquals(1986, track.getYear().intValue());

        assertEquals("f4d5d82d09124feda0633a2671f8c81a", track.getArtist().getId());
        assertEquals("spotify:artist:7rZR0ugcLEhNrFYOrUtZii", track.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7rZR0ugcLEhNrFYOrUtZii", track.getArtist().getHttpURL());
        assertEquals("Miles Davis", track.getArtist().getName());
        assertNull(track.getArtist().getPopularity());
        assertNull(track.getArtist().getPortrait());
        // TODO: track.getArtist().getSimilarArtists();

        assertEquals("spotify:album:6eEhgZIrHftYRvgpAKJC2K", track.getAlbum().getSpotifyURI());
        assertEquals("http://open.spotify.com/album/6eEhgZIrHftYRvgpAKJC2K", track.getAlbum().getHttpURL());
        assertEquals("Miles Davis And The Jazz Giants", track.getAlbum().getName());
        assertNull(track.getAlbum().getCover());
        assertEquals("cce79af3bd864a799806a557877dda7a", track.getAlbum().getId());
        assertNull(track.getAlbum().getPopularity());
      }
    });
    track = store.getTrack("48daf12f96f84793a526b579aa4d1f66");
    new LoadTracks(store, track).send(manager);
    track.accept(new VisitorAdapter() {
      @Override
      public void visit(Track track) {

        assertEquals("48daf12f96f84793a526b579aa4d1f66", track.getId());
        assertEquals("spotify:track:2dtvgPd3vsotKXtGk4dWlg", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2dtvgPd3vsotKXtGk4dWlg", track.getHttpURL());
        assertEquals("Five", track.getTitle());
        assertTrue(track.getCover().getId().matches(hex40));
        assertEquals(1, track.getFiles().size());
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(385000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertEquals(2007, track.getYear().intValue());

        assertEquals("f6150726a8e94c89a7cf336d3f72be9c", track.getArtist().getId());
        assertEquals("spotify:artist:7ulIMfVKiXh8ecEpAVHIAY", track.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7ulIMfVKiXh8ecEpAVHIAY", track.getArtist().getHttpURL());
        assertEquals("Electrelane", track.getArtist().getName());
        assertNull(track.getArtist().getPopularity());
        assertNull(track.getArtist().getPortrait());
        // TODO: track.getArtist().getSimilarArtists();

        assertEquals("spotify:album:3GETv5yNXeM0cnhq8XahWu", track.getAlbum().getSpotifyURI());
        assertEquals("http://open.spotify.com/album/3GETv5yNXeM0cnhq8XahWu", track.getAlbum().getHttpURL());
        assertEquals("No Shouts, No Calls", track.getAlbum().getName());
        assertNull(track.getAlbum().getCover());
        assertEquals("792d90d6e5c14679afd00e7ea28982ce", track.getAlbum().getId());
        assertNull(track.getAlbum().getPopularity());
      }
    });


    // end generated tests
  }


}
