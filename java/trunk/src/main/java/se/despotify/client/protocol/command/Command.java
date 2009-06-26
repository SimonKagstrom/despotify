package se.despotify.client.protocol.command;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.exceptions.DespotifyException;
import se.despotify.ConnectionManager;

import java.nio.charset.Charset;

/**
 *
 * @since 2009-apr-24 02:47:36
 */
public abstract class Command<T> {

  public static final Charset UTF8 = Charset.forName("UTF-8");

  // this is here to make it easy to find the template code from a subclass..
  private static Logger log = LoggerFactory.getLogger(Command.class);

  public abstract T send(ConnectionManager connectionManager) throws DespotifyException;

}
