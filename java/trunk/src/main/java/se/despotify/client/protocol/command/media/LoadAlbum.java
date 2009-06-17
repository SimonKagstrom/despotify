package se.despotify.client.protocol.command.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.BrowseType;
import se.despotify.Connection;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.media.Album;
import se.despotify.exceptions.DespotifyException;
import se.despotify.util.GZIP;
import se.despotify.util.Hex;
import se.despotify.util.XML;
import se.despotify.util.XMLElement;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Date;
import java.io.*;

/**
 * @since 2009-apr-25 16:28:42
 */
public class LoadAlbum extends Command<Album> {

  protected static Logger log = LoggerFactory.getLogger(LoadAlbum.class);

  private Album album;
  private Store store;

  public LoadAlbum(Store store, Album album) {
    this.store = store;
    this.album = album;
  }

  @Override
  public Album send(Connection connection) throws DespotifyException {

/* Create channel callback */
    ChannelCallback callback = new ChannelCallback();

    /* Send browse request. */

    /* Create channel and buffer. */
    Channel channel = new Channel("Browse-Channel", Channel.Type.TYPE_BROWSE, callback);
    ByteBuffer buffer = ByteBuffer.allocate(2 + 1 + 16 + 4);


    buffer.putShort((short) channel.getId());
    buffer.put((byte) BrowseType.album.getValue());
    buffer.put(album.getByteUUID());
    buffer.putInt(0); // unknown


    buffer.flip();

    /* Register channel. */
    Channel.register(channel);

    /* Send packet. */
    connection.getProtocol().sendPacket(PacketType.browse, buffer, "load album");

//    INFO  Protocol - sending load album, 26 bytes:
//        1   3    5  7   |9   11  13  15  |17  19  21  23  |25  27  29  31  |           1111111112222222222333
//          2   4   6   8 |  10  12  14  16|  18  20  22  24|  26  28  31  32| 12345678901234567890123456789012
//        ----------------|----------------|----------------|----------------|----------------------------------
//        30001700000202f8 df4ad52d449caca8 c6a25d2eca080000 0000             [0????????J?-D?????].??????]


    /* Get data and inflate it. */
    byte[] data = GZIP.inflate(callback.getData("gzipped load album response"));

    if (log.isInfoEnabled()) {
      log.info("load album response, " + data.length + " uncompressed bytes:\n" + Hex.log(data, log));
    }


    /* Cut off that last 0xFF byte... */
    data = Arrays.copyOfRange(data, 0, data.length - 1);
    /* Load XML. */

    String xml = new String(data, Charset.forName("UTF-8"));
    if (log.isDebugEnabled()) {
      log.debug(xml);
    }

//    try {
//      Writer out = new OutputStreamWriter(new FileOutputStream(new File("tmp/load_album_"+album.getId()+".xml")), "UTF8");
//      out.write(xml);
//      out.close();
//    } catch (IOException e) {
//      e.printStackTrace();
//    }


    XMLElement root = XML.load(xml);

    // load tracks


    if (!"album".equals(root.getElement().getNodeName())) {
      throw new DespotifyException("Expected document root to be of type <album>");
    }
    Album album = Album.fromXMLElement(root, store, new Date());
    if (this.album != album) {
      throw new DespotifyException("Album in response has different UUID than the requested album!");
    }

    return (Album)store.persist(album);
  }
}