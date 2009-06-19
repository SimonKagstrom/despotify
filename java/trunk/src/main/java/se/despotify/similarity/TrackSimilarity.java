package se.despotify.similarity;

import se.despotify.DespotifyManager;
import se.despotify.client.protocol.command.media.LoadTracks;
import se.despotify.domain.Store;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;

import java.util.HashSet;
import java.util.Set;

/**
 * @author kalle
 * @since 2009-jun-13 17:39:00
 */
public class TrackSimilarity extends MediaSimilarity<Track> {


  private ArtistSimilarity artistSimilarity;
  private AlbumSimilarity albumSimilarity;

  public TrackSimilarity(Store despotifyStore, DespotifyManager despotifyManager, ArtistSimilarity artistSimilarity, AlbumSimilarity albumSimilarity) {
    super(despotifyStore, Track.class, despotifyManager);
    this.artistSimilarity = artistSimilarity;
    this.albumSimilarity = albumSimilarity;
  }

  public double itemSimilarity(Track track, Track track1) throws DespotifyException {

    if (track.equals(track1)) {
      return 1d;
    }

    // todo load both at once
    if (track.getLoaded() == null) {
      despotifyManager.send(new LoadTracks(despotifyStore, track));
    }
    if (track1.getLoaded() == null) {
      despotifyManager.send(new LoadTracks(despotifyStore, track1));
    }

    // todo if artist and title equals return 0.99d. i.e. same track in multiple albums.     

    double similarity = 0d;

    if (track.getSimilarTracks() != null && track.getSimilarTracks().contains(track1)) {
      similarity += 0.3d;
    } else if (track1.getSimilarTracks() != null && track.getSimilarTracks().contains(track1)) {
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