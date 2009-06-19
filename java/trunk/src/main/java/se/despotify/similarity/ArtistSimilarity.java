package se.despotify.similarity;

import se.despotify.DespotifyManager;
import se.despotify.client.protocol.command.media.LoadArtist;
import se.despotify.domain.Store;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.Artist;
import se.despotify.exceptions.DespotifyException;

import java.util.HashSet;
import java.util.Set;

/**
 * @see se.despotify.similarity.Similarity
 * @author kalle
 * @since 2009-jun-13 17:38:47
 */
public class ArtistSimilarity extends MediaSimilarity<Artist> {


  ArtistSimilarity(Store despotifyStore, DespotifyManager despotifyManager) {
    super(despotifyStore, Artist.class, despotifyManager);
  }

  public double itemSimilarity(Artist artist, Artist artist1) throws DespotifyException {

    if (artist.equals(artist1)) {
      return 1d;
    }

    if (artist.getLoaded() == null) {
      artist = (Artist) manager.send(new LoadArtist(store, artist));
    }
    if (artist1.getLoaded() == null) {
      artist1 = (Artist) manager.send(new LoadArtist(store, artist1));
    }

    double similarity = 0d;


    if (artist.getSimilarArtists() != null && artist.getSimilarArtists().contains(artist1)) {
      similarity += 0.5d;
    } else if (artist1.getSimilarArtists() != null && artist1.getSimilarArtists().contains(artist)) {
      similarity += 0.5d;
    }

    if (artist.getGenres() != null && artist1.getGenres() != null) {
      Set<String> genres = new HashSet<String>(artist.getGenres());
      genres.retainAll(artist1.getGenres());
      if (genres.size() > 2) {
        similarity += 0.2d;
      } else if (genres.size() > 0) {
        similarity += 0.1d;
      }
    }

    if (artist.getAllAlbumsWithTrackPresent() != null && artist1.getAllAlbumsWithTrackPresent() != null) {
      Set<Album> albums = new HashSet<Album>(artist.getAllAlbumsWithTrackPresent());
      albums.retainAll(artist1.getAllAlbumsWithTrackPresent());
      if (albums.size() > 2) {
        similarity += 0.2d;
      } else if (albums.size() > 0) {
        similarity += 0.1d;
      }
    } else {
      System.currentTimeMillis();  // should be various artists only
    }

//    if (se.despotify.similarity == 0d) {
//      se.despotify.similarity = -1d;
//    }

    return similarity;

  }

}
