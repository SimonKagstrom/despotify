package se.despotify;

import org.apache.commons.pool.BasePoolableObjectFactory;
import org.apache.commons.pool.impl.GenericObjectPool;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.client.protocol.command.Command;
import se.despotify.exceptions.DespotifyException;

import java.util.HashMap;
import java.util.Map;

/**
 * @since 2009-maj-12 15:52:41
 */
public class DespotifyManager {

  private static Logger log = LoggerFactory.getLogger(DespotifyManager.class);

  private GenericObjectPool connectionPool;


  public DespotifyManager() {
  }

  public DespotifyManager(String username, String password) {
    this(username, password, 1);
  }

  public DespotifyManager(String username, String password, int connectionPoolSize) {
    this.connectionPool = new GenericObjectPool(new PoolableConnectionFactory(username, password), connectionPoolSize);
  }

  public GenericObjectPool getConnectionPool() {
    return connectionPool;
  }

  public void setConnectionPool(GenericObjectPool connectionPool) {
    this.connectionPool = connectionPool;
  }

  @Deprecated
  public Object send(Command command) throws DespotifyException {
    return command.send(this);
  }

  public void connect() {
    getManagedConnection().close();
  }

  private static class ConnectionThread {
    private Thread thread;
    private Connection connection;
  }



  public ManagedConnection getManagedConnection() throws DespotifyException {
    return new ManagedConnection(this, getConnection());
  }

  ConnectionImpl getConnection() throws DespotifyException {
    try {
      return (ConnectionImpl)connectionPool.borrowObject();
    } catch (Exception e) {
      throw new DespotifyException(e);
    }
  }

  /**
   * releases the lock on a connection, allowing someone else to use it.
   *
   * @param connection
   * @throws DespotifyException
   */
  void releaseConnection(ConnectionImpl connection) throws DespotifyException {
    try {
      connectionPool.returnObject(connection);
    } catch (Exception e) {
      throw new DespotifyException(e);
    }
  }

  public void stop() throws DespotifyException {
    try {
      connectionPool.close();
    } catch (Exception e) {
      throw new DespotifyException(e);
    }

  }

  public static class PoolableConnectionFactory extends  BasePoolableObjectFactory{

    public PoolableConnectionFactory() {
    }

    public PoolableConnectionFactory(String username, String password) {
      this.username = username;
      this.password = password;
    }

    private String username;
    private String password;

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

    private Map<ConnectionImpl, ConnectionThread> threads = new HashMap<ConnectionImpl, ConnectionThread>();

      @Override
      public Object makeObject() throws Exception {
        ConnectionThread thread = new ConnectionThread();
        ConnectionImpl connection = new ConnectionImpl();
        thread.connection = connection;

        threads.put(connection, thread);

        connection.login(username, password);

        thread.thread = new Thread(connection);
        thread.thread.start();

        return connection;
      }

      @Override
      public void destroyObject(Object o) throws Exception {
        ConnectionImpl connection = (ConnectionImpl) o;
        ConnectionThread thread = threads.remove(connection);
        thread.connection.close();
        thread.thread.stop();
//        thread.thread.join(destroyConnectionJoinTimeOut);
        System.currentTimeMillis();
      }

      @Override
      public boolean validateObject(Object o) {
        ConnectionImpl connection = (ConnectionImpl) o;
        return connection.isConnected();
      }

  }
}
