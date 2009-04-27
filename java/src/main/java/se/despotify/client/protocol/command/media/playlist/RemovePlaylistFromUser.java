package se.despotify.client.protocol.command.media.playlist;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.client.protocol.PacketType;
import se.despotify.client.protocol.Protocol;
import se.despotify.client.protocol.channel.Channel;
import se.despotify.client.protocol.channel.ChannelCallback;
import se.despotify.client.protocol.command.Command;
import se.despotify.domain.Store;
import se.despotify.domain.User;
import se.despotify.domain.media.Playlist;
import se.despotify.domain.media.PlaylistContainer;
import se.despotify.exceptions.ProtocolException;
import se.despotify.util.Hex;
import se.despotify.util.XML;
import se.despotify.util.XMLElement;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Date;

/**
 * @since 2009-apr-26 23:53:40
 */
public class RemovePlaylistFromUser extends Command<Boolean> {

  private static Logger log = LoggerFactory.getLogger(RemovePlaylistFromUser.class);

  private Store store;
  private User user;
  private Playlist playlist;

  public RemovePlaylistFromUser(Store store, User user, Playlist playlist) {
    this.store = store;
    this.user = user;
    this.playlist = playlist;
  }

  @Override
  public Boolean send(Protocol protocol) throws ProtocolException {

    if (user.getPlaylists() == null) {
      log.warn("user playlists not loaded yet! should it be? loading..");
      new LoadUserPlaylists(store, user).send(protocol);
    }

    int position = user.getPlaylists().getItems().indexOf(playlist) + 1;
    if (position == 0) {
      throw new RuntimeException("playlist is not available in user playlists");
    }
    if (playlist != user.getPlaylists().getItems().remove(position - 1)) {
      throw new RuntimeException();
    }

    // todo probably dont destoy collaborative that is not owned by user? or?

    return sendDelete(protocol, position) && sendDestroy(protocol, position);
  }

  /*

   create and delete a playlist:

   shn_encrypt(ctx=0x1b6f5c, buf=0xbfffea3f, len=17 [0x0011]) called from 0x0007e635
     input (plaintext):
     0000000000000000 0000000000000000 00                                [?????????????????]
   shn_encrypt(ctx=0x852490, buf=0xbfffddb0, len=213 [0x00d5]) called from 0x000abc35
     input (plaintext):
     3600d20000928c1d 2c0a4e86dee946cf d96c1ac985020000 000000000000ffff [6???????,?N???F??l??????????????]
     ffff01033c69642d 69732d756e697175 652f3e3c6368616e 67653e3c6f70733e [????<id-is-unique/><change><ops>]
     3c6372656174652f 3e3c6e616d653e73 76656e736b746f70 70656e3c2f6e616d [<create/><name>svensktoppen</nam]
     653e3c2f6f70733e 3c74696d653e3132 3430383532303936 3c2f74696d653e3c [e></ops><time>1240852096</time><]
     757365723e6b656e 742e66696e656c6c 3c2f757365723e3c 2f6368616e67653e [user>kent.finell</user></change>]
     3c76657273696f6e 3e30303030303030 3030312c30303030 3030303030302c30 [<version>0000000001,0000000000,0]
     3030303030303030 312c303c2f766572 73696f6e3e                        [000000001,0</version>]
   readv(fd=5, iovec=0xb0122e18 {iov_base=0x81321c, iov_len=3000}, iovcnt=15) called from 0x000cc80f (thread 0xb0123000)
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090004                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x81321f, len=4 [0x0004]) called from 0x000adf64
     output (plaintext):		00000000                                                            [????]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090059                                                              [??Y]
   shn_decrypt(ctx=0x8523c0, buf=0x81322a, len=89 [0x0059]) called from 0x000adf64
     output (plaintext):
     00003c636f6e6669 726d3e3c7269643e 383432373c2f7269 643e3c7665727369 [??<confirm><rid>8427</rid><versi]
     6f6e3e3030303030 30303030312c3030 3030303030303030 2c30303030303030 [on>0000000001,0000000000,0000000]
     3030312c303c2f76 657273696f6e3e3c 2f636f6e6669726d 3e               [001,0</version></confirm>]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090002                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x81328a, len=2 [0x0002]) called from 0x000adf64
     output (plaintext):		0000                                                                [??]
   17:08:17.643 I [playlist] playlist ACK
   shn_encrypt(ctx=0x852490, buf=0xbfffdde0, len=232 [0x00e8]) called from 0x000abc35
     input (plaintext):
     3600e50000000000 0000000000000000 0000000000000000 001e00000003a452 [6??????????????????????????????R]
     183a00033c636861 6e67653e3c6f7073 3e3c6164643e3c69 3e333c2f693e3c69 [?:??<change><ops><add><i>3</i><i]
     74656d733e393238 6331643263306134 6538366465653934 3663666439366331 [tems>928c1d2c0a4e86dee946cfd96c1]
     616339383530323c 2f6974656d733e3c 2f6164643e3c2f6f 70733e3c74696d65 [ac98502</items></add></ops><time]
     3e31323430383532 3039363c2f74696d 653e3c757365723e 6b656e742e66696e [>1240852096</time><user>kent.fin]
     656c6c3c2f757365 723e3c2f6368616e 67653e3c76657273 696f6e3e30303030 [ell</user></change><version>0000]
     3030303033312c30 3030303030303030 342c323232323539 363130362c303c2f [000031,0000000004,2222596106,0</]
     76657273696f6e3e                                                    [version>]
   readv(fd=5, iovec=0xb0122e18 {iov_base=0x8c821c, iov_len=3000}, iovcnt=15) called from 0x000cc80f (thread 0xb0123000)
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090004                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x8c821f, len=4 [0x0004]) called from 0x000adf64
     output (plaintext):		00000000                                                            [????]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090059                                                              [??Y]
   shn_decrypt(ctx=0x8523c0, buf=0x8c822a, len=89 [0x0059]) called from 0x000adf64
     output (plaintext):
     00003c636f6e6669 726d3e3c7269643e 363230333c2f7269 643e3c7665727369 [??<confirm><rid>6203</rid><versi]
     6f6e3e3030303030 30303033312c3030 3030303030303034 2c32323232353936 [on>0000000031,0000000004,2222596]
     3130362c303c2f76 657273696f6e3e3c 2f636f6e6669726d 3e               [106,0</version></confirm>]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090002                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x8c828a, len=2 [0x0002]) called from 0x000adf64
     output (plaintext):		0000                                                                [??]
   17:08:18.148 I [playlist] playlist ACK
   readv(fd=5, iovec=0xb0122e18 {iov_base=0x81321c, iov_len=3000}, iovcnt=15) called from 0x000cc80f (thread 0xb0123000)
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090004                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x81321f, len=4 [0x0004]) called from 0x000adf64
     output (plaintext):		00020000                                                            [????]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090063                                                              [??c]
   shn_decrypt(ctx=0x8523c0, buf=0x81322a, len=99 [0x0063]) called from 0x000adf64
     output (plaintext):
     00023c626173652d 76657273696f6e3e 3c7269643e383831 373c2f7269643e3c [??<base-version><rid>8817</rid><]
     76657273696f6e3e 3030303030303030 30382c3030303030 30303030332c3235 [version>0000000008,0000000003,25]
     3839393839303834 2c303c2f76657273 696f6e3e3c2f6261 73652d7665727369 [89989084,0</version></base-versi]
     6f6e3e                                                              [on>]
   shn_decrypt(ctx=0x8523c0, buf=0xb0122e8c, len=3 [0x0003]) called from 0x000adef4
     output (plaintext):		090002                                                              [???]
   shn_decrypt(ctx=0x8523c0, buf=0x813294, len=2 [0x0002]) called from 0x000adf64
     output (plaintext):		0002                                                                [??]
   shn_encrypt(ctx=0x852490, buf=0xb0122aa0, len=57 [0x0039]) called from 0x000abc35
     input (plaintext):
     4800363430093109 706c61796c697374 0931343739360937 3731353033326239 [H?640?1?playlist?14796?7715032b9]
     6162666134333636 3966393230353931 6261623534356430 32               [abfa43669f920591bab545d02]
   shn_encrypt(ctx=0x852490, buf=0xbfffddc0, len=191 [0x00bf]) called from 0x000abc35
     input (plaintext):
     3600bc0000000000 0000000000000000 0000000000000000 001f00000004847a [6??????????????????????????????z]
     200a00033c636861 6e67653e3c6f7073 3e3c64656c3e3c69 3e333c2f693e3c6b [ ???<change><ops><del><i>3</i><k]
     3e313c2f6b3e3c2f 64656c3e3c2f6f70 733e3c74696d653e 3132343038353231 [>1</k></del></ops><time>12408521]
     30303c2f74696d65 3e3c757365723e6b 656e742e66696e65 6c6c3c2f75736572 [00</time><user>kent.finell</user]
     3e3c2f6368616e67 653e3c7665727369 6f6e3e3030303030 30303033322c3030 [></change><version>0000000032,00]
     3030303030303033 2c32373536383433 3537382c303c2f76 657273696f6e3e   [00000003,2756843578,0</version>]
   shn_encrypt(ctx=0x852490, buf=0xbfffddd0, len=174 [0x00ae]) called from 0x000abc35
     input (plaintext):
     3600ab0001928c1d 2c0a4e86dee946cf d96c1ac985020000 0001000000000000 [6???????,?N???F??l??????????????]
     000100033c636861 6e67653e3c6f7073 3e3c64657374726f 792f3e3c2f6f7073 [????<change><ops><destroy/></ops]
     3e3c74696d653e31 3234303835323130 303c2f74696d653e 3c757365723e6b65 [><time>1240852100</time><user>ke]
     6e742e66696e656c 6c3c2f757365723e 3c2f6368616e6765 3e3c76657273696f [nt.finell</user></change><versio]
     6e3e303030303030 303030322c303030 303030303030302c 3030303030303030 [n>0000000002,0000000000,00000000]
     30312c303c2f7665 7273696f6e3e                                       [01,0</version>]
   shn_encrypt(ctx=0x852490, buf=0xb0122aa0, len=56 [0x0038]) called from 0x000abc35
     input (plaintext):
     4800353430093109 706c61796c697374 0931313339093932 3863316432633061 [H?540?1?playlist?1139?928c1d2c0a]
     3465383664656539 3436636664393663 3161633938353032                  [4e86dee946cfd96c1ac98502]

  */

  /**
   * @param protocol
   * @return
   * @throws ProtocolException
   */
  public boolean sendDelete(Protocol protocol, int position) throws ProtocolException {

    ChannelCallback callback = new ChannelCallback();

    /* Create channel and buffer. */
    Channel channel = new Channel("Create-Playlist-Channel", Channel.Type.TYPE_PLAYLIST, callback);

    PlaylistContainer playlists = user.getPlaylists();

    String xml = String.format
        ("<change><ops><del><i>%s</i><k>%s</k></del></ops><time>%s</time><user>%s</user></change>" +
            "<version>%010d,%010d,%010d,%d</version>",
            // change
            position,
            1, // unknown
            new Date().getTime() / 1000,
            user.getName(),
            // version
            playlists.getRevision() + 1,
            playlists.getItems().size(),
            playlists.getChecksum(),
            playlist.isCollaborative() ? 1 : 0
        );

    byte[] xmlBytes = xml.getBytes();
    ByteBuffer buffer = ByteBuffer.allocate(2 + 16 + 1 + 4 + 4 + 4 + 1 + 1 + xmlBytes.length);

    //  3600bd00000000 000000000000000000 000000000000 00000426 0000001a a63f [6????????????????????????&??????]
    //  d3a7 00 03 3c636861 6e67653e3c6f7073 3e3c64656c3e3c69 3e32313c2f693e3c [????<change><ops><del><i>21</i><]


    buffer.putShort((short) channel.getId());
    buffer.put(Hex.toBytes("00000000000000000000000000000000")); // UUID? not used
    buffer.put((byte) 0x00); // type? not used
    buffer.putInt((int) playlists.getRevision());
    buffer.putInt(position);
    buffer.putInt((int) playlists.getChecksum());

    buffer.put((byte) (playlist.isCollaborative() ? 0x01 : 0x00));
    buffer.put((byte) 0x03); // unknown
    buffer.put(xmlBytes);
    buffer.flip();

    /* Register channel. */
    Channel.register(channel);

    /* Send packet. */
    protocol.sendPacket(PacketType.changePlaylist, buffer, "remove playlist from user");

//    /* Get response. */
//    byte[] data = callback.getData("remove playlist from user response");
//
//    assert data.length == 0;
//
    return true;

//    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<playlists>\n" +
//        new String(data, Charset.forName("UTF-8")) +
//        "\n</playlists>";
//
//    if (log.isInfoEnabled()) {
//      log.info(xml.replaceAll("\\s+", ""));
//    }
//    XMLElement response = XML.load(xml);
//
//    PlaylistContainer.fromXMLElement(response, store, user.getPlaylists());
//
//    if (response.hasChild("next-change")) {
//      return true;
//    } else {
//      playlists.getItems().add(position - 1, playlist);
//      throw new RuntimeException("Unknown server response:\n" + xml);
//    }
  }

  public Boolean sendDestroy(Protocol protocol, int position) throws ProtocolException {


    ChannelCallback callback = new ChannelCallback();

    /* Create channel and buffer. */
    Channel channel = new Channel("Create-Playlist-Channel", Channel.Type.TYPE_PLAYLIST, callback);

    PlaylistContainer playlists = user.getPlaylists();


    String xml = String.format
        ("<change><ops><destroy/></ops><time>%s</time><user>%s</user></change>" +
            "<version>%010d,%010d,%010d,%d</version>",
            new Date().getTime() / 1000,
            user.getName(),
            // version
            playlist.getRevision() + 1,
            playlist.getTracks().size(),
            playlist.getChecksum(),
            playlist.isCollaborative() ? 1 : 0
        );

    byte[] xmlBytes = xml.getBytes();
    ByteBuffer buffer = ByteBuffer.allocate(2 + 16 + 1 + 4 + 4 + 4 + 1 + 1 + xmlBytes.length);

    //  3600bd00000000 000000000000000000 000000000000 00000426 0000001a a63f [6????????????????????????&??????]
    //  d3a7 00 03 3c636861 6e67653e3c6f7073 3e3c64656c3e3c69 3e32313c2f693e3c [????<change><ops><del><i>21</i><]


    buffer.putShort((short) channel.getId());
    buffer.put(playlist.getUUID());
    buffer.put((byte) 0x02); // playlist type UUID tag
    buffer.putInt((int) playlist.getRevision().longValue());
    buffer.putInt(playlist.getTracks().size());
    buffer.putInt((int) playlists.getChecksum());

    buffer.put((byte) (playlist.isCollaborative() ? 0x01 : 0x00));
    buffer.put((byte) 0x03); // unknown
    buffer.put(xmlBytes);
    buffer.flip();

    /* Register channel. */
    Channel.register(channel);

    /* Send packet. */
    protocol.sendPacket(PacketType.changePlaylist, buffer, "destroy playlist");

    /* Get response. */
    byte[] data = callback.getData("destroy playlist response");

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<playlists>\n" +
        new String(data, Charset.forName("UTF-8")) +
        "\n</playlists>";

    if (log.isInfoEnabled()) {
      log.info(xml.replaceAll("\\s+", ""));
    }
    XMLElement response = XML.load(xml);

    Playlist.fromXMLElement(response, store, playlist);

    if (response.hasChild("next-change")) {
      return true;
    } else {
      playlists.getItems().add(position - 1, playlist);
      throw new RuntimeException("Unknown server response:\n" + xml);
    }
  }
}
