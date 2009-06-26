package se.despotify.similarity;

import se.despotify.ConnectionManager;
import se.despotify.client.protocol.command.media.LoadAlbum;
import se.despotify.client.protocol.command.media.LoadTracks;
import se.despotify.domain.Store;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * @author kalle
 * @see se.despotify.similarity.Similarity
 * @since 2009-jun-13 17:39:19
 */
public class AlbumSimilarity extends MediaSimilarity<Album> {

  private ArtistSimilarity artistSimilarity;

  AlbumSimilarity(Store despotifyStore, ConnectionManager connectionManager, ArtistSimilarity artistSimilarity) {
    super(despotifyStore, Album.class, connectionManager);
    this.artistSimilarity = artistSimilarity;
  }

  public double itemSimilarity(Album album, Album album1) throws DespotifyException {

    if (album.equals(album1)) {
      return 1d;
    }

    double similarity = 0d;

    if (album.getLoaded() == null) {
      album = new LoadAlbum(store, album).send(manager);
    }

    if (album1.getLoaded() == null) {
      album1 = new LoadAlbum(store, album1).send(manager);
    }


    similarity += artistSimilarity.itemSimilarity(album.getMainArtist(), album1.getMainArtist()) / 3d;


    // add mean similarity of all artists of all tracks of the albums

    // load..
    Set<Track> tracksToLoad = new HashSet<Track>();
    for (Track track : album.getTracks()) {
      if (track.getLoaded() == null) {
        tracksToLoad.add(track);
      }
    }
    for (Track track : album1.getTracks()) {
      if (track.getLoaded() == null) {
        tracksToLoad.add(track);
      }
    }
    if (tracksToLoad.size() > 0) {
      new LoadTracks(store, tracksToLoad).send(manager);
    }

    // sum up
    List<Double> distances = new ArrayList<Double>();
    for (Track track : new ArrayList<Track>(album.getTracks())) {
      for (Track track1 : new ArrayList<Track>(album1.getTracks())) {
        distances.add(artistSimilarity.itemSimilarity(track.getArtist(), track1.getArtist()));
      }
    }
    double length = distances.size();
    double tracksArtistsSimilariry = 0d;
    for (Double distance : distances) {
      tracksArtistsSimilariry += distance;
    }
    tracksArtistsSimilariry /= length;

    similarity += tracksArtistsSimilariry / 3d;


    return similarity;
  }

}
