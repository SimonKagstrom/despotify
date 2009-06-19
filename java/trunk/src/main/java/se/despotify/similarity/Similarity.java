package se.despotify.similarity;

import se.despotify.DespotifyManager;
import se.despotify.domain.Store;

/**
 * Consumer facade against artist, album and track similarity classes.
 * 
 * @author kalle
 * @since 2009-jun-14 20:16:40
 */
public class Similarity {

  private AlbumSimilarity albumSimilarity;
  private ArtistSimilarity artistSimilarity;
  private TrackSimilarity trackSimilarity;

  private Store store;
  private DespotifyManager manager;

  public Similarity(Store store, DespotifyManager manager) {
    artistSimilarity = new ArtistSimilarity(store, manager);
    albumSimilarity = new AlbumSimilarity(store,manager, artistSimilarity);
    trackSimilarity = new TrackSimilarity(store,manager, artistSimilarity, albumSimilarity);
    this.store = store;
    this.manager = manager;
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

  public Store getStore() {
    return store;
  }

  public DespotifyManager getManager() {
    return manager;
  }

  

}
