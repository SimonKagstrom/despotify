package se.despotify;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.client.protocol.command.Command;
import se.despotify.exceptions.DespotifyException;

import java.util.HashMap;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * todo handle timeout on managed threads
 *
 * @since 2009-maj-12 15:52:41
 */
public class DespotifyManager {

  private static Logger log = LoggerFactory.getLogger(DespotifyManager.class);

  public DespotifyManager() {
  }

  public DespotifyManager(String username, String password) {
    this(username, password, 1);
  }

  public DespotifyManager(String username, String password, int poolSize) {
    this.username = username;
    this.password = password;
    this.poolSize = poolSize;
  }

  private String username;
  private String password;

  public Object send(Command command) throws DespotifyException {
    Object ret;
    ManagedConnection connection = getManagedConnection();
    try {
      ret = command.send(connection);
    } catch (DespotifyException e) {
      connection.close();
      throw e;
    }
    connection.close();
    return ret;    
  }

  public void connect() {
    getManagedConnection().close();
  }

  private static class ConnectionThread {
    private Thread thread;
    private Connection connection;
  }

  private Queue<ConnectionImpl> connections = new ConcurrentLinkedQueue<ConnectionImpl>();
  private int poolSize = 1;
  private AtomicInteger connectionsCreated = new AtomicInteger(0);
  private Map<ConnectionImpl, ConnectionThread> threads = new HashMap<ConnectionImpl, ConnectionThread>();

  public int getPoolSize() {
    return poolSize;
  }

  public void setPoolSize(int poolSize) {
    this.poolSize = poolSize;
  }

  public ManagedConnection getManagedConnection() throws DespotifyException {
    return new ManagedConnection(this, getConnection());
  }

  ConnectionImpl getConnection() throws DespotifyException {
    ConnectionImpl connection = connections.poll();
    if (connection != null && !connection.isConnected()) {
      ConnectionThread thread = threads.remove(connection);
      try {
        thread.thread.join(1000);
      } catch (Exception e) {
        thread.thread.stop();
      }
      connection = null;
    }
    if (connection == null) {
      if (connectionsCreated.incrementAndGet() <= poolSize) {
        ConnectionThread thread = new ConnectionThread();
        connection = new ConnectionImpl();
        thread.connection = connection;
        threads.put(connection, thread);
        connection.login(username, password);
        thread.thread = new Thread(connection);
        thread.thread.start();
        try {
          Thread.sleep(1000);
        } catch (Exception e) {
        }
        System.currentTimeMillis();
      } else {
        connectionsCreated.decrementAndGet();
        // todo wait for connection
        throw new UnsupportedOperationException("Not implemented> waiting for connection"); // todo
      }
    }
    return connection;
  }

  /**
   * releases the lock on a connection, allowing someone else to use it.
   *
   * @param connection
   */
  void releaseConnection(ConnectionImpl connection) {
    if (!threads.containsKey(connection)) {
      throw new UnsupportedOperationException("You really want to add an unknown connection to the pool?");
    }
    connections.add(connection);
  }

  public void stop() {
    stop(true);
  }

  public void stop(boolean wait) {
    for (ConnectionThread thread : threads.values()) {
      thread.connection.close();
    }
    for (ConnectionThread thread : threads.values()) {
      if (wait) {
        try {
          thread.thread.join();
        } catch (InterruptedException ie) {
          log.warn("Error joining connection thread " + thread.thread.getName(), ie);
        }
      } else {
        thread.thread.stop();
      }
    }
  }

  public String getUsername() {
    return username;
  }

  public void setUsername(String username) {
    this.username = username;
  }

  public String getPassword() {
    return password;
  }

  public void setPassword(String password) {
    this.password = password;
  }
}
