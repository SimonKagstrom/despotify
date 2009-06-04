package se.despotify;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Queue;
import java.util.Map;
import java.util.HashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

import se.despotify.exceptions.DespotifyException;

/**
 * @since 2009-maj-12 15:52:41
 */
public class DespotifyManager {

  private static Logger log = LoggerFactory.getLogger(DespotifyManager.class);

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

  private static class ConnectionThread {
    private Thread thread;
    private Connection connection;
  }

  private Queue<Connection> connections = new ConcurrentLinkedQueue<Connection>();
  private int poolSize = 1;
  private AtomicInteger connectionsCreated = new AtomicInteger(0);
  private Map<Connection, ConnectionThread> threads = new HashMap<Connection, ConnectionThread>();

  public int getPoolSize() {
    return poolSize;
  }

  public void setPoolSize(int poolSize) {
    this.poolSize = poolSize;
  }

  public Connection getConnection() throws DespotifyException {
    Connection connection = connections.poll();
    if (connection == null) {
      if (connectionsCreated.incrementAndGet() <= poolSize) {
        ConnectionThread thread = new ConnectionThread();
        connection = new Connection();
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
        throw new UnsupportedOperationException(""); // todo
      }
    }
    return connection;
  }

  public void releaseConnection(Connection connection) {
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
      thread.connection.stop();
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

}
