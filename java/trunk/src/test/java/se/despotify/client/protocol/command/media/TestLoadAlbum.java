package se.despotify.client.protocol.command.media;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.util.SpotifyURI;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.VisitorAdapter;
import se.despotify.domain.media.Track;

import java.util.regex.Pattern;

/**
 * @since 2009-apr-25 18:28:29
 */
public class TestLoadAlbum extends DespotifyClientTest {

  
  @Test
  public void test() throws Exception {

// test generation>
//
//    for (Album album : defaultAlbums) {
//      new LoadAlbum(store, album).send(manager);
//    }
//
//    for (Album album : defaultAlbums) {
//      MediaTestCaseGenerator.createEqualsTest(album, "album = store.getAlbum(\""+album.getHexUUID()+"\");\n" +
//          "    new LoadAlbum(store, album).send(manager);\n" +
//          "    album");
//    }
//
// < test generation
    
    final String hex40 = "^[a-f0-9]{40}$";
    
    Album album;

    // generated tests follows

    album = store.getAlbum(SpotifyURI.toHex("1rKQfglNABmKc8K0Kmfvdz"));
    new LoadAlbum(store, album).send(manager);

    album = store.getAlbum("02f8df4ad52d449caca8c6a25d2eca08");
    new LoadAlbum(store, album).send(manager);
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:05BIC4TZptbiQoF03QhojS", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/05BIC4TZptbiQoF03QhojS", album.getHttpURL());
        assertEquals("The Legend Of Johnny Cash", album.getName());
        assertTrue(album.getCover().getId().matches(hex40));
        assertEquals("02f8df4ad52d449caca8c6a25d2eca08", album.getId());
        assertNull(album.getPopularity());

        assertEquals(21, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3GhpgjhCNZZa6Lb7Wtrp3S", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3GhpgjhCNZZa6Lb7Wtrp3S", track.getHttpURL());
        assertEquals("Ring Of Fire", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(155866l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3uatQAOZ90mnrixwhKiSAI", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3uatQAOZ90mnrixwhKiSAI", track.getHttpURL());
        assertEquals("I Walk The Line", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(164440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4fQNEmRzjhgmPe5TkL6WNQ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4fQNEmRzjhgmPe5TkL6WNQ", track.getHttpURL());
        assertEquals("Jackson", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(165973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1Caywbok6SQb7czVXzi2ro", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1Caywbok6SQb7czVXzi2ro", track.getHttpURL());
        assertEquals("Folsom Prison Blues", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(169506l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:41z2CLPSwQVA0hVNvu28ej", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/41z2CLPSwQVA0hVNvu28ej", track.getHttpURL());
        assertEquals("A Boy Named Sue", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));
        assertEquals(225466l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7jKMavlOi9N4ZZG6755hk7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7jKMavlOi9N4ZZG6755hk7", track.getHttpURL());
        assertEquals("Big River", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(151293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6KCzeoC39w23DZ3tZ88mKz", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6KCzeoC39w23DZ3tZ88mKz", track.getHttpURL());
        assertEquals("Get Rhythm", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(133240l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1poMNfOIDOIxltyUYjV6qn", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1poMNfOIDOIxltyUYjV6qn", track.getHttpURL());
        assertEquals("Cry! Cry! Cry!", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(144226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6DyHB5mWMzUfP3QEXwwVUZ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6DyHB5mWMzUfP3QEXwwVUZ", track.getHttpURL());
        assertEquals("Hey Porter", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(132906l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6VdwOFKf0u6hLVtXcL8CWn", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6VdwOFKf0u6hLVtXcL8CWn", track.getHttpURL());
        assertEquals("A Thing Called Love - Original Version", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(151360l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6yYcerWiS6tebVzoIpu74j", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6yYcerWiS6tebVzoIpu74j", track.getHttpURL());
        assertEquals("Guess Things Happen That Way - 1958 Single Version", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(110000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(11);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3v6m3ICCXoMzZw3EMr5Kn2", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3v6m3ICCXoMzZw3EMr5Kn2", track.getHttpURL());
        assertEquals("San Quentin - Live Version", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(185880l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(12, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(12);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3AgqeAt9qzUdSBor27IRn4", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3AgqeAt9qzUdSBor27IRn4", track.getHttpURL());
        assertEquals("Man In Black - Single Version", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(172226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(13, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(13);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:71z0kIckftTVfBSG0HBHAx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/71z0kIckftTVfBSG0HBHAx", track.getHttpURL());
        assertEquals("Highwayman", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(183426l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(14, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(14);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:41OfOvVyuD5x185BTOW0ET", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/41OfOvVyuD5x185BTOW0ET", track.getHttpURL());
        assertEquals("The Wanderer", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(284173l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(15, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(15);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6y8zaPjL3eHZW3UAuL2VJo", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6y8zaPjL3eHZW3UAuL2VJo", track.getHttpURL());
        assertEquals("I've Been Everywhere", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(196386l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(16, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(16);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:48pt86Kh5s2cuhkJLy1Pue", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/48pt86Kh5s2cuhkJLy1Pue", track.getHttpURL());
        assertEquals("Rusty Cage", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(169493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(17, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(17);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:2CYqXSe4pzCqpSWZvqgE39", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2CYqXSe4pzCqpSWZvqgE39", track.getHttpURL());
        assertEquals("Personal Jesus", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(198960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(18, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(18);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1SPMhRJlmBukPQuv3nIlAA", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1SPMhRJlmBukPQuv3nIlAA", track.getHttpURL());
        assertEquals("Give My Love To Rose", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(207253l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(19, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(19);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4vdV2Eua6RkUoUM51jdH56", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4vdV2Eua6RkUoUM51jdH56", track.getHttpURL());
        assertEquals("One", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(231200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(20, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(20);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:5VFzIzR8nACf9Bad4O73f6", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5VFzIzR8nACf9Bad4O73f6", track.getHttpURL());
        assertEquals("Hurt", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(218586l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(21, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("d00d9e7b82894fb8851a109c82568eb5", album.getMainArtist().getId());
        assertEquals("spotify:artist:6kACVPfCOnqzgfEF5ryl0x", album.getMainArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6kACVPfCOnqzgfEF5ryl0x", album.getMainArtist().getHttpURL());
        assertEquals("Johnny Cash", album.getMainArtist().getName());
        assertNull(album.getMainArtist().getPopularity());
        assertNull(album.getMainArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("4dc7cec0b8e441daaef85f46a915c7d4");
    new LoadAlbum(store, album).send(manager);
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:2mLIJwfgNPGjpuKaN7njPm", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/2mLIJwfgNPGjpuKaN7njPm", album.getHttpURL());
        assertEquals("Easy Tiger", album.getName());
        assertTrue(album.getCover().getId().matches(hex40));
        assertEquals("4dc7cec0b8e441daaef85f46a915c7d4", album.getId());
        assertNull(album.getPopularity());

        assertEquals(20, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:24oi2ZcdtjTFDsbVs3tjAb", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/24oi2ZcdtjTFDsbVs3tjAb", track.getHttpURL());
        assertEquals("Goodnight Rose", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(200120l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6iVTOPCmpABvG9jDZ2JozY", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6iVTOPCmpABvG9jDZ2JozY", track.getHttpURL());
        assertEquals("Two", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(158293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:0pl0QFnT982MoMkNOG7Jhu", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0pl0QFnT982MoMkNOG7Jhu", track.getHttpURL());
        assertEquals("Everybody Knows", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(145133l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6NGu6Ms3wByHSiJVYLgaS5", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6NGu6Ms3wByHSiJVYLgaS5", track.getHttpURL());
        assertEquals("Halloweenhead", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(203200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4AAo2jSIOpiXGVZpMNAbkf", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4AAo2jSIOpiXGVZpMNAbkf", track.getHttpURL());
        assertEquals("Oh My God, Whatever, Etc.", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(151960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6tWlfogLs4i7F3aC8nwYHJ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6tWlfogLs4i7F3aC8nwYHJ", track.getHttpURL());
        assertEquals("Tears Of Gold", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(173493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7JyO6CeRnQI4Fz3Lm70hms", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7JyO6CeRnQI4Fz3Lm70hms", track.getHttpURL());
        assertEquals("The Sun Also Sets", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(249846l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:0V9cJwuy5JCmKM9XDcTerk", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0V9cJwuy5JCmKM9XDcTerk", track.getHttpURL());
        assertEquals("Off Broadway", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(151386l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6wt6Z7lgWUKuTsooVRjR4H", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6wt6Z7lgWUKuTsooVRjR4H", track.getHttpURL());
        assertEquals("Pearls On A String", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(143586l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:783efX6wNRPhii3dGfdIDO", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/783efX6wNRPhii3dGfdIDO", track.getHttpURL());
        assertEquals("Rip Off", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(192720l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:62cWqXZyQIhGWfb2aeCyTx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/62cWqXZyQIhGWfb2aeCyTx", track.getHttpURL());
        assertEquals("Two Hearts", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(183213l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(11);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:743RjzEhzIsAFwYwPMyLeP", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/743RjzEhzIsAFwYwPMyLeP", track.getHttpURL());
        assertEquals("These Girls", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(170133l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(12, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(12);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4IGfz3eAl6eRNv1OMufxIY", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4IGfz3eAl6eRNv1OMufxIY", track.getHttpURL());
        assertEquals("I Taught Myself How To Grow Old", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(201146l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(13, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(13);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6b5TegeCtLqLac3iu8xMIs", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6b5TegeCtLqLac3iu8xMIs", track.getHttpURL());
        assertEquals("Follow The Lights", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(182720l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(14);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:08CIucCl4AK4m8xeAcBndp", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/08CIucCl4AK4m8xeAcBndp", track.getHttpURL());
        assertEquals("My Love For You Is Real", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(292440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(15);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:5yypaFOfcl5J7zqTtlg5kM", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5yypaFOfcl5J7zqTtlg5kM", track.getHttpURL());
        assertEquals("Blue Hotel", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(311333l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(16);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1gkUnDFauJz6XGMU1e7J5f", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1gkUnDFauJz6XGMU1e7J5f", track.getHttpURL());
        assertEquals("Down In A Hole", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(276680l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(17);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3dqufGyqCKtyXkcB1xxxeF", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3dqufGyqCKtyXkcB1xxxeF", track.getHttpURL());
        assertEquals("This Is It - Cardinals Version", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(212333l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(18);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6C5nfSccea6oLEryJ9N3Yr", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6C5nfSccea6oLEryJ9N3Yr", track.getHttpURL());
        assertEquals("If I Am A Stranger - Live in The Studio", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(283360l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(19);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:05rWobbKhM4Em2GKbWYQnt", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/05rWobbKhM4Em2GKbWYQnt", track.getHttpURL());
        assertEquals("Dear John - Live In The Studio", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(311786l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("4f9873e19e5a4b4096c216c98bcdb010", album.getMainArtist().getId());
        assertEquals("spotify:artist:2qc41rNTtdLK0tV3mJn2Pm", album.getMainArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/2qc41rNTtdLK0tV3mJn2Pm", album.getMainArtist().getHttpURL());
        assertEquals("Ryan Adams", album.getMainArtist().getName());
        assertNull(album.getMainArtist().getPopularity());
        assertNull(album.getMainArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("b8a09d31b4994b79a01f966b86cb9394");
    new LoadAlbum(store, album).send(manager);
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:5CnZjFfPDmxOX7KnWLLqpC", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/5CnZjFfPDmxOX7KnWLLqpC", album.getHttpURL());
        assertEquals("Protection", album.getName());
        assertTrue(album.getCover().getId().matches(hex40));

        assertEquals("b8a09d31b4994b79a01f966b86cb9394", album.getId());
        assertNull(album.getPopularity());

        assertEquals(10, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3kKVqFF4pv4EXeQe428zl2", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3kKVqFF4pv4EXeQe428zl2", track.getHttpURL());
        assertEquals("Protection", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(471560l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:46bTgJAQkrkNRxMuFnpRyD", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/46bTgJAQkrkNRxMuFnpRyD", track.getHttpURL());
        assertEquals("Karmacoma", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(316440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7FKhuZtIPchBVNIhFnNL5W", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7FKhuZtIPchBVNIhFnNL5W", track.getHttpURL());
        assertEquals("Three", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(229066l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4u5KFWcW1ptk0hJqp2lrCE", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4u5KFWcW1ptk0hJqp2lrCE", track.getHttpURL());
        assertEquals("Weather Storm", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(299960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6yzqdl5qv6BGu6BlRNWIDa", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6yzqdl5qv6BGu6BlRNWIDa", track.getHttpURL());
        assertEquals("Spying Glass", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(320973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4orPBWfQNUxs3RObirJwZR", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4orPBWfQNUxs3RObirJwZR", track.getHttpURL());
        assertEquals("Better Things", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(253933l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:5LrkJFmitzUrKZnt5iXl8A", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5LrkJFmitzUrKZnt5iXl8A", track.getHttpURL());
        assertEquals("Eurochild", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(311160l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7IdsIcmcti4v5VXkCJ3ou7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7IdsIcmcti4v5VXkCJ3ou7", track.getHttpURL());
        assertEquals("Sly", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(324640l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1sMr77QrXyS9LRIhcCI2el", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1sMr77QrXyS9LRIhcCI2el", track.getHttpURL());
        assertEquals("Heat Miser", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(219000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1JQLVgeRWU7Hf7y8EpmOqd", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1JQLVgeRWU7Hf7y8EpmOqd", track.getHttpURL());
        assertEquals("Light My Fire (live)", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(195493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("db614c7060fc47baa7be732d88ae446d", album.getMainArtist().getId());
        assertEquals("spotify:artist:6FXMGgJwohJLUSr5nVlf9X", album.getMainArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6FXMGgJwohJLUSr5nVlf9X", album.getMainArtist().getHttpURL());
        assertEquals("Massive Attack", album.getMainArtist().getName());
        assertNull(album.getMainArtist().getPopularity());
        assertNull(album.getMainArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("cce79af3bd864a799806a557877dda7a");
    new LoadAlbum(store, album).send(manager);
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:6eEhgZIrHftYRvgpAKJC2K", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/6eEhgZIrHftYRvgpAKJC2K", album.getHttpURL());
        assertEquals("Miles Davis And The Jazz Giants", album.getName());
        assertTrue(album.getCover().getId().matches(hex40));

        assertEquals("cce79af3bd864a799806a557877dda7a", album.getId());
        assertNull(album.getPopularity());

        assertEquals(9, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:5YfxBHo9eqlyNcECl6rnQT", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5YfxBHo9eqlyNcECl6rnQT", track.getHttpURL());
        assertEquals("Dig", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(452200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:05QlhAtZvmw8FJEGnY43Bw", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/05QlhAtZvmw8FJEGnY43Bw", track.getHttpURL());
        assertEquals("The Serpent's Tooth - Take 1", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(419293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:3qqKWUVfiLMrNPacFRzTzh", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3qqKWUVfiLMrNPacFRzTzh", track.getHttpURL());
        assertEquals("Four", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(240226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4ZFur3kCESWiJeypvgVZ34", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4ZFur3kCESWiJeypvgVZ34", track.getHttpURL());
        assertEquals("Walkin'", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(803533l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7jRHtpSkj3GPmqJGVsY1qT", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7jRHtpSkj3GPmqJGVsY1qT", track.getHttpURL());
        assertEquals("Doxy", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(291973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7amEjazqkOWSh71SAf3xZM", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7amEjazqkOWSh71SAf3xZM", track.getHttpURL());
        assertEquals("Bags' Groove - Take 1", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(672640l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1nKQ6jNqXLO6qXUk4Jv9LV", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1nKQ6jNqXLO6qXUk4Jv9LV", track.getHttpURL());
        assertEquals("A Gal In Calico", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(314533l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:59SYCaLBK41rjpoj268Eep", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/59SYCaLBK41rjpoj268Eep", track.getHttpURL());
        assertEquals("Minor March", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(494306l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:0LwxvuWvgRNQ2ABGN1goFU", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0LwxvuWvgRNQ2ABGN1goFU", track.getHttpURL());
        assertEquals("Vierd Blues", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(409093l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("f4d5d82d09124feda0633a2671f8c81a", album.getMainArtist().getId());
        assertEquals("spotify:artist:7rZR0ugcLEhNrFYOrUtZii", album.getMainArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7rZR0ugcLEhNrFYOrUtZii", album.getMainArtist().getHttpURL());
        assertEquals("Miles Davis", album.getMainArtist().getName());
        assertNull(album.getMainArtist().getPopularity());
        assertNull(album.getMainArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("792d90d6e5c14679afd00e7ea28982ce");
    new LoadAlbum(store, album).send(manager);
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:3GETv5yNXeM0cnhq8XahWu", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/3GETv5yNXeM0cnhq8XahWu", album.getHttpURL());
        assertEquals("No Shouts, No Calls", album.getName());
        assertTrue(album.getCover().getId().matches(hex40));

        assertEquals("792d90d6e5c14679afd00e7ea28982ce", album.getId());
        assertNull(album.getPopularity());

        assertEquals(11, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4yQT6yJR3OcL3PLKsCKXjx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4yQT6yJR3OcL3PLKsCKXjx", track.getHttpURL());
        assertEquals("The Greater Times", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(222000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6N3v6P4QFiD6v8Xc1WZdD7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6N3v6P4QFiD6v8Xc1WZdD7", track.getHttpURL());
        assertEquals("To the East", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(294000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:4goRw3CP6Ke3aLZslgXbpP", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4goRw3CP6Ke3aLZslgXbpP", track.getHttpURL());
        assertEquals("After the Call", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(184000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:1RA0kfuQBrmcmuPPo0x0w7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1RA0kfuQBrmcmuPPo0x0w7", track.getHttpURL());
        assertEquals("Tram 21", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(269000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:2qOK4xkpjWsXDM65VMpe56", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2qOK4xkpjWsXDM65VMpe56", track.getHttpURL());
        assertEquals("In Berlin", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(254000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:42yUeFI77nh5TV3M8SGV3T", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/42yUeFI77nh5TV3M8SGV3T", track.getHttpURL());
        assertEquals("At Sea", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(287000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:34vSGKwatlJFBtJuRDkHo7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/34vSGKwatlJFBtJuRDkHo7", track.getHttpURL());
        assertEquals("Between the Wolf and the Dog", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(245000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:7yxdkHY6hSUDVJnNhJ8boj", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7yxdkHY6hSUDVJnNhJ8boj", track.getHttpURL());
        assertEquals("Saturday", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(235000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:2dtvgPd3vsotKXtGk4dWlg", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2dtvgPd3vsotKXtGk4dWlg", track.getHttpURL());
        assertEquals("Five", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(385000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:6LVzKLdwFRbCJKAsEA0pKz", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6LVzKLdwFRbCJKAsEA0pKz", track.getHttpURL());
        assertEquals("Cut and Run", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(207000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertTrue(track.getId().matches("[a-fA-F0-9]{32}"));
        assertEquals("spotify:track:70RPNhD2lWbwDZZPWCTWtd", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/70RPNhD2lWbwDZZPWCTWtd", track.getHttpURL());
        assertEquals("The Lighthouse", track.getTitle());
        assertNull(track.getCover());
        assertTrue(track.getFiles().size() > 0);
        assertTrue(track.getFiles().get(0).getId().matches(hex40));

        assertEquals(262000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("f6150726a8e94c89a7cf336d3f72be9c", album.getMainArtist().getId());
        assertEquals("spotify:artist:7ulIMfVKiXh8ecEpAVHIAY", album.getMainArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7ulIMfVKiXh8ecEpAVHIAY", album.getMainArtist().getHttpURL());
        assertEquals("Electrelane", album.getMainArtist().getName());
        assertNull(album.getMainArtist().getPopularity());
        assertNull(album.getMainArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });


    // end generated tests
  }


}
