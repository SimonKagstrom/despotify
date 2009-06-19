package se.despotify.similarity;

import se.despotify.DespotifyManager;
import se.despotify.client.protocol.command.media.LoadTracks;
import se.despotify.domain.Store;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;

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


    if (track.getLoaded() == null) {
      despotifyManager.send(new LoadTracks(despotifyStore, track));
    }
    if (track1.getLoaded() == null) {
      despotifyManager.send(new LoadTracks(despotifyStore, track1));
    }

    double similarity = 0d;
    
    similarity += albumSimilarity.itemSimilarity(track.getAlbum(), track1.getAlbum()) / 2d;
    similarity += artistSimilarity.itemSimilarity(track.getArtist(), track1.getArtist()) / 2d;

    return similarity;
  }

}