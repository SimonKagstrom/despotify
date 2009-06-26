package se.despotify.client.protocol.command.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.ConnectionManager;
import se.despotify.ManagedConnection;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.ResponseUnmarshaller;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.media.Result;
import se.despotify.exceptions.DespotifyException;

import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
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

  public Result send(ConnectionManager connectionManager) throws DespotifyException {
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
    ManagedConnection connection = connectionManager.getManagedConnection();
    connection.getProtocol().sendPacket(PacketType.search, buffer, "search");

    /* Get data and inflate it. */
    byte[] data = callback.getData("gzipped search response");
    connection.close();

//    System.out.println(new String(GZIP.inflate(data), UTF8));

    try {
      XMLStreamReader xmlr = ResponseUnmarshaller.createReader(new InputStreamReader(new GZIPInputStream(new ByteArrayInputStream(data)), Charset.forName("UTF-8")));
      ResponseUnmarshaller responseUnmarshaller = new ResponseUnmarshaller(store, xmlr);
      responseUnmarshaller.skip();
      if (!"result".equals(xmlr.getLocalName())) {
        throw new DespotifyException("Expected document root to be of type <result>");
      }

      Result result = responseUnmarshaller.unmarshallSearchResult();
      xmlr.close();
      return result;

    } catch (XMLStreamException e) {
      throw new DespotifyException(e);
    } catch (IOException e) {
      throw new DespotifyException(e);
    }


  }
}
