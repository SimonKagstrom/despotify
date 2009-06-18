package se.despotify.similarity;

import org.junit.Test;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.Artist;
import se.despotify.domain.media.Track;

/**
 * @author kalle
 * @since 2009-jun-18 17:05:20
 */
public class TestSimilarity extends se.despotify.DespotifyClientTest {

  @Test
  public void test() throws Exception {

    Similarity similarity = new Similarity(store, manager);

    System.out.println ("Artists");
    for (Artist artist : defaultArtists) {
      for (Artist artist1 : defaultArtists) {
        double value = similarity.getArtistSimilarity().itemSimilarity(artist, artist1);
        System.out.println(artist.getName() + " <-- " + value + " --> " + artist1.getName());
      }
    }


    System.out.println ("Albums");
    for (Album album : defaultAlbums) {
      for (Album album1 : defaultAlbums) {
        double value = similarity.getAlbumSimilarity().itemSimilarity(album, album1);
        System.out.println(album.getMainArtist().getName() + "#" + album.getName() + " <-- " + value + " --> " + album1.getMainArtist().getName() + "#" + album1.getName());
      }
    }

    System.out.println ("Tracks");
    for (Track track : defaultTracks) {
      for (Track track1 : defaultTracks) {
        double value = similarity.getTrackSimilarity().itemSimilarity(track, track1);
        System.out.println(track.getArtist().getName() + "#" + track.getTitle() + " <-- " + value + " --> " + track1.getArtist().getName() + "#" + track1.getTitle());

      }
    }

  }

}
