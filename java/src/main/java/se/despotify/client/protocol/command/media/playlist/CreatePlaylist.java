package se.despotify.client.protocol.command.media.playlist;

import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.User;
import se.despotify.domain.media.Playlist;
import se.despotify.exceptions.DespotifyException;
import se.despotify.Connection;

/**
 * @since 2009-apr-27 00:46:31
 */
public class CreatePlaylist extends Command<Playlist> {

  private Store store;
  private User user;
  private String playlistName;
  private boolean collaborative;

  public CreatePlaylist(Store store, User user, String playlistName, boolean collaborative) {
    this.store = store;
    this.user = user;
    this.playlistName = playlistName;
    this.collaborative = collaborative;
  }

  @Override
  public Playlist send(Connection connection) throws DespotifyException {
    byte[] playlistUUID = new ReserveRandomPlaylistUUID(store, user, playlistName, collaborative).send(connection);
    Playlist playlist = store.getPlaylist(playlistUUID);
    playlist.setAuthor(user.getName());
    playlist.setName(playlistName);
    playlist.setUUID(playlistUUID);
    if (new CreatePlaylistWithReservedUUID(store, user, playlist).send(connection)) {
      return playlist;
    } else {
      throw new DespotifyException();
    }
  }
}
