package se.despotify;

import junit.framework.TestCase;
import org.junit.After;
import org.junit.Before;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.Store;
import se.despotify.domain.User;
import se.despotify.domain.media.Album;
import se.despotify.domain.media.Artist;
import se.despotify.domain.media.Playlist;
import se.despotify.domain.media.Track;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;

import java.util.Random;

/**
 * @since 2009-apr-20 20:41:13
 */
public abstract class DespotifyClientTest extends TestCase {

  public final String hex40 = "^[a-f0-9]{40}$";
  public final String hex32 = "^[a-f0-9]{32}$";

  protected final Logger log = LoggerFactory.getLogger(getClass());

  protected DespotifyManager manager;

  protected Store store;
  protected User user;

  protected Playlist defaultPlaylist;
  protected Artist[] defaultArtists;
  protected Album[] defaultAlbums;
  protected Track[] defaultTracks;

  protected String username;
  protected String password;

  public String randomPlaylistName() {
    long seed = System.currentTimeMillis();
    Random random = new Random(seed);

    byte[] playlistNameBytes = new byte[8];
    random.nextBytes(playlistNameBytes);
    return "despotify_" + getClass().getSimpleName() + "_" + Hex.toHex(playlistNameBytes);
  }

  /**
   * reconnected to spotify with a new empty memory store.
   * @throws Exception
   */
  public void reset() throws Exception {
    if (manager != null) {
      manager.stop();
    }
    manager = new DespotifyManager(username, password);
    if (doConnect()) {
      manager.connect();
    }

    user = new User();
    user.setId(username);

    store = new MemoryStore();

    defaultPlaylist = store.getPlaylist(Hex.toBytes(SpotifyURI.toHex("6wvPFkLGKOVl1v3qRJD6HX")));

    defaultArtists = new Artist[]{
        store.getArtist(Hex.toBytes(SpotifyURI.toHex("6kACVPfCOnqzgfEF5ryl0x"))),
        store.getArtist(Hex.toBytes(SpotifyURI.toHex("2qc41rNTtdLK0tV3mJn2Pm"))),
        store.getArtist(Hex.toBytes(SpotifyURI.toHex("6FXMGgJwohJLUSr5nVlf9X"))),
        store.getArtist(Hex.toBytes(SpotifyURI.toHex("7rZR0ugcLEhNrFYOrUtZii"))),
        store.getArtist(Hex.toBytes(SpotifyURI.toHex("7ulIMfVKiXh8ecEpAVHIAY"))),
    };

    defaultAlbums = new Album[]{
        store.getAlbum(Hex.toBytes(SpotifyURI.toHex("05BIC4TZptbiQoF03QhojS"))),
        store.getAlbum(Hex.toBytes(SpotifyURI.toHex("2mLIJwfgNPGjpuKaN7njPm"))),
        store.getAlbum(Hex.toBytes(SpotifyURI.toHex("5CnZjFfPDmxOX7KnWLLqpC"))),
        store.getAlbum(Hex.toBytes(SpotifyURI.toHex("6eEhgZIrHftYRvgpAKJC2K"))),
        store.getAlbum(Hex.toBytes(SpotifyURI.toHex("3GETv5yNXeM0cnhq8XahWu"))),
    };

    defaultTracks = new Track[]{
        store.getTrack(Hex.toBytes(SpotifyURI.toHex("4vdV2Eua6RkUoUM51jdH56"))), // 93f98ea75b4748f797668485a3d01bd0
        store.getTrack(Hex.toBytes(SpotifyURI.toHex("6iVTOPCmpABvG9jDZ2JozY"))), // cf2cd530980e450d855977ba0a80ec6e
        store.getTrack(Hex.toBytes(SpotifyURI.toHex("7FKhuZtIPchBVNIhFnNL5W"))), // fc1f1b5860f04a739971fcad9c1cd634
        store.getTrack(Hex.toBytes(SpotifyURI.toHex("3qqKWUVfiLMrNPacFRzTzh"))), // 7093f50c9ecf428eb780348c076f9f7f
        store.getTrack(Hex.toBytes(SpotifyURI.toHex("2dtvgPd3vsotKXtGk4dWlg"))), // 48daf12f96f84793a526b579aa4d1f66
    };

  }

  @Before
  @Override
  protected void setUp() throws Exception {
    super.setUp();

    try {
      System.getProperties().load(getClass().getResourceAsStream("/despotify.properties"));
    } catch (Exception e) {
      log.warn("could not load properties file", e);
    }

    username = System.getProperty("despotify.username", null);
    password = System.getProperty("despotify.password", null);

    if (username == null || password == null) {
      throw new Exception("Both -Ddespotify.username and -Ddespotify.password must be set or set in src/test/resources/despotify.properties");
    }

    reset();

  }

  protected boolean doConnect() {
    return true;
  }

  @After
  @Override
  protected void tearDown() throws Exception {
    log.info("Tearing down test..");
    super.tearDown();
    if (manager != null) {
      manager.stop();
    }
  }
}
