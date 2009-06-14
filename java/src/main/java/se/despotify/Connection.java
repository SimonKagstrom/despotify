package se.despotify;

import se.despotify.client.protocol.CommandListener;
import se.despotify.client.protocol.Session;
import se.despotify.client.protocol.Protocol;
import se.despotify.exceptions.DespotifyException;
import se.despotify.exceptions.ConnectionException;

/**
 * @author kalle
 * @since 2009-jun-14 17:58:55
 */
public interface Connection extends CommandListener, Runnable {
  /**
   * Login to Spotify using the specified username and password.
   *
   * @param username Username to use.
   * @param password Corresponding password.
   * @throws se.despotify.exceptions.ConnectionException
   * @throws se.despotify.exceptions.AuthenticationException
   */
  void login(String username, String password) throws DespotifyException;

  /**
   * Closes connection to a Spotify server.
   *
   * @throws se.despotify.exceptions.ConnectionException
   */
  void close() throws ConnectionException;

  ProductType getProductType();

  boolean allowProductType(ProductType productType);

  Session getSession();

  Protocol getProtocol();

  /**
   * @return if false the connection will closed if an exception is thrown while receiving data.
   */
  boolean isFailFast();

  /**
   *
   * @param failFast false if the connection will be closed when an exception is thrown while receiving data.
   */
  void setFailFast(boolean failFast);

  boolean isConnected();

  public enum ProductType {
    free, daypass, premium
  }
}
