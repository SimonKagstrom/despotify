package se.despotify.client.protocol.command.media;

import org.junit.Test;
import se.despotify.DespotifyClientTest;
import se.despotify.util.SpotifyURI;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.VisitorAdapter;
import se.despotify.domain.media.Track;

/**
 * @since 2009-apr-25 18:28:29
 */
public class TestLoadAlbum extends DespotifyClientTest {

  @Test
  public void test() throws Exception {

// test generation>
//
//    for (Album album : defaultAlbums) {
//      manager.send(new LoadAlbum(store, album));
//    }
//
//    for (Album album : defaultAlbums) {
//      MediaTestCaseGenerator.createEqualsTest(album, "album = store.getAlbum(\""+album.getHexUUID()+"\");\n" +
//          "    manager.send(new LoadAlbum(store, album));\n" +
//          "    album");
//    }
//
// < test generation
    
    Album album;

    // generated tests follows

    album = store.getAlbum(SpotifyURI.toHex("1rKQfglNABmKc8K0Kmfvdz"));
    manager.send(new LoadAlbum(store, album));

    album = store.getAlbum("02f8df4ad52d449caca8c6a25d2eca08");
    manager.send(new LoadAlbum(store, album));
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:05BIC4TZptbiQoF03QhojS", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/05BIC4TZptbiQoF03QhojS", album.getHttpURL());
        assertEquals("The Legend Of Johnny Cash", album.getName());
        assertEquals("3d44ff9d9b75a44b835f7e719a193e9033cae6af", album.getCover());
        assertEquals("02f8df4ad52d449caca8c6a25d2eca08", album.getId());
        assertNull(album.getPopularity());

        assertEquals(21, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertEquals("78fa2ba4d4124b51a708dd074c62d24c", track.getId());
        assertEquals("spotify:track:3GhpgjhCNZZa6Lb7Wtrp3S", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3GhpgjhCNZZa6Lb7Wtrp3S", track.getHttpURL());
        assertEquals("Ring Of Fire", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("43cc97fb01631fc466ca5b731a1ac0df9000f585", track.getFiles().get(0));
        assertEquals(155866l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertEquals("728f02c7fc5d4a1c83ae91a88b455d6c", track.getId());
        assertEquals("spotify:track:3uatQAOZ90mnrixwhKiSAI", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3uatQAOZ90mnrixwhKiSAI", track.getHttpURL());
        assertEquals("I Walk The Line", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("35fd6527101e034b5cf540862036da3485ddde54", track.getFiles().get(0));
        assertEquals(164440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertEquals("8bd3f688cf9f4c96ad3926eda6ebf87a", track.getId());
        assertEquals("spotify:track:4fQNEmRzjhgmPe5TkL6WNQ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4fQNEmRzjhgmPe5TkL6WNQ", track.getHttpURL());
        assertEquals("Jackson", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("f728aef36d171335f32c4231514cae159d5c57e2", track.getFiles().get(0));
        assertEquals(165973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertEquals("3515cd3fba3249ca83f746da0a2e1faa", track.getId());
        assertEquals("spotify:track:1Caywbok6SQb7czVXzi2ro", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1Caywbok6SQb7czVXzi2ro", track.getHttpURL());
        assertEquals("Folsom Prison Blues", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("ac8062a849d8c599da4d32a21ffe02c6968148e9", track.getFiles().get(0));
        assertEquals(169506l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertEquals("8441c41dbfda4bb1b66f2b68ac330447", track.getId());
        assertEquals("spotify:track:41z2CLPSwQVA0hVNvu28ej", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/41z2CLPSwQVA0hVNvu28ej", track.getHttpURL());
        assertEquals("A Boy Named Sue", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("0f1abf8152fe6c663f2d63a0787aeb428e82941c", track.getFiles().get(0));
        assertEquals(225466l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertEquals("f07783cebc8f4bafa2fce8c6d0cc1deb", track.getId());
        assertEquals("spotify:track:7jKMavlOi9N4ZZG6755hk7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7jKMavlOi9N4ZZG6755hk7", track.getHttpURL());
        assertEquals("Big River", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("ed4f20006951a88d904541a4a1d3f7f234b40d4e", track.getFiles().get(0));
        assertEquals(151293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertEquals("ddd9350f4c2246e19de7b203d5bb1aff", track.getId());
        assertEquals("spotify:track:6KCzeoC39w23DZ3tZ88mKz", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6KCzeoC39w23DZ3tZ88mKz", track.getHttpURL());
        assertEquals("Get Rhythm", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("9bc4d33943875d1cec863d8141a0503ee2897d32", track.getFiles().get(0));
        assertEquals(133240l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertEquals("2e5144d2fbb14cf9889410dde3f62f63", track.getId());
        assertEquals("spotify:track:1poMNfOIDOIxltyUYjV6qn", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1poMNfOIDOIxltyUYjV6qn", track.getHttpURL());
        assertEquals("Cry! Cry! Cry!", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("68da7e85a3adf4afaf53bbc8152c592835f39658", track.getFiles().get(0));
        assertEquals(144226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertEquals("da1b14a8928d4f19b0758e49076d1451", track.getId());
        assertEquals("spotify:track:6DyHB5mWMzUfP3QEXwwVUZ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6DyHB5mWMzUfP3QEXwwVUZ", track.getHttpURL());
        assertEquals("Hey Porter", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("284a66a37e42afe216d76f59688715b9a594b333", track.getFiles().get(0));
        assertEquals(132906l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertEquals("e376c1392b8f42539a0668445657492b", track.getId());
        assertEquals("spotify:track:6VdwOFKf0u6hLVtXcL8CWn", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6VdwOFKf0u6hLVtXcL8CWn", track.getHttpURL());
        assertEquals("A Thing Called Love - Original Version", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("b0fc17ab0b7f5b2d052dbc44b6dc4549dfb13d76", track.getFiles().get(0));
        assertEquals(151360l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertEquals("d7ac88008f5a4924969941721d6ebec7", track.getId());
        assertEquals("spotify:track:6yYcerWiS6tebVzoIpu74j", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6yYcerWiS6tebVzoIpu74j", track.getHttpURL());
        assertEquals("Guess Things Happen That Way - 1958 Single Version", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("dac9f53ea0a5ccf3a4390c9faa294f21fa276d8c", track.getFiles().get(0));
        assertEquals(110000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(11);
        assertEquals("730da679d40443e28b64b4ab44e7b554", track.getId());
        assertEquals("spotify:track:3v6m3ICCXoMzZw3EMr5Kn2", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3v6m3ICCXoMzZw3EMr5Kn2", track.getHttpURL());
        assertEquals("San Quentin - Live Version", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("fd56992a6ff2eefccfaea039634e30b3571be695", track.getFiles().get(0));
        assertEquals(185880l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(12, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(12);
        assertEquals("75ca03b768d040a3b6b9197faeaa5bfa", track.getId());
        assertEquals("spotify:track:3AgqeAt9qzUdSBor27IRn4", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3AgqeAt9qzUdSBor27IRn4", track.getHttpURL());
        assertEquals("Man In Black - Single Version", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("57f3bee4cf7ad7e16fc5bd174366e2b86eb7a6ca", track.getFiles().get(0));
        assertEquals(172226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(13, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(13);
        assertEquals("e6d3c0171555466496dc31180ebcd98d", track.getId());
        assertEquals("spotify:track:71z0kIckftTVfBSG0HBHAx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/71z0kIckftTVfBSG0HBHAx", track.getHttpURL());
        assertEquals("Highwayman", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("f80a8cebffcba8a5b2ed816e17b632c3d6eb6da7", track.getFiles().get(0));
        assertEquals(183426l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(14, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(14);
        assertEquals("84630de2b70044938abadc2b5b004717", track.getId());
        assertEquals("spotify:track:41OfOvVyuD5x185BTOW0ET", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/41OfOvVyuD5x185BTOW0ET", track.getHttpURL());
        assertEquals("The Wanderer", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("1b723c9d911479688274f8157c01675814561ebd", track.getFiles().get(0));
        assertEquals(284173l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(15, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(15);
        assertEquals("d73b8e4253f2474793691045569a2002", track.getId());
        assertEquals("spotify:track:6y8zaPjL3eHZW3UAuL2VJo", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6y8zaPjL3eHZW3UAuL2VJo", track.getHttpURL());
        assertEquals("I've Been Everywhere", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("8bfaf0989ee7eb4b5036574b6acbe5e57435b75c", track.getFiles().get(0));
        assertEquals(196386l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(16, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(16);
        assertEquals("87e27d275915442ba1d3d09053ad6696", track.getId());
        assertEquals("spotify:track:48pt86Kh5s2cuhkJLy1Pue", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/48pt86Kh5s2cuhkJLy1Pue", track.getHttpURL());
        assertEquals("Rusty Cage", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("ca9302252216f7ff90701a15af9d5b4355e00484", track.getFiles().get(0));
        assertEquals(169493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(17, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(17);
        assertEquals("565e4c91f4ae42a39aa5646d325590e3", track.getId());
        assertEquals("spotify:track:2CYqXSe4pzCqpSWZvqgE39", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2CYqXSe4pzCqpSWZvqgE39", track.getHttpURL());
        assertEquals("Personal Jesus", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("cd0b782fc82ff27768ba1299d8e305dfbbadc4da", track.getFiles().get(0));
        assertEquals(198960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(18, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(18);
        assertEquals("3deaacde160b4df9aa2a5ddac44dd2a0", track.getId());
        assertEquals("spotify:track:1SPMhRJlmBukPQuv3nIlAA", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1SPMhRJlmBukPQuv3nIlAA", track.getHttpURL());
        assertEquals("Give My Love To Rose", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("b12571e85d6162ccdd0307bcd452ca488e97bd95", track.getFiles().get(0));
        assertEquals(207253l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(19, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(19);
        assertEquals("93f98ea75b4748f797668485a3d01bd0", track.getId());
        assertEquals("spotify:track:4vdV2Eua6RkUoUM51jdH56", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4vdV2Eua6RkUoUM51jdH56", track.getHttpURL());
        assertEquals("One", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("36fa172ca1a707ba71e79757f3014cc1a26fbbf6", track.getFiles().get(0));
        assertEquals(231200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(20, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(20);
        assertEquals("c2d8c567f4db4adc83120d132657955c", track.getId());
        assertEquals("spotify:track:5VFzIzR8nACf9Bad4O73f6", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5VFzIzR8nACf9Bad4O73f6", track.getHttpURL());
        assertEquals("Hurt", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("eac61cfb6c40feef3b55f503b655716575898cfb", track.getFiles().get(0));
        assertEquals(218586l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(21, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("d00d9e7b82894fb8851a109c82568eb5", album.getArtist().getId());
        assertEquals("spotify:artist:6kACVPfCOnqzgfEF5ryl0x", album.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6kACVPfCOnqzgfEF5ryl0x", album.getArtist().getHttpURL());
        assertEquals("Johnny Cash", album.getArtist().getName());
        assertNull(album.getArtist().getPopularity());
        assertNull(album.getArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("4dc7cec0b8e441daaef85f46a915c7d4");
    manager.send(new LoadAlbum(store, album));
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:2mLIJwfgNPGjpuKaN7njPm", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/2mLIJwfgNPGjpuKaN7njPm", album.getHttpURL());
        assertEquals("Easy Tiger", album.getName());
        assertEquals("95cdf93b035a5c568bc19981a9e618bd2f9a50bf", album.getCover());
        assertEquals("4dc7cec0b8e441daaef85f46a915c7d4", album.getId());
        assertNull(album.getPopularity());

        assertEquals(20, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertEquals("440a880a046e40eb905796a2720aa397", track.getId());
        assertEquals("spotify:track:24oi2ZcdtjTFDsbVs3tjAb", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/24oi2ZcdtjTFDsbVs3tjAb", track.getHttpURL());
        assertEquals("Goodnight Rose", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("67fe1c1550d05ad824ea287d95df6a81d2a80e23", track.getFiles().get(0));
        assertEquals(200120l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertEquals("cf2cd530980e450d855977ba0a80ec6e", track.getId());
        assertEquals("spotify:track:6iVTOPCmpABvG9jDZ2JozY", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6iVTOPCmpABvG9jDZ2JozY", track.getHttpURL());
        assertEquals("Two", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("ddc30e1090b43403cc0828db91300450a358a1a8", track.getFiles().get(0));
        assertEquals(158293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertEquals("0d6da839f28d43db8340cf00ff259358", track.getId());
        assertEquals("spotify:track:0pl0QFnT982MoMkNOG7Jhu", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0pl0QFnT982MoMkNOG7Jhu", track.getHttpURL());
        assertEquals("Everybody Knows", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("85864026a4928bea53d5619d56fd8cb1a09a9c15", track.getFiles().get(0));
        assertEquals(145133l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertEquals("df78c7bcd413493996b85cea57114ff1", track.getId());
        assertEquals("spotify:track:6NGu6Ms3wByHSiJVYLgaS5", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6NGu6Ms3wByHSiJVYLgaS5", track.getHttpURL());
        assertEquals("Halloweenhead", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("a59e6582e0c7fc183e529b72d7ffc698462949d4", track.getFiles().get(0));
        assertEquals(203200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertEquals("96d10e3865e148ecb45c8c7cd85942c3", track.getId());
        assertEquals("spotify:track:4AAo2jSIOpiXGVZpMNAbkf", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4AAo2jSIOpiXGVZpMNAbkf", track.getHttpURL());
        assertEquals("Oh My God, Whatever, Etc.", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("0dc5eca40a3942ab4028bbd7c20652d9fd9f7570", track.getFiles().get(0));
        assertEquals(151960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertEquals("d502235382984bc58fb138edf1cf9877", track.getId());
        assertEquals("spotify:track:6tWlfogLs4i7F3aC8nwYHJ", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6tWlfogLs4i7F3aC8nwYHJ", track.getHttpURL());
        assertEquals("Tears Of Gold", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("b143f8621039bf5befb78a3eb5c2b41bac143ba7", track.getFiles().get(0));
        assertEquals(173493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertEquals("fe24aae4b84c4f81a6cff8e8cb76eda4", track.getId());
        assertEquals("spotify:track:7JyO6CeRnQI4Fz3Lm70hms", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7JyO6CeRnQI4Fz3Lm70hms", track.getHttpURL());
        assertEquals("The Sun Also Sets", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("df5191824c5a7786321a009fabe2a8f31f8504c1", track.getFiles().get(0));
        assertEquals(249846l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertEquals("1e4929b9f87545ba97f83a07827e9a3e", track.getId());
        assertEquals("spotify:track:0V9cJwuy5JCmKM9XDcTerk", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0V9cJwuy5JCmKM9XDcTerk", track.getHttpURL());
        assertEquals("Off Broadway", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("8e353cfc7d198ced7c63f2163e34d470be6b9d59", track.getFiles().get(0));
        assertEquals(151386l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertEquals("d6592db3a888473c912ee572f4a8ad8f", track.getId());
        assertEquals("spotify:track:6wt6Z7lgWUKuTsooVRjR4H", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6wt6Z7lgWUKuTsooVRjR4H", track.getHttpURL());
        assertEquals("Pearls On A String", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("39ac18a703aadbcc4fe39e30e89caa05963f91c7", track.getFiles().get(0));
        assertEquals(143586l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertEquals("ea43e3a144e7450c8fc5f22758595c5c", track.getId());
        assertEquals("spotify:track:783efX6wNRPhii3dGfdIDO", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/783efX6wNRPhii3dGfdIDO", track.getHttpURL());
        assertEquals("Rip Off", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("e8b57d158a1010b4e1146753bf92fe6ea1902504", track.getFiles().get(0));
        assertEquals(192720l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertEquals("c64fc8e8b9fa4e1c95dcc98558857a6b", track.getId());
        assertEquals("spotify:track:62cWqXZyQIhGWfb2aeCyTx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/62cWqXZyQIhGWfb2aeCyTx", track.getHttpURL());
        assertEquals("Two Hearts", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("8660c3e83fa88e396e9130c1d396d238c13d08de", track.getFiles().get(0));
        assertEquals(183213l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(11);
        assertEquals("e826997fe8b443d5aff0c4e5e9f56ae3", track.getId());
        assertEquals("spotify:track:743RjzEhzIsAFwYwPMyLeP", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/743RjzEhzIsAFwYwPMyLeP", track.getHttpURL());
        assertEquals("These Girls", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("973a4c7e9c96163fbfa531ada9e808bb551dd515", track.getFiles().get(0));
        assertEquals(170133l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(12, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(12);
        assertEquals("9b1b38c422a14039973ddbbc51108fd0", track.getId());
        assertEquals("spotify:track:4IGfz3eAl6eRNv1OMufxIY", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4IGfz3eAl6eRNv1OMufxIY", track.getHttpURL());
        assertEquals("I Taught Myself How To Grow Old", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("c7d569bce432ef91715352f78b63226a40e8c0c0", track.getFiles().get(0));
        assertEquals(201146l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(13, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(13);
        assertEquals("cb055b8b790e4b3480e3ebf4c3c4193c", track.getId());
        assertEquals("spotify:track:6b5TegeCtLqLac3iu8xMIs", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6b5TegeCtLqLac3iu8xMIs", track.getHttpURL());
        assertEquals("Follow The Lights", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("285accdf1bf22b607221bd20228aad7d416a5f67", track.getFiles().get(0));
        assertEquals(182720l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(14);
        assertEquals("04920e964b65440ab0255c8c540e15b3", track.getId());
        assertEquals("spotify:track:08CIucCl4AK4m8xeAcBndp", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/08CIucCl4AK4m8xeAcBndp", track.getHttpURL());
        assertEquals("My Love For You Is Real", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("69e34e4175379a1657c171692c3800d5d6d1d357", track.getFiles().get(0));
        assertEquals(292440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(15);
        assertEquals("b698bd814767422390d66b72a0bf9b0c", track.getId());
        assertEquals("spotify:track:5yypaFOfcl5J7zqTtlg5kM", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5yypaFOfcl5J7zqTtlg5kM", track.getHttpURL());
        assertEquals("Blue Hotel", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("681a0eaf7f6b16d9f4e9a0db2f1b889bae7d3d22", track.getFiles().get(0));
        assertEquals(311333l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(16);
        assertEquals("2983c7def4134f3aa591189199b0b881", track.getId());
        assertEquals("spotify:track:1gkUnDFauJz6XGMU1e7J5f", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1gkUnDFauJz6XGMU1e7J5f", track.getHttpURL());
        assertEquals("Down In A Hole", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("15c86c65b7dd1c8578eaadc9f3b487df0ef9a3ac", track.getFiles().get(0));
        assertEquals(276680l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(17);
        assertEquals("69afb25b304344b8a81644b31b3b7439", track.getId());
        assertEquals("spotify:track:3dqufGyqCKtyXkcB1xxxeF", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3dqufGyqCKtyXkcB1xxxeF", track.getHttpURL());
        assertEquals("This Is It - Cardinals Version", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("f83d63a37044e39ed7fd6b674045a7f50fbae57f", track.getFiles().get(0));
        assertEquals(212333l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(18);
        assertEquals("d9533d14214d4c99a5bc2ac2d6332917", track.getId());
        assertEquals("spotify:track:6C5nfSccea6oLEryJ9N3Yr", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6C5nfSccea6oLEryJ9N3Yr", track.getHttpURL());
        assertEquals("If I Am A Stranger - Live in The Studio", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("1ddd62f6581727cd6ac726ce71246cc5a5461947", track.getFiles().get(0));
        assertEquals(283360l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(19);
        assertEquals("02e37a05d72a49b4bc17e4f9b30dbf5f", track.getId());
        assertEquals("spotify:track:05rWobbKhM4Em2GKbWYQnt", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/05rWobbKhM4Em2GKbWYQnt", track.getHttpURL());
        assertEquals("Dear John - Live In The Studio", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("d8fb9426e785bcdb224dc395ea211f1a57ba70c0", track.getFiles().get(0));
        assertEquals(311786l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("4f9873e19e5a4b4096c216c98bcdb010", album.getArtist().getId());
        assertEquals("spotify:artist:2qc41rNTtdLK0tV3mJn2Pm", album.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/2qc41rNTtdLK0tV3mJn2Pm", album.getArtist().getHttpURL());
        assertEquals("Ryan Adams", album.getArtist().getName());
        assertNull(album.getArtist().getPopularity());
        assertNull(album.getArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("b8a09d31b4994b79a01f966b86cb9394");
    manager.send(new LoadAlbum(store, album));
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:5CnZjFfPDmxOX7KnWLLqpC", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/5CnZjFfPDmxOX7KnWLLqpC", album.getHttpURL());
        assertEquals("Protection", album.getName());
        assertEquals("1946f99c0408e958f88c1811f1d9d92df0a4b16f", album.getCover());
        assertEquals("b8a09d31b4994b79a01f966b86cb9394", album.getId());
        assertNull(album.getPopularity());

        assertEquals(10, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertEquals("6d9216aaeaf648f68bb4cc90d0579b84", track.getId());
        assertEquals("spotify:track:3kKVqFF4pv4EXeQe428zl2", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3kKVqFF4pv4EXeQe428zl2", track.getHttpURL());
        assertEquals("Protection", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("78259642401b554e34dd9e673a7a7aed24900a91", track.getFiles().get(0));
        assertEquals(471560l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertEquals("86b571617c24410194bb3450f837a13f", track.getId());
        assertEquals("spotify:track:46bTgJAQkrkNRxMuFnpRyD", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/46bTgJAQkrkNRxMuFnpRyD", track.getHttpURL());
        assertEquals("Karmacoma", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("6f73a839b022a2fb5a29cf7471019a4941acd7b4", track.getFiles().get(0));
        assertEquals(316440l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertEquals("fc1f1b5860f04a739971fcad9c1cd634", track.getId());
        assertEquals("spotify:track:7FKhuZtIPchBVNIhFnNL5W", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7FKhuZtIPchBVNIhFnNL5W", track.getHttpURL());
        assertEquals("Three", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("6186903f7ffa0e579954fa42c537cebc06ac7427", track.getFiles().get(0));
        assertEquals(229066l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertEquals("936004d9503e408ca8a3aab8c3a00720", track.getId());
        assertEquals("spotify:track:4u5KFWcW1ptk0hJqp2lrCE", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4u5KFWcW1ptk0hJqp2lrCE", track.getHttpURL());
        assertEquals("Weather Storm", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("19809f3c4c232cd31fcc85a4e6043f72e2f6b99a", track.getFiles().get(0));
        assertEquals(299960l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertEquals("d77651fe713946f6aecd0bf4b8271f8c", track.getId());
        assertEquals("spotify:track:6yzqdl5qv6BGu6BlRNWIDa", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6yzqdl5qv6BGu6BlRNWIDa", track.getHttpURL());
        assertEquals("Spying Glass", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("6f002a020807c0823006554141b49bb25b3eabd2", track.getFiles().get(0));
        assertEquals(320973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertEquals("906254b85ef340d6a82e99a4f9649543", track.getId());
        assertEquals("spotify:track:4orPBWfQNUxs3RObirJwZR", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4orPBWfQNUxs3RObirJwZR", track.getHttpURL());
        assertEquals("Better Things", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("41a7a6f835153635f43453a6e800d7e880bee1d3", track.getFiles().get(0));
        assertEquals(253933l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertEquals("bd6cf00bfdac407382ed9fe31c0059d0", track.getId());
        assertEquals("spotify:track:5LrkJFmitzUrKZnt5iXl8A", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5LrkJFmitzUrKZnt5iXl8A", track.getHttpURL());
        assertEquals("Eurochild", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("0c89b1b0fa8cc91ec0cfd37feb6bdc51c0ee1d73", track.getFiles().get(0));
        assertEquals(311160l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertEquals("fd6e4b330b3e45339bc733a6e010a923", track.getId());
        assertEquals("spotify:track:7IdsIcmcti4v5VXkCJ3ou7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7IdsIcmcti4v5VXkCJ3ou7", track.getHttpURL());
        assertEquals("Sly", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("8b40074a76578611c7fdff75be5034e194f50aca", track.getFiles().get(0));
        assertEquals(324640l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertEquals("301c0561a43044a7bd7fc3bcd4546e41", track.getId());
        assertEquals("spotify:track:1sMr77QrXyS9LRIhcCI2el", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1sMr77QrXyS9LRIhcCI2el", track.getHttpURL());
        assertEquals("Heat Miser", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("2a039bdba4b8e6b3a22c3dfbfbfcee02fe884fdd", track.getFiles().get(0));
        assertEquals(219000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertEquals("3927d8f0a4d34133b4babc44bfb9b901", track.getId());
        assertEquals("spotify:track:1JQLVgeRWU7Hf7y8EpmOqd", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1JQLVgeRWU7Hf7y8EpmOqd", track.getHttpURL());
        assertEquals("Light My Fire (live)", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("17fccc8bfc3e756a26f6b053f588444992eca7db", track.getFiles().get(0));
        assertEquals(195493l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("db614c7060fc47baa7be732d88ae446d", album.getArtist().getId());
        assertEquals("spotify:artist:6FXMGgJwohJLUSr5nVlf9X", album.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/6FXMGgJwohJLUSr5nVlf9X", album.getArtist().getHttpURL());
        assertEquals("Massive Attack", album.getArtist().getName());
        assertNull(album.getArtist().getPopularity());
        assertNull(album.getArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("cce79af3bd864a799806a557877dda7a");
    manager.send(new LoadAlbum(store, album));
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:6eEhgZIrHftYRvgpAKJC2K", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/6eEhgZIrHftYRvgpAKJC2K", album.getHttpURL());
        assertEquals("Miles Davis And The Jazz Giants", album.getName());
        assertEquals("2df174a97a577044edaa482b5eb198b5bc20dddb", album.getCover());
        assertEquals("cce79af3bd864a799806a557877dda7a", album.getId());
        assertNull(album.getPopularity());

        assertEquals(9, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertEquals("c436ce17825947128b2dc42afc534793", track.getId());
        assertEquals("spotify:track:5YfxBHo9eqlyNcECl6rnQT", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/5YfxBHo9eqlyNcECl6rnQT", track.getHttpURL());
        assertEquals("Dig", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("23277e10ca261b017f4a0adde8938f6d0eecac30", track.getFiles().get(0));
        assertEquals(452200l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertEquals("0318df1a31e6424ea2007325113bc062", track.getId());
        assertEquals("spotify:track:05QlhAtZvmw8FJEGnY43Bw", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/05QlhAtZvmw8FJEGnY43Bw", track.getHttpURL());
        assertEquals("The Serpent's Tooth - Take 1", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("a0e47c33e7b63c3d40fce7122efacb6ca16187b9", track.getFiles().get(0));
        assertEquals(419293l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertEquals("7093f50c9ecf428eb780348c076f9f7f", track.getId());
        assertEquals("spotify:track:3qqKWUVfiLMrNPacFRzTzh", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/3qqKWUVfiLMrNPacFRzTzh", track.getHttpURL());
        assertEquals("Four", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("55473589cbccde7f9621fea833f38487f92f2892", track.getFiles().get(0));
        assertEquals(240226l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertEquals("a41be5cf75194e58bfdadbae72b3384a", track.getId());
        assertEquals("spotify:track:4ZFur3kCESWiJeypvgVZ34", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4ZFur3kCESWiJeypvgVZ34", track.getHttpURL());
        assertEquals("Walkin'", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("2214424d7d9ab590a8620aca5b73cdef9b9b1c33", track.getFiles().get(0));
        assertEquals(803533l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertEquals("f086aa9a46e6455eb7455bc63a6f5447", track.getId());
        assertEquals("spotify:track:7jRHtpSkj3GPmqJGVsY1qT", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7jRHtpSkj3GPmqJGVsY1qT", track.getHttpURL());
        assertEquals("Doxy", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("2173a854ccb7a6ce1285e6e21a1534d1e509eb52", track.getFiles().get(0));
        assertEquals(291973l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertEquals("eb7db7d1b62a443986544a4e9e7fe5d2", track.getId());
        assertEquals("spotify:track:7amEjazqkOWSh71SAf3xZM", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7amEjazqkOWSh71SAf3xZM", track.getHttpURL());
        assertEquals("Bags' Groove - Take 1", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("9c2a888f691e9869c35d34f95ddd5bf13e90df74", track.getFiles().get(0));
        assertEquals(672640l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertEquals("2d723102f82f42ffbf7c80363d0b0117", track.getId());
        assertEquals("spotify:track:1nKQ6jNqXLO6qXUk4Jv9LV", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1nKQ6jNqXLO6qXUk4Jv9LV", track.getHttpURL());
        assertEquals("A Gal In Calico", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("6a1de993962d08b9488ca3cdc36c3d56105790e2", track.getFiles().get(0));
        assertEquals(314533l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertEquals("a986144d154c4685bd24ae7950baabbd", track.getId());
        assertEquals("spotify:track:59SYCaLBK41rjpoj268Eep", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/59SYCaLBK41rjpoj268Eep", track.getHttpURL());
        assertEquals("Minor March", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("98473913e3b9427f5bff8c22b7b14233c07dc076", track.getFiles().get(0));
        assertEquals(494306l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertEquals("192f8dd889c64cab9058d81b336727f6", track.getId());
        assertEquals("spotify:track:0LwxvuWvgRNQ2ABGN1goFU", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/0LwxvuWvgRNQ2ABGN1goFU", track.getHttpURL());
        assertEquals("Vierd Blues", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("2c0e58c233807a1ec0a0753fb76562465a486966", track.getFiles().get(0));
        assertEquals(409093l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("f4d5d82d09124feda0633a2671f8c81a", album.getArtist().getId());
        assertEquals("spotify:artist:7rZR0ugcLEhNrFYOrUtZii", album.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7rZR0ugcLEhNrFYOrUtZii", album.getArtist().getHttpURL());
        assertEquals("Miles Davis", album.getArtist().getName());
        assertNull(album.getArtist().getPopularity());
        assertNull(album.getArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });
    album = store.getAlbum("792d90d6e5c14679afd00e7ea28982ce");
    manager.send(new LoadAlbum(store, album));
    album.accept(new VisitorAdapter() {
      @Override
      public void visit(Album album) {
        assertEquals("spotify:album:3GETv5yNXeM0cnhq8XahWu", album.getSpotifyURI());
        assertEquals("http://open.spotify.com/album/3GETv5yNXeM0cnhq8XahWu", album.getHttpURL());
        assertEquals("No Shouts, No Calls", album.getName());
        assertEquals("e1712082598617226c51214314392d7d19f989d3", album.getCover());
        assertEquals("792d90d6e5c14679afd00e7ea28982ce", album.getId());
        assertNull(album.getPopularity());

        assertEquals(11, album.getTracks().size());
        Track track;

        track = album.getTracks().get(0);
        assertEquals("95e5d43024a3426788fc2b1cddc56f17", track.getId());
        assertEquals("spotify:track:4yQT6yJR3OcL3PLKsCKXjx", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4yQT6yJR3OcL3PLKsCKXjx", track.getHttpURL());
        assertEquals("The Greater Times", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("5d5fa6f55783a2d868dce019e72a9208980a5105", track.getFiles().get(0));
        assertEquals(222000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(1, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(1);
        assertEquals("df237a0344a044c3b2241b02b91dc8c5", track.getId());
        assertEquals("spotify:track:6N3v6P4QFiD6v8Xc1WZdD7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6N3v6P4QFiD6v8Xc1WZdD7", track.getHttpURL());
        assertEquals("To the East", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("3497561ff94003bd5ec365785eeeb3421a46d0e5", track.getFiles().get(0));
        assertEquals(294000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(2, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(2);
        assertEquals("8c1e7f5cbbc14462bce956e0a09bcc75", track.getId());
        assertEquals("spotify:track:4goRw3CP6Ke3aLZslgXbpP", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/4goRw3CP6Ke3aLZslgXbpP", track.getHttpURL());
        assertEquals("After the Call", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("7463a41b0938a9c28bc165c0e6a989eec8a57342", track.getFiles().get(0));
        assertEquals(184000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(3, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(3);
        assertEquals("3d407e2c693244d88bd6fb9681e49bff", track.getId());
        assertEquals("spotify:track:1RA0kfuQBrmcmuPPo0x0w7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/1RA0kfuQBrmcmuPPo0x0w7", track.getHttpURL());
        assertEquals("Tram 21", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("16f85ab705c7eaa51a74cdc74d508047199e937b", track.getFiles().get(0));
        assertEquals(269000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(4, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(4);
        assertEquals("4fed16657cc04aa69f69b9deca1989cc", track.getId());
        assertEquals("spotify:track:2qOK4xkpjWsXDM65VMpe56", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2qOK4xkpjWsXDM65VMpe56", track.getHttpURL());
        assertEquals("In Berlin", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("5f39ede3d1f63685d421b23930d8a8881d3a8eb8", track.getFiles().get(0));
        assertEquals(254000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(5, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(5);
        assertEquals("84c923148ab34771a42b8c41968fc965", track.getId());
        assertEquals("spotify:track:42yUeFI77nh5TV3M8SGV3T", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/42yUeFI77nh5TV3M8SGV3T", track.getHttpURL());
        assertEquals("At Sea", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("e5b7d93ad8431fd54e73208d786560248d1966db", track.getFiles().get(0));
        assertEquals(287000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(6, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(6);
        assertEquals("64f67f33b9824986b9b5e03abc5b2eb3", track.getId());
        assertEquals("spotify:track:34vSGKwatlJFBtJuRDkHo7", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/34vSGKwatlJFBtJuRDkHo7", track.getHttpURL());
        assertEquals("Between the Wolf and the Dog", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("cb851ff1c7c971a247dd6150c1de129e60fcd4fb", track.getFiles().get(0));
        assertEquals(245000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(7, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(7);
        assertEquals("f84cd8282f984e6c8233c4f58f74a73f", track.getId());
        assertEquals("spotify:track:7yxdkHY6hSUDVJnNhJ8boj", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/7yxdkHY6hSUDVJnNhJ8boj", track.getHttpURL());
        assertEquals("Saturday", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("d8da754dcef45ceac89e7c447dd3ae71d23779f1", track.getFiles().get(0));
        assertEquals(235000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(8, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(8);
        assertEquals("48daf12f96f84793a526b579aa4d1f66", track.getId());
        assertEquals("spotify:track:2dtvgPd3vsotKXtGk4dWlg", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/2dtvgPd3vsotKXtGk4dWlg", track.getHttpURL());
        assertEquals("Five", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("4c5a99ef480c133dc365642f827f79e6e963c565", track.getFiles().get(0));
        assertEquals(385000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(9, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(9);
        assertEquals("de8a77d4292f4dd9b106f1bb5abcf8eb", track.getId());
        assertEquals("spotify:track:6LVzKLdwFRbCJKAsEA0pKz", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/6LVzKLdwFRbCJKAsEA0pKz", track.getHttpURL());
        assertEquals("Cut and Run", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("7535328095a2ec09bcfbfc1d655862f9b55aa3a5", track.getFiles().get(0));
        assertEquals(207000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(10, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        track = album.getTracks().get(10);
        assertEquals("e67549687c5046aaa3b46de9367aa9a3", track.getId());
        assertEquals("spotify:track:70RPNhD2lWbwDZZPWCTWtd", track.getSpotifyURI());
        assertEquals("http://open.spotify.com/track/70RPNhD2lWbwDZZPWCTWtd", track.getHttpURL());
        assertEquals("The Lighthouse", track.getTitle());
        assertNull(track.getCover());
        assertEquals(1, track.getFiles().size());
        assertEquals("5e3062cfb5d3681ce6affa7cf40c581437900611", track.getFiles().get(0));
        assertEquals(262000l, track.getLength().longValue());
        assertTrue(track.getPopularity() > 0f);
        assertEquals(11, track.getTrackNumber().intValue());
        assertNull(track.getYear());
        assertTrue(track.getAlbum() == album);

        assertEquals("f6150726a8e94c89a7cf336d3f72be9c", album.getArtist().getId());
        assertEquals("spotify:artist:7ulIMfVKiXh8ecEpAVHIAY", album.getArtist().getSpotifyURI());
        assertEquals("http://open.spotify.com/artist/7ulIMfVKiXh8ecEpAVHIAY", album.getArtist().getHttpURL());
        assertEquals("Electrelane", album.getArtist().getName());
        assertNull(album.getArtist().getPopularity());
        assertNull(album.getArtist().getPortrait());
        // TODO: album.getArtist().getSimilarArtists();
      }
    });


    // end generated tests
  }


}
