package se.despotify.client.protocol.command.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.BrowseType;
import se.despotify.ConnectionManager;
import se.despotify.ManagedConnection;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.ResponseUnmarshaller;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.media.Track;
import se.despotify.exceptions.DespotifyException;

import javax.xml.stream.XMLStreamReader;
import javax.xml.stream.XMLStreamException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.ArrayList;
import java.util.zip.GZIPInputStream;
import java.io.Reader;
import java.io.InputStreamReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;

/**
 * @since 2009-apr-25 16:28:42
 */
public class LoadTracks extends Command<Object> {

//  public static final MeanTimer timeRequest = new MeanTimer("request and get zipped xml response");
//  public static final MeanTimer timeUnmarshall = new MeanTimer("unmarshall zipped xml response");

  protected static Logger log = LoggerFactory.getLogger(LoadTracks.class);

  private Track[] tracks;
  private Store store;

  public LoadTracks(Store store, Collection<Track> tracks) {
    this(store, tracks.toArray(new Track[tracks.size()]));

  }

  public LoadTracks(Store store, Track... tracks) {
    this.store = store;
    this.tracks = tracks;
  }

  @Override
  public Boolean send(ConnectionManager connectionManager) throws DespotifyException {

    // todo send multiple requests if more than 200 tracks!

    if (tracks.length == 0) {
      throw new DespotifyException("No tracks supplied to be loaded!");
    }

    if (tracks.length > 240) {
      throw new DespotifyException("Can only load up to 240 track at the time.");
    }

/* Create channel callback */
    ChannelCallback callback = new ChannelCallback();

    /* Send browse request. */

    /* Create channel and buffer. */
    Channel channel = new Channel("Browse-Channel", Channel.Type.TYPE_BROWSE, callback);
    ByteBuffer buffer = ByteBuffer.allocate(2 + 1 + tracks.length * 16); //+ ((type == BrowseType.artist || type == BrowseType.album)?4:0));

//		if(ids.size() > 1 && type != BrowseType.track){
//			throw new IllegalArgumentException("Only BrowserType.track accepts multiple ids.");
//		}

    /* Append channel id and type. */
    buffer.putShort((short) channel.getId());
    buffer.put((byte) BrowseType.track.getValue());

    /* Append (16 byte) ids. */
    for (Track track : tracks) {
      buffer.put(Arrays.copyOfRange(track.getByteUUID(), 0, 16));
    }

//		/* Append zero. */
//		if(type == BrowseType.artist || type == BrowseType.album){
//			buffer.putInt(0);
//		}

    buffer.flip();

    /* Register channel. */
    Channel.register(channel);

    /* Send packet. */
//    timeRequest.start();

    ManagedConnection connection = connectionManager.getManagedConnection();
    connection.getProtocol().sendPacket(PacketType.browse, buffer, "load track");
    byte[] compressedData = callback.getData("gzipped load tracks reponse");
    connection.close();

//    timeRequest.stop();

    try {

//      timeUnmarshall.start();

      Reader reader = new InputStreamReader(new GZIPInputStream(new ByteArrayInputStream(compressedData)), UTF8);
      XMLStreamReader xmlr = ResponseUnmarshaller.createReader(reader);
      ResponseUnmarshaller unmarshaller = new ResponseUnmarshaller(store, xmlr);
      unmarshaller.skip();
      List<Track> tracks = unmarshaller.unmarshallLoadTracks();
      xmlr.close();
      for (Track track : tracks) {
        store.persist(track);
      }
      
//      timeUnmarshall.stop();

    } catch (IOException e) {
      throw new DespotifyException(e);
    } catch (XMLStreamException e) {
      throw new DespotifyException(e);
    }

//    System.out.println(timeRequest);
//    System.out.println(timeUnmarshall);

    return true;


  }
}
