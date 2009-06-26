package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

@Entity
public class Track extends RestrictedMedia {

  private static final long serialVersionUID = 1L;

  @Column(length = 1024)
  private String title;
  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Artist artist;
  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Album album;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Album composition;

  private Integer year;
  private Integer discNumber;
  private Integer trackNumber;
  private Integer length;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<File> files;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Image cover;

  private Float popularity;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> similarTracks;

  /**
   * If this instance of track has been replaced with another track.
   */
  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> redirections;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<ExternalId> externalIds;

  public Track() {
  }

  public Track(byte[] UUID) {
    this(Hex.toHex(UUID));
  }

  public Track(String hexUUID) {
    this.id = hexUUID;
  }

  public Track(String hexUUID, String title, Artist artist, Album album) {
    this(hexUUID);
    this.title = title;
    this.artist = artist;
    this.album = album;
  }

  public Track(byte[] UUID, String title, Artist artist, Album album) {
    this(UUID);
    this.title = title;
    this.artist = artist;
    this.album = album;
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  @Override
  public String getSpotifyURI() {
    return "spotify:track:" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/track/" + SpotifyURI.toURI(getByteUUID());
  }

  public Integer getDiscNumber() {
    return discNumber;
  }

  public void setDiscNumber(Integer discNumber) {
    this.discNumber = discNumber;
  }

  public String getTitle() {
    return title;
  }

  public void setTitle(String title) {
    this.title = title;
  }

  public Artist getArtist() {
    return artist;
  }

  public void setArtist(Artist artist) {
    this.artist = artist;
  }

  public Album getAlbum() {
    return album;
  }

  public void setAlbum(Album album) {
    this.album = album;
  }

  public Integer getYear() {
    return year;
  }

  public void setYear(Integer year) {
    this.year = year;
  }

  public Integer getTrackNumber() {
    return trackNumber;
  }

  public void setTrackNumber(Integer trackNumber) {
    this.trackNumber = trackNumber;
  }

  public Integer getLength() {
    return length;
  }

  public void setLength(Integer length) {
    this.length = length;
  }

  public List<Track> getRedirections() {
    return redirections;
  }

  public void setRedirections(List<Track> redirections) {
    this.redirections = redirections;
  }

  public List<File> getFiles() {
    return files;
  }

  public void setFiles(List<File> files) {
    this.files = files;
  }

  public Image getCover() {
    return cover;
  }

  public void setCover(Image cover) {
    this.cover = cover;
  }

  public Float getPopularity() {
    return popularity;
  }

  public void setPopularity(Float popularity) {
    this.popularity = popularity;
  }

  public List<ExternalId> getExternalIds() {
    return externalIds;
  }

  public void setExternalIds(List<ExternalId> externalIds) {
    this.externalIds = externalIds;
  }

  public List<Track> getSimilarTracks() {
    return similarTracks;
  }

  public void setSimilarTracks(List<Track> similarTracks) {
    this.similarTracks = similarTracks;
  }

  public Album getComposition() {
    return composition;
  }

  public void setComposition(Album composition) {
    this.composition = composition;
  }

  /**
   * @param trackElement
   * @param store
   * @param fullyLoadedDate set this not null only if the xml contains data for the complete album
   * @return
   */
  public static Track fromXMLElement(XMLElement trackElement, Store store, Date fullyLoadedDate) {

    Track track = store.getTrack(trackElement.getChildText("id"));

    /* Set title. */
    if (trackElement.hasChild("title")) {
      track.title = trackElement.getChildText("title");
    }

    /* todo redirect
    3c3f786d6c207665 7273696f6e3d2231 2e302220656e636f 64696e673d227574 [<?xml version="1.0" encoding="ut]
    662d38223f3e0a0a 0a0a0a0a3c726573 756c743e0a20203c 76657273696f6e3e [f-8"?>??????<result>?  <version>]
    313c2f7665727369 6f6e3e0a20203c74 6f74616c2d747261 636b733e323c2f74 [1</version>?  <total-tracks>2</t]
    6f74616c2d747261 636b733e0a20203c 747261636b733e0a 202020203c747261 [otal-tracks>?  <tracks>?    <tra]
    636b3e0a20202020 20203c69643e3163 3637393463636535 3636346135316132 [ck>?      <id>1c6794cce5664a51a2]
    6263343766393934 3936613930633c2f 69643e0a20202020 20203c7265646972 [bc47f99496a90c</id>?      <redir]
    6563743e35336239 6236396166623639 3439653561326461 6539326433316561 [ect>53b9b69afb6949e5a2dae92d31ea]
    336638623c2f7265 6469726563743e0a 2020202020203c72 656469726563743e [3f8b</redirect>?      <redirect>]
    3437643461653962 3033653634626333 3962353732383932 3535353065653566 [47d4ae9b03e64bc39b5728925550ee5f]
    3c2f726564697265 63743e0a20202020 20203c7469746c65 3e54616b65205465 [</redirect>?      <title>Take Te]
    6e3c2f7469746c65 3e0a202020202020 3c6172746973742d 69643e6339386637 [n</title>?      <artist-id>c98f7]
    6663313066613034 6564636138373764 3862393131376332 3035633c2f617274 [fc10fa04edca877d8b9117c205c</art]
    6973742d69643e0a 2020202020203c61 72746973743e5061 756c204465736d6f [ist-id>?      <artist>Paul Desmo]
    6e643c2f61727469 73743e0a20202020 20203c6172746973 742d69643e626261 [nd</artist>?      <artist-id>bba]
    6639356331636438 3534363031613938 3862303830636437 35393330373c2f61 [f95c1cd854601a988b080cd759307</a]
    72746973742d6964 3e0a202020202020 3c6172746973743e 4761626f7220537a [rtist-id>?      <artist>Gabor Sz]
    61626f3c2f617274 6973743e0a202020 2020203c616c6275 6d3e536b796c6172 [abo</artist>?      <album>Skylar]
    6b3c2f616c62756d 3e0a202020202020 3c616c62756d2d69 643e363537353361 [k</album>?      <album-id>65753a]
    3838656431623461 3032623465363064 3130313432363761 30613c2f616c6275 [88ed1b4a02b4e60d1014267a0a</albu]
    6d2d69643e0a2020 202020203c796561 723e313937333c2f 796561723e0a2020 [m-id>?      <year>1973</year>?  ]
    202020203c747261 636b2d6e756d6265 723e313c2f747261 636b2d6e756d6265 [    <track-number>1</track-numbe]
    723e0a2020202020 203c6c656e677468 3e3336353432363c 2f6c656e6774683e [r>?      <length>365426</length>]
    0a2020202020203c 66696c65733e0a20 202020202020203c 66696c652069643d [?      <files>?        <file id=]
    2236636438633235 3636643036326636 3930376232303562 3662363961353165 ["6cd8c2566d062f6907b205b6b69a51e]
    6163316532313737 342220666f726d61 743d224f67672056 6f726269732c3136 [ac1e21774" format="Ogg Vorbis,16]
    303030302c312c33 322c3422202f3e3c 2f66696c65733e0a 2020202020203c63 [0000,1,32,4" /></files>?      <c]
    6f7665723e643837 3630353863623866 3232343065303066 3761373165636430 [over>d876058cb8f2240e00f7a71ecd0]
    3539366463623630 35653938633c2f63 6f7665723e0a2020 202020203c706f70 [596dcb605e98c</cover>?      <pop]
    756c61726974793e 302e32393736393c 2f706f70756c6172 6974793e0a202020 [ularity>0.29769</popularity>?   ]
    2020203c72657374 72696374696f6e73 3e0a202020202020 20203c7265737472 [   <restrictions>?        <restr]
    696374696f6e2066 6f7262696464656e 3d2241532c41572c 41582c424c2c4256 [iction forbidden="AS,AW,AX,BL,BV]
    2c43412c43432c43 4f2c43522c43582c 45432c47472c4753 2c484d2c494d2c4a [,CA,CC,CO,CR,CX,EC,GG,GS,HM,IM,J]
    452c4a502c4d452c 4d462c4e462c4e5a 2c52532c534a2c55 4d2c55532c56452c [E,JP,ME,MF,NF,NZ,RS,SJ,UM,US,VE,]


     */

    /* Set artist. */
    if (trackElement.hasChild("artist-id") && trackElement.hasChild("artist")) {
      track.artist = store.getArtist(trackElement.getChildText("artist-id"));
      track.artist.setId(trackElement.getChildText("artist-id"));
      track.artist.setName(trackElement.getChildText("artist"));
    }

    /* Set album. */
    if (trackElement.hasChild("album-id")) {
      track.album = store.getAlbum(trackElement.getChildText("album-id"));
    }

    if (trackElement.hasChild("album")) {
      // todo not sure when this happends or what might happend then...
      track.album.setName(trackElement.getChildText("album"));
    }

    /* Set year. */
    if (trackElement.hasChild("year")) {
      try {
        track.year = Integer.parseInt(trackElement.getChildText("year"));
      }
      catch (NumberFormatException e) {
// ignored
      }
    }

    /* Set track number. */
    if (trackElement.hasChild("track-number")) {
      track.trackNumber = Integer.parseInt(trackElement.getChildText("track-number"));
    }

    /* Set length. */
    if (trackElement.hasChild("length")) {
      track.length = Integer.parseInt(trackElement.getChildText("length"));
    }

    if (trackElement.hasChild("similar-tracks")) {
      List<Track> similarTracks = new ArrayList<Track>();
      for (XMLElement similarTrackElement : trackElement.getChild("similar-tracks").getChildren()) {
        similarTracks.add(store.getTrack(similarTrackElement.getText()));
      }
      track.setSimilarTracks(similarTracks);
    }

    /* Set files. */
    if (trackElement.hasChild("files")) {

      List<File> files = new ArrayList<File>();

      for (XMLElement fileElement : trackElement.getChild("files").getChildren()) {
        File file = store.getFile(fileElement.getAttribute("id"));
        file.setFormat(fileElement.getAttribute("format"));
        files.add(file);
      }
      track.setFiles(files);
    }

    /* Set cover. */
    // FIXED: now null if ""
    if (trackElement.hasChild("cover")) {
      String value = trackElement.getChildText("cover");
      if (!"".equals(value)) {
        track.cover = store.getImage(value);
      }
    }

    /* Set popularity. */
    if (trackElement.hasChild("popularity")) {
      track.popularity = Float.parseFloat(trackElement.getChildText("popularity"));
    }


    XMLElement restrictionsNode = trackElement.getChild("restrictions");
    if (restrictionsNode != null) {
      RestrictedMedia.fromXMLElement(restrictionsNode, track);
    }

    if (fullyLoadedDate != null) {
      track.setLoaded(fullyLoadedDate);
    }

    return track;

  }

  @Override
  public String toString() {
    return "Track{" +
        "id='" + id + '\'' +
        ", title='" + title + '\'' +
        ", artist=" + (artist == null ? null : artist.getId()) +
        ", album=" + (album == null ? null : album.getId()) +
        ", year=" + year +
        ", discNumber=" + discNumber +
        ", trackNumber=" + trackNumber +
        ", length=" + length +
        ", files=" + files +
        ", cover='" + cover + '\'' +
        ", popularity=" + popularity +
        '}';
  }

}
