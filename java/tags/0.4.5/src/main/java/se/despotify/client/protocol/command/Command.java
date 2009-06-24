package se.despotify.client.protocol.command;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.exceptions.DespotifyException;
import se.despotify.ConnectionImpl;
import se.despotify.DespotifyManager;
import se.despotify.Connection;

/**
 *
 * @since 2009-apr-24 02:47:36
 */
public abstract class Command<T> {

  // this is here to make it easy to find the template code from a subclass..
  private static Logger log = LoggerFactory.getLogger(Command.class);

  public abstract T send(DespotifyManager connectionManager) throws DespotifyException;

}
