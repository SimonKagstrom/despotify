package se.despotify.client.protocol.command;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.client.protocol.Protocol;
import se.despotify.exceptions.DespotifyException;
import se.despotify.Connection;
import se.despotify.DespotifyManager;

/**
 *
 * @since 2009-apr-24 02:47:36
 */
public abstract class Command<T> {

  // this is here to make it easy to find the template code from a subclass..
  private static Logger log = LoggerFactory.getLogger(Command.class);

  public T send(DespotifyManager manager) throws DespotifyException {
    Connection connection = manager.getConnection();
    T ret = send(connection);
    manager.releaseConnection(connection);
    return ret;
  }

  public abstract T send(Connection connection) throws DespotifyException;

}
