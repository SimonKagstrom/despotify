package se.despotify.client.protocol.command.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.DespotifyManager;
import se.despotify.ManagedConnection;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.ResponseUnmarshaller;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.media.Result;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;
import se.despotify.util.*;

import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.List;
import java.util.zip.GZIPInputStream;

/**
 * @since 2009-apr-25 20:07:48
 */
public class Search extends Command<Result> {

  private static Logger log = LoggerFactory.getLogger(Search.class);

  // result.setQuery(query)

  private Store store;
  private String query;
  private int offset;
  private int maxResults;

  public Search(Store store, String query) {
    this(store, query, 0, -1);
  }

  public Search(Store store, String query, int offset, int maxResults) {
    this.store = store;
    this.query = query;
    this.offset = offset;
    this.maxResults = maxResults;
  }

  public Result send(DespotifyManager connectionManager) throws DespotifyException {
    /* Create channel callback */
    ChannelCallback callback = new ChannelCallback();

    byte[] utf8Bytes = query.getBytes(Charset.forName("UTF8"));

    /* Create channel and buffer. */
    Channel channel = new Channel("Search-Channel", Channel.Type.TYPE_SEARCH, callback);
    ByteBuffer buffer = ByteBuffer.allocate(2 + 4 + 4 + 2 + 1 + utf8Bytes.length);

    /* Check offset and limit. */
    if (offset < 0) {
      throw new IllegalArgumentException("Offset needs to be >= 0");
    }
//    else if ((maxResults < 0 && maxResults != -1) || maxResults == 0) {
//      throw new IllegalArgumentException("Limit needs to be either -1 for no limit or > 0");
//    }

    /* Append channel id, some values, query length and query. */
    buffer.putShort((short) channel.getId());
    buffer.putInt(offset); /* Result offset. */
    buffer.putInt(maxResults); /* Reply limit. */
    buffer.putShort((short) 0x0000);
    buffer.put((byte) utf8Bytes.length);
    buffer.put(utf8Bytes);
    buffer.flip();

    /* Register channel. */
    Channel.register(channel);

    /* Send packet. */
    Timer timeSendPacket = new Timer("send packet");
    ManagedConnection connection = connectionManager.getManagedConnection();
    connection.getProtocol().sendPacket(PacketType.search, buffer, "search");

    /* Get data and inflate it. */
    byte[] compressedData = callback.getData("gzipped search response");
    connection.close();
    timeSendPacket.stop();

    Timer timeOld = new Timer("old");
    Timer timeInflate = new Timer("inflate only");
    byte[] inflatedData = GZIP.inflate(compressedData);
    timeInflate.stop();

    if (log.isInfoEnabled()) {
      log.info("received search response packet, " + inflatedData.length + " uncompressed bytes:\n" + Hex.log(inflatedData, log));
    }


    /* Cut off that last 0xFF byte... */
    inflatedData = Arrays.copyOfRange(inflatedData, 0, inflatedData.length - 1);

    String xml = new String(inflatedData, UTF8);
    if (log.isDebugEnabled()) {
      log.debug(xml);
    }
    XMLElement root = XML.load(xml);

    /* Create result from XML. */

    Result result = Result.fromXMLElement(root, store);

    timeOld.stop();



    System.out.println(timeSendPacket);
    System.out.println(timeInflate);
    System.out.println(timeOld);
//    System.out.println(timeNew);
    return result;

  }
}
