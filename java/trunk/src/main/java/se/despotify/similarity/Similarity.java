package se.despotify.similarity;

import se.despotify.DespotifyManager;
import se.despotify.domain.Store;

/**
 * @author kalle
 * @since 2009-jun-14 20:16:40
 */
public class Similarity {

  private AlbumSimilarity albumSimilarity;
  private ArtistSimilarity artistSimilarity;
  private TrackSimilarity trackSimilarity;

  public Similarity(Store despotifyStore, DespotifyManager despotifyManager) {
    artistSimilarity = new ArtistSimilarity(despotifyStore, despotifyManager);
    albumSimilarity = new AlbumSimilarity(despotifyStore,despotifyManager, artistSimilarity);
    trackSimilarity = new TrackSimilarity(despotifyStore,despotifyManager, artistSimilarity, albumSimilarity);
  }


  public AlbumSimilarity getAlbumSimilarity() {
    return albumSimilarity;
  }

  public ArtistSimilarity getArtistSimilarity() {
    return artistSimilarity;
  }

  public TrackSimilarity getTrackSimilarity() {
    return trackSimilarity;
  }
}
