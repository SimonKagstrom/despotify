package se.despotify.similarity;

import se.despotify.ConnectionManager;
import se.despotify.domain.Store;
import se.despotify.domain.media.Media;
import se.despotify.exceptions.DespotifyException;

import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-14 02:31:40
 */
public abstract class MediaSimilarity<M extends Media> implements Serializable {

  protected final Store store;
  protected final ConnectionManager manager;
  protected final Class<M> _class;

  protected MediaSimilarity(Store store, Class<M> _class, ConnectionManager connectionManager) {
    this.store = store;
    this.manager = connectionManager;
    this._class = _class;
  }

  /**
   *
   * @param item
   * @param item1
   * @return the similarity of the two items, where 1 means they are exactly the same and -1 means they are completely the opposite to each other.
   * @throws DespotifyException
   */
  public abstract double itemSimilarity(M item, M item1) throws DespotifyException;

}
