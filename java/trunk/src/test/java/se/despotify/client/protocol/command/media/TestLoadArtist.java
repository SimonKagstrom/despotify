package se.despotify.client.protocol.command.media;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.media.Artist;
import se.despotify.domain.media.VisitorAdapter;
import se.despotify.util.SpotifyURI;
import se.despotify.util.Timer;

/**
 * @since 2009-apr-25 18:28:29
 */
public class TestLoadArtist extends DespotifyClientTest {

//  @Test
//  public void testLoadLots() throws Exception {
//
//    manager.getManagedConnection().close();
//
//    MemoryStore store = new MemoryStore();
//    Timer timer = new Timer("");
//
//    for (String artistId : twoHundredGoodArtistIds) {
//      Artist artist = store.getArtist(artistId);
//      timer.start();
//      new LoadArtist(store, artist).send(manager);
//      timer.stop();
//    }
//
//    System.out.println(timer);
//  }


  @Test
  public void test() throws Exception {

    // todo
//    Artist variousArtists = store.getArtist("19334eaffa3f4f2282e251e36611e26f");
//    manager.send(new LoadArtist(store, variousArtists));


    Artist dolly = store.getArtist(SpotifyURI.toHex("32vWCbZh0xZ4o9gkz4PsEU"));
    new LoadArtist(store, dolly).send(manager);


    for (Artist artist : defaultArtists) {
      new LoadArtist(store, artist).send(manager);
    }

//    for (Artist artist : defaultArtists) {
//      MediaTestCaseGenerator.createEqualsTest(artist, "artist = store.getArtist(\"" + artist.getHexUUID() + "\");\n" +
//          "    new LoadArtist(store, artist).send(manager);\n" +
//          "    artist");
//    }
//    System.out.flush();


    MemoryStore store = new MemoryStore();

    Artist artist;

    // generated tests


    artist = store.getArtist("d00d9e7b82894fb8851a109c82568eb5");
    new LoadArtist(store, artist).send(manager);
    artist.accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
        assertEquals("d00d9e7b82894fb8851a109c82568eb5", artist.getId());
        assertEquals("spotify:artist:6kACVPfCOnqzgfEF5ryl0x", artist.getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6kACVPfCOnqzgfEF5ryl0x", artist.getHttpURL());
        assertEquals("Johnny Cash", artist.getName());
//        assertNull(artist.getPopularity());
//        assertNull(artist.getPortrait());
        // TODO: artist.getSimilarArtists();
      }
    });
    artist = store.getArtist("4f9873e19e5a4b4096c216c98bcdb010");
    new LoadArtist(store, artist).send(manager);
    artist.accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
        assertEquals("4f9873e19e5a4b4096c216c98bcdb010", artist.getId());
        assertEquals("spotify:artist:2qc41rNTtdLK0tV3mJn2Pm", artist.getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/2qc41rNTtdLK0tV3mJn2Pm", artist.getHttpURL());
        assertEquals("Ryan Adams", artist.getName());
//        assertNull(artist.getPopularity());
//        assertNull(artist.getPortrait());
        // TODO: artist.getSimilarArtists();
      }
    });
    artist = store.getArtist("db614c7060fc47baa7be732d88ae446d");
    new LoadArtist(store, artist).send(manager);
    artist.accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
        assertEquals("db614c7060fc47baa7be732d88ae446d", artist.getId());
        assertEquals("spotify:artist:6FXMGgJwohJLUSr5nVlf9X", artist.getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6FXMGgJwohJLUSr5nVlf9X", artist.getHttpURL());
        assertEquals("Massive Attack", artist.getName());
//        assertNull(artist.getPopularity());
//        assertNull(artist.getPortrait());
        // TODO: artist.getSimilarArtists();
      }
    });
    artist = store.getArtist("f4d5d82d09124feda0633a2671f8c81a");
    new LoadArtist(store, artist).send(manager);
    artist.accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
        assertEquals("f4d5d82d09124feda0633a2671f8c81a", artist.getId());
        assertEquals("spotify:artist:7rZR0ugcLEhNrFYOrUtZii", artist.getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7rZR0ugcLEhNrFYOrUtZii", artist.getHttpURL());
        assertEquals("Miles Davis", artist.getName());
//        assertNull(artist.getPopularity());
//        assertNull(artist.getPortrait());
        // TODO: artist.getSimilarArtists();
      }
    });
    artist = store.getArtist("f6150726a8e94c89a7cf336d3f72be9c");
    new LoadArtist(store, artist).send(manager);
    artist.accept(new VisitorAdapter() {
      @Override
      public void visit(Artist artist) {
        assertEquals("f6150726a8e94c89a7cf336d3f72be9c", artist.getId());
        assertEquals("spotify:artist:7ulIMfVKiXh8ecEpAVHIAY", artist.getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7ulIMfVKiXh8ecEpAVHIAY", artist.getHttpURL());
        assertEquals("Electrelane", artist.getName());
//        assertNull(artist.getPopularity());
//        assertNull(artist.getPortrait());
        // TODO: artist.getSimilarArtists();
      }
    });


    // generated tests stops
  }


}