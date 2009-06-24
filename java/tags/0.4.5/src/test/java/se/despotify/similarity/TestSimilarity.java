package se.despotify.similarity;

import org.junit.Test;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.Artist;
import se.despotify.domain.media.Track;
import se.despotify.util.SpotifyURI;

/**
 * @author kalle
 * @since 2009-jun-18 17:05:20
 */
public class TestSimilarity extends se.despotify.DespotifyClientTest {

  @Test
  public void test() throws Exception {

    Similarity similarity = new Similarity(store, manager);

    Artist dolly = store.getArtist(SpotifyURI.toHex("32vWCbZh0xZ4o9gkz4PsEU"));
    Artist kenny = store.getArtist(SpotifyURI.toHex("0WjkBDqno4HbjwNDqyMgVa"));
    Artist gary = store.getArtist(SpotifyURI.toHex("2DI8C4YNiTSH2RiNa7ViVx"));
        
    System.out.println ("Artists");
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(dolly, dolly)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(dolly, kenny)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(dolly, gary)));

    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(kenny, dolly)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(kenny, kenny)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(kenny, gary)));

    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(gary, dolly)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(gary, kenny)));
    System.out.println(String.valueOf(similarity.getArtistSimilarity().itemSimilarity(gary, gary)));


    System.out.println ("Albums");
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(0), dolly.getAllAlbumsWithTrackPresent().get(0))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(0), dolly.getAllAlbumsWithTrackPresent().get(1))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(0), dolly.getAllAlbumsWithTrackPresent().get(2))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(0), dolly.getAllAlbumsWithTrackPresent().get(3))));

    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getMainArtistAlbums().get(0), dolly.getMainArtistAlbums().get(0))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getMainArtistAlbums().get(0), dolly.getMainArtistAlbums().get(1))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getMainArtistAlbums().get(0), dolly.getMainArtistAlbums().get(2))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getMainArtistAlbums().get(0), dolly.getMainArtistAlbums().get(3))));


    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(0), kenny.getAllAlbumsWithTrackPresent().get(0))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(1), kenny.getAllAlbumsWithTrackPresent().get(1))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(2), kenny.getAllAlbumsWithTrackPresent().get(2))));
    System.out.println(String.valueOf(similarity.getAlbumSimilarity().itemSimilarity(dolly.getAllAlbumsWithTrackPresent().get(3), kenny.getAllAlbumsWithTrackPresent().get(3))));

    System.out.println ("Tracks");
    for (Track track : defaultTracks) {
      for (Track track1 : defaultTracks) {
        double value = similarity.getTrackSimilarity().itemSimilarity(track, track1);
        System.out.println(track.getArtist().getName() + "#" + track.getTitle() + " <-- " + value + " --> " + track1.getArtist().getName() + "#" + track1.getTitle());

      }
    }

  }

}
