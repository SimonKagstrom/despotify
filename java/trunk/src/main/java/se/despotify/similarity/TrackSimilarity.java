package se.despotify.similarity;

import se.despotify.ConnectionManager;
import se.despotify.client.protocol.command.media.LoadTracks;
import se.despotify.domain.Store;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;

import java.util.HashSet;
import java.util.Set;

/**
 *
 * @see se.despotify.similarity.Similarity 
 * @author kalle
 * @since 2009-jun-13 17:39:00
 */
public class TrackSimilarity extends MediaSimilarity<Track> {


  private ArtistSimilarity artistSimilarity;
  private AlbumSimilarity albumSimilarity;

  TrackSimilarity(Store despotifyStore, ConnectionManager connectionManager, ArtistSimilarity artistSimilarity, AlbumSimilarity albumSimilarity) {
    super(despotifyStore, Track.class, connectionManager);
    this.artistSimilarity = artistSimilarity;
    this.albumSimilarity = albumSimilarity;
  }

  public double itemSimilarity(Track track, Track track1) throws DespotifyException {

    if (track == null || track1 == null) {
      throw new NullPointerException();
    }
    
    if (track.equals(track1)) {
      return 1d;
    }

    // todo load both at once
    if (track.getLoaded() == null) {
      new LoadTracks(store, track).send(manager);
    }
    if (track1.getLoaded() == null) {
      new LoadTracks(store, track1).send(manager);
    }

    // todo if artist and title equals return 0.99d. i.e. same track in multiple albums.     

    double similarity = 0d;

    if (track.getSimilarTracks() != null && track.getSimilarTracks().contains(track1)) {
      similarity += 0.3d;
    } else if (track1.getSimilarTracks() != null && track1.getSimilarTracks().contains(track)) {
      similarity += 0.3d;
    } else if (track.getSimilarTracks() != null && track1.getSimilarTracks() != null) {
      Set<Track> similarTracksInCommon = new HashSet<Track>(track.getSimilarTracks());
      similarTracksInCommon.retainAll(track1.getSimilarTracks());
      if (similarTracksInCommon.size() > 2) {
        similarity += 0.2d;
      } else if (similarTracksInCommon.size() > 0) {
        similarity += 0.1d;
      }
    }
        
    similarity += albumSimilarity.itemSimilarity(track.getAlbum(), track1.getAlbum()) / 3d;
    similarity += artistSimilarity.itemSimilarity(track.getArtist(), track1.getArtist()) / 3d;

    return similarity;
  }

}