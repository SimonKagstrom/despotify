package se.despotify;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.Protocol;
import se.despotify.client.protocol.Session;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.crypto.RSA;
import se.despotify.exceptions.AuthenticationException;
import se.despotify.exceptions.ConnectionException;
import se.despotify.exceptions.DespotifyException;
import se.despotify.exceptions.RecievedInvalidHeaderException;
import se.despotify.util.Hex;
import se.despotify.util.XML;
import se.despotify.util.XMLElement;

import java.nio.charset.Charset;
import java.util.Arrays;

public class ConnectionImpl implements Connection {

  private static final Charset UTF8 = Charset.forName("UTF-8");

  private boolean failFast = false;

  private static Logger log = LoggerFactory.getLogger(ConnectionImpl.class);

  private ProductType productType;

  private Session session;
  private Protocol protocol;
  private boolean running = false;  

  /**
   * Create a new instance using the default client revision
   */
  public ConnectionImpl() {
    this(-1);
  }

  /**
   * Create a new instance using a specified client revision
   *
   * @param revision Revision number to use when connecting.
   */
  public ConnectionImpl(int revision) {
    this.session = new Session(revision);
  }

  /**
   * Login to Spotify using the specified username and password.
   *
   * @param username Username to use.
   * @param password Corresponding password.
   * @throws ConnectionException
   * @throws AuthenticationException
   */
  public void login(String username, String password) throws DespotifyException {
    /* Authenticate session. */
    this.protocol = this.session.authenticate(username, password);

    /* Add command handler. */
    this.protocol.addListener(this);
  }

  /**
   * Closes connection to a Spotify server.
   *
   * @throws ConnectionException
   */
  public void close() throws ConnectionException {
    this.running = false;
  }

  /**
   * Continuously receives packets in order to handle them.
   * Use a {@link Thread} to run this.
   */
  public void run() {
    if (this.protocol == null) {
      throw new Error("You need to login first!");
    }

    running = true;

    while (running) {
      try {
        this.protocol.receivePacket();
      } catch (RecievedInvalidHeaderException e) {
        try {
          this.protocol.disconnect();
        } catch (Exception e2) {
          log.warn(e2.getMessage(), e2);
        }
        throw e;
      } catch (DespotifyException e) {

        log.error("Exception when receiving packet.", e);
        if (isFailFast()) {
          break;
        }
      }

    }
        
    try {
      this.protocol.disconnect();
    }
    catch (ConnectionException e) {
      log.error("Exception while disconnecting from protocol", e);
    }

  }

  /**
   * Handles incoming commands from the server.
   *
   * @param packetType A command.
   * @param payload    Payload of packet.
   */
  public void commandReceived(PacketType packetType, byte[] payload) {
    //System.out.format("< Command: 0x%02x Length: %d\n", command, payload.length);

    //switch(command){
    if (packetType == PacketType.secrectBlock) {
      /* Check length. */
      if (payload.length != 336) {
        System.err.format("Got command 0x02 with len %d, expected 336!\n", payload.length);
      }

      /* Check RSA public key. */
      byte[] rsaPublicKey = RSA.keyToBytes(this.session.getRSAPublicKey());

      for (int i = 0; i < 128; i++) {
        if (payload[16 + i] != rsaPublicKey[i]) {
          log.error(String.format("RSA public key doesn't match! %d\n", i));
          break;
        }
      }

      /* Send cache hash. */
      try {
        this.protocol.sendCacheHash();
      }
      catch (DespotifyException e) {
        log.warn("could not send cache hash", e);
      }


    } else if (packetType == PacketType.ping) {
      /* Ignore the timestamp but respond to the request. */
      /* int timestamp = IntegerUtilities.bytesToInteger(payload); */
      try {
        this.protocol.sendPong();
      } catch (DespotifyException e) {
        log.warn("could not send pong", e);
      }


    } else if (packetType == PacketType.channelData) {
      Channel.process(payload);


    } else if (packetType == PacketType.channelerR) {
      Channel.error(payload);


    } else if (packetType == PacketType.AESkey) {
      /* Channel id is at offset 2. AES Key is at offset 4. */
      Channel.process(Arrays.copyOfRange(payload, 2, payload.length));


    } else if (packetType == PacketType.SHAcache) {
      /* Do nothing. */


    } else if (packetType == PacketType.countryCode) {
      System.out.println("Country: " + new String(payload, UTF8));


    } else if (packetType == PacketType.p2pInitBlock) {
      /* Do nothing. */


    } else if (packetType == PacketType.notify) {
      /* HTML-notification, shown in a yellow bar in the official client. */
      /* Skip 11 byte header... */
      System.out.println("Notification: " + new String(
          Arrays.copyOfRange(payload, 11, payload.length), UTF8
      ));


    } else if (packetType == PacketType.productInformation) {
      /* Payload is uncompressed UTF8 formatted XML. */

      String xml = new String(payload, UTF8);
      XMLElement root = XML.load(xml);
      productType = ProductType.valueOf(root.getChild("product").getChild("type").getText());
      if (!allowProductType(productType)) {
        // todo more generic message
        log.error("Sorry, you need a premium account to use Despotify (this is a restriction by Spotify).\nTry setting property despotify.allowProductType = true");
        System.exit(0);
      }

    } else if (packetType == PacketType.welcome) {
      /* Request ads. */
      //this.protocol.sendAdRequest(new ChannelAdapter(), 0);
      //this.protocol.sendAdRequest(new ChannelAdapter(), 1);


    } else if (packetType == PacketType.pause) {
      /* TODO: Show notification and pause. */

    } else if (packetType == PacketType.pongAck) {

    } else if (packetType == PacketType.trackAddedToPlaylist) {

      // todo perhaps refresh the cache?
      String playlistId = Hex.toHex(payload);

    } else {
      log.warn("!!! Unsupported command " + packetType);
    }
  }


  public ProductType getProductType() {
    return productType;
  }

  public boolean allowProductType(ProductType productType) {
    return Boolean.valueOf(System.getProperty("despotify.allowProductType ", "false")) || productType == ProductType.premium;
  }

  public Session getSession() {
    return session;
  }

  public Protocol getProtocol() {
    return protocol;
  }

  /**
   * @return if false the connection will closed if an exception is thrown while receiving data.
   */
  public boolean isFailFast() {
    return failFast;
  }

  /**
   *
   * @param failFast false if the connection will be closed when an exception is thrown while receiving data.
   */
  public void setFailFast(boolean failFast) {
    this.failFast = failFast;
  }

  public boolean isConnected() {
    return running;
  }


}
