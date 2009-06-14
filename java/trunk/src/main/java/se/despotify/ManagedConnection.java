package se.despotify;

import se.despotify.exceptions.DespotifyException;
import se.despotify.exceptions.ConnectionException;
import se.despotify.client.protocol.Session;
import se.despotify.client.protocol.Protocol;
import se.despotify.client.protocol.PacketType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * an intermediate facade towards a connection
 * on order to drop the connection from a user
 * on time out or so.
 * 
 * @author kalle
 * @since 2009-jun-14 18:00:27
 */
public class ManagedConnection implements Connection {

  private static Logger log = LoggerFactory.getLogger(ManagedConnection.class);

  private ConnectionImpl decorated;
  private DespotifyManager manager;
  private boolean hasLock;

  public ManagedConnection(DespotifyManager manager, ConnectionImpl decorated) {
    this.manager = manager;
    this.decorated = decorated;
    hasLock = true;
  }

  private void checkConnection() throws DespotifyException {
    if (!isConnected()) {
      log.warn("Connection was lost! Attempting to reconnect.");
      decorated = manager.getConnection();
    }
  }

  /**
   * Login to Spotify using the specified username and password.
   *
   * @param username Username to use.
   * @param password Corresponding password.
   * @throws se.despotify.exceptions.ConnectionException
   *
   * @throws se.despotify.exceptions.AuthenticationException
   *
   */
  @Override
  public void login(String username, String password) throws DespotifyException {
    checkConnection();
    decorated.login(username, password);
  }

  /**
   * Closes connection to a Spotify server.
   *
   * @throws se.despotify.exceptions.ConnectionException
   *
   */
  @Override
  public void close() throws ConnectionException {
    if (manager != null) {
      manager.releaseConnection(decorated);
    }
  }

  @Override
  public ProductType getProductType() {
    checkConnection();
    return decorated.getProductType();
  }

  @Override
  public boolean allowProductType(ProductType productType) {
    checkConnection();
    return decorated.allowProductType(productType);
  }

  @Override
  public Session getSession() {
    checkConnection();
    return decorated.getSession();
  }

  @Override
  public Protocol getProtocol() {
    checkConnection();
    return decorated.getProtocol();
  }

  /**
   * @return if false the connection will closed if an exception is thrown while receiving data.
   */
  @Override
  public boolean isFailFast() {
    return decorated.isFailFast();
  }

  /**
   * @param failFast false if the connection will be closed when an exception is thrown while receiving data.
   */
  @Override
  public void setFailFast(boolean failFast) {
    decorated.setFailFast(failFast);
  }

  @Override
  public boolean isConnected() {
    return hasLock && decorated.isConnected();
  }


  @Override
  public void commandReceived(PacketType packetType, byte[] payload) {
    decorated.commandReceived(packetType, payload);
  }

  @Override
  public void run() {
    decorated.run();
  }

  void setHasLock(boolean hasLock) {
    this.hasLock = hasLock;
  }

}
