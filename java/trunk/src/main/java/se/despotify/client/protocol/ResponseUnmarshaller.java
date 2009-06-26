package se.despotify.client.protocol;

import com.sun.xml.internal.stream.events.XMLEventAllocatorImpl;
import se.despotify.domain.MemoryStore;
import se.despotify.domain.Store;
import se.despotify.domain.media.*;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import javax.xml.stream.events.XMLEvent;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.*;

/**
 * Spotify XML response parser
 * <p/>
 * Uses XML stream to parse the data.
 * <p/>
 * <p/>
 * Benchmarks compared with previous DOM based solution:
 * <p/>
 * 5 tracks takes ~30 milliseconds to request gzipped XML.
 * ~30 ms mean time to unmarshall 5 tracks using DOM.
 * ~7 ms using XML stream
 * <p/>
 * <p/>
 * 200 tracks takes ~120 milliseconds to request gzipped XML.
 * ~400 ms mean time to unmarshall 200 tracks using DOM.
 * ~50 ms using XML stream
 * <p/>
 * The average artist takes ~50 ms to request gzipped XML
 * while a major artist can take up to one second.
 * ~200 ms mean time to unmarshall an artist when loading 200 random
 * ~50 ms using XML stream
 *
 * Dolly Parton takes 245 ms with XML stream and 4130 ms with DOM.
 * Kenny Rogers takes 131 ms with XML stream and 2815 ms with DOM.
 * Johnny Cash takes 480 ms with XML stream and 8503 ms with DOM.
 *
 * @author kalle
 * @since 2009-jun-24 00:40:32
 */
public class ResponseUnmarshaller {

  /**
   * TODO move these to tests
   *
   * @param args
   * @throws Exception
   */
  public static void main(String[] args) throws Exception {
    loadArtist();
    loadAlbum();
    loadTracks();
  }

  public static void loadArtist() throws Exception {
    Store store = new MemoryStore();
    XMLStreamReader xmlr = createReader(new InputStreamReader(new FileInputStream(new java.io.File("src/main/resources/response/xml/load_artist_4f9873e19e5a4b4096c216c98bcdb010.xml")), "UTF8"));
    ResponseUnmarshaller unmarshaller = new ResponseUnmarshaller(store, xmlr);
    unmarshaller.skip();
    Artist artist = unmarshaller.unmarshallArtist(new Date());
    System.currentTimeMillis();
  }

  public static void loadAlbum() throws Exception {
    Store store = new MemoryStore();
    XMLStreamReader xmlr = createReader(new InputStreamReader(new FileInputStream(new java.io.File("src/main/resources/response/xml/load_album_2f90dd571f8942e0b8bb6e06b2f6d5ed.xml")), "UTF8"));
    ResponseUnmarshaller unmarshaller = new ResponseUnmarshaller(store, xmlr);
    unmarshaller.skip();
    Album album = unmarshaller.unmarshallAlbum(new Date());
    System.currentTimeMillis();
  }

  public static void loadTracks() throws Exception {
    Store store = new MemoryStore();
    XMLStreamReader xmlr = createReader(new InputStreamReader(new FileInputStream(new java.io.File("src/main/resources/response/xml/load_tracks_1245076237936.xml")), "UTF8"));
    ResponseUnmarshaller unmarshaller = new ResponseUnmarshaller(store, xmlr);
    unmarshaller.skip();
    List<Track> tracks = unmarshaller.unmarshallLoadTracks();
    System.currentTimeMillis();
  }

  public static XMLStreamReader createReader(Reader xml) throws XMLStreamException {
    XMLInputFactory xmlif = XMLInputFactory.newInstance();
    xmlif.setEventAllocator(new XMLEventAllocatorImpl());
    return xmlif.createXMLStreamReader(xml);
  }

  private Store store;
  private XMLStreamReader xmlr;

  public ResponseUnmarshaller(Store store, XMLStreamReader xmlr) {
    this.store = store;
    this.xmlr = xmlr;
  }


  public List<Track> unmarshallLoadTracks() throws XMLStreamException {
    Date now = new Date();

    List<Track> tracks = new ArrayList<Track>();

    int eventType;
    String localName;

    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      localName = xmlr.getLocalName();
      if ("version".equals(localName)) {
        skip();
      } else if ("total-tracks".equals(localName)) {
        skip();
      } else if ("tracks".equals(localName)) {
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("track".equals(localName)) {
            tracks.add(unmarshallTrack(now));
          } else {
            throw unexpected();
          }
        }
      } else {
        throw unexpected();
      }
    }

    return tracks;
  }


  /**
   * <xs:complexType name="restrictionsType">
   * <xs:sequence>
   * <xs:element type="restrictionType" name="restriction"/>
   * </xs:sequence>
   * </xs:complexType>
   * <p/>
   * <xs:complexType name="restrictionType">
   * <xs:simpleContent>
   * <xs:extension base="xs:string">
   * <xs:attribute type="xs:string" name="allowed" use="optional"/>
   * <xs:attribute type="xs:string" name="catalogues" use="optional"/>
   * <xs:attribute type="xs:string" name="forbidden" use="optional"/>
   * </xs:extension>
   * </xs:simpleContent>
   * </xs:complexType>
   *
   * @param restrictedMedia
   */
  public boolean unmarshalRestrictedMedia(RestrictedMedia restrictedMedia) throws XMLStreamException {
    String localName = xmlr.getLocalName();
    if ("restrictions".equals(localName)) {
      restrictedMedia.setRestrictions(new ArrayList<Restriction>());
      int eventType;
      while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
        localName = xmlr.getLocalName();
        if ("restriction".equals(localName)) {
          Restriction restriction = new Restriction();
          for (int i = 0; i < xmlr.getAttributeCount(); i++) {
            String attribute = xmlr.getAttributeName(i).getLocalPart();
            String value = xmlr.getAttributeValue(i).trim();
            if (!"".equals(value)) {
              List<String> values = Arrays.asList(value.split(",|\\s+"));
              if ("allowed".equals(attribute)) {
                restriction.setAllowed(new HashSet<String>(values));
              } else if ("catalogues".equals(attribute)) {
                restriction.setCatalogues(new HashSet<String>(values));
              } else if ("forbidden".equals(attribute)) {
                restriction.setForbidden(new HashSet<String>(values));
              } else {
                throw unexpected();
              }
            }
          }
          skip();
          restrictedMedia.getRestrictions().add(restriction);
        } else {
          throw unexpected();
        }
      }

    } else if ("allowed".equals(localName)) {
      skip();
    } else if ("forbidden".equals(localName)) {
      skip();
    } else {
      return false;
    }

    return true;
  }

  public Track unmarshallTrack(Date fullyLoaded) throws XMLStreamException {

    Track track = null;

    List<Track> redirects = null;

    // sometimes album name occurs prior to the album id.
    String albumName = null;
    String albumArtistName = null;

    int eventType;
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      String localName = xmlr.getLocalName();
      if ("id".equals(localName)) {
        track = store.getTrack(xmlr.getElementText());
      } else if ("title".equals(localName)) {
        track.setTitle(getString());
      } else if ("artist-id".equals(localName)) {
        track.setArtist(store.getArtist(getString()));
      } else if ("artist".equals(localName)) {
        track.getArtist().setName(getString());
      } else if ("album-artist".equals(localName)) {
        if (track.getAlbum().getMainArtist() == null) {
          albumArtistName = getString();
        } else {
          track.getAlbum().getMainArtist().setName(getString());
        }
      } else if ("album-artist-id".equals(localName)) {
        track.getAlbum().setMainArtist((store.getArtist(getString())));
        if (albumArtistName != null) {
          track.getAlbum().getMainArtist().setName(albumArtistName);
          albumArtistName = null;
        }
      } else if ("album".equals(localName)) {
        if (track.getAlbum() == null) {
          albumName = getString();
        } else {
          track.getAlbum().setName(xmlr.getElementText());
        }
      } else if ("album-id".equals(localName)) {
        track.setAlbum(store.getAlbum(getString()));
        if (albumName != null) {
          track.getAlbum().setName(albumName);
          albumName = null;
        }
      } else if ("year".equals(localName)) {
        track.setYear(getInteger());
      } else if ("track-number".equals(localName)) {
        track.setTrackNumber(getInteger());
      } else if ("length".equals(localName)) {
        track.setLength(getInteger());
      } else if ("redirect".equals(localName)) {
        if (redirects == null) {
          redirects = new ArrayList<Track>();
        }
        redirects.add(store.getTrack(getString()));
      } else if ("files".equals(localName)) {
        track.setFiles(new ArrayList<File>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("file".equals(localName)) {
            File file = null;
            for (int i = 0; i < xmlr.getAttributeCount(); i++) {
              String attribute = xmlr.getAttributeName(i).getLocalPart();
              if ("id".equals(attribute)) {
                file = store.getFile(xmlr.getAttributeValue(i));
              } else if ("format".equals(attribute)) {
                file.setFormat(xmlr.getAttributeValue(i));
              } else {
                throw unexpected();
              }
            }
            track.getFiles().add(file);
            skip();
          } else {
            throw unexpected();
          }
        }
      } else if ("links".equals(localName)) {
        skipLinks();
      } else if ("album-links".equals(localName)) {
        skipLinks();
      } else if ("cover".equals(localName)) {
        track.setCover(store.getImage(getString()));
      } else if ("popularity".equals(localName)) {
        track.setPopularity(getFloat());
      } else if ("similar-tracks".equals(localName)) {
        track.setSimilarTracks(new ArrayList<Track>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("id".equals(localName)) {
            track.getSimilarTracks().add(store.getTrack(getString()));
          } else {
            throw unexpected();
          }
        }
      } else if ("external-ids".equals(localName)) {
        track.setExternalIds(unmarshallExternalIds());

      } else if (!unmarshalRestrictedMedia(track)) {
        throw unexpected();
      }
    }

    // we replace all redirects if they were loaded.
    if (redirects != null) {
      track.setRedirections(redirects);
    }

    if (fullyLoaded != null) {
      track.setLoaded(fullyLoaded);
      track = (Track) store.persist(track);
    }

    return track;
  }

  private List<ExternalId> unmarshallExternalIds() throws XMLStreamException {
    int eventType;
    String localName;
    List<ExternalId> externalIds = new ArrayList<ExternalId>();
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      localName = xmlr.getLocalName();
      if ("external-id".equals(localName)) {
        ExternalId externalId = new ExternalId();
        for (int i = 0; i < xmlr.getAttributeCount(); i++) {
          String attribute = xmlr.getAttributeName(i).getLocalPart();
          if ("type".equals(attribute)) {
            externalId.setType(xmlr.getAttributeValue(i));
          } else if ("id".equals(attribute)) {
            externalId.setExternalId(xmlr.getAttributeValue(i));
          } else {
            throw unexpected();
          }
        }
        externalIds.add(externalId);
        skip();
      } else {
        throw unexpected();
      }
    }
    return externalIds;
  }

  private void skipLinks() throws XMLStreamException {
    int eventType;
    String localName;
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      localName = xmlr.getLocalName();
      if ("link".equals(localName)) {
        eventType = skip(); // end element
        // todo keep these? i think its buy-album buttons or something like that
      } else {
        throw unexpected();
      }
    }
  }

  public Album unmarshallAlbum(Date fullyLoaded) throws XMLStreamException {

    Album album = null;
    String albumName = null; // sometimes album name occurs prior to the album id.
    String artistName = null;  // sometimes artist name occurs prior to the artist id.

    int eventType;
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      String localName = xmlr.getLocalName();
      if ("id".equals(localName)) {
        album = store.getAlbum(xmlr.getElementText());
        if (albumName != null) {
          album.setName(albumName);
          albumName = null;
        }
      } else if ("version".equals(localName)) {
        skip();
      } else if ("name".equals(localName)) {
        if (album == null) {
          albumName = xmlr.getElementText();
        } else {
          album.setName(xmlr.getElementText());
        }
      } else if ("artist-id".equals(localName)) {
        album.setMainArtist(store.getArtist(xmlr.getElementText()));
        if (artistName != null) {
          album.getMainArtist().setName(artistName);
          artistName = null;
        }
      } else if ("artist".equals(localName)) {
        if (album == null || album.getMainArtist() == null) {
          artistName = xmlr.getElementText();
        } else {
          album.getMainArtist().setName(xmlr.getElementText());
        }
      } else if ("album-type".equals(localName)) {
        album.setType(xmlr.getElementText());
      } else if ("year".equals(localName)) {
        album.setYear(getInteger());
      } else if ("cover".equals(localName)) {
        album.setCover(store.getImage(xmlr.getElementText()));
      } else if ("copyright".equals(localName)) {
        album.setP(new ArrayList<String>());
        album.setC(new ArrayList<String>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("c".equals(localName)) {
            album.getC().add(xmlr.getElementText());
          } else if ("p".equals(localName)) {
            album.getP().add(xmlr.getElementText());
          } else {
            throw unexpected();
          }
        }
      } else if ("review".equals(localName)) {
        album.setReview(xmlr.getElementText());
      } else if ("links".equals(localName)) {
        skipLinks();
      } else if ("discs".equals(localName)) {
        album.setTracks(new ArrayList<Track>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("disc".equals(localName)) {
            Integer discNumber = null;
            while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
              localName = xmlr.getLocalName();
              if ("disc-number".equals(localName)) {
                discNumber = Integer.valueOf(xmlr.getElementText());
              } else if ("name".equals(localName)) {
                album.setDiscName(discNumber - 1, xmlr.getElementText());
              } else if ("track".equals(localName)) {
                Track track = unmarshallTrack(fullyLoaded);
                track.setDiscNumber(discNumber);
                track.setAlbum(album);
                album.getTracks().add(track);
              } else {
                throw unexpected();
              }
            }
          } else {
            throw unexpected();
          }
        }
      } else if ("similar-albums".equals(localName)) {
        album.setSimilarAlbums(new ArrayList<Album>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("id".equals(localName)) {
            album.getSimilarAlbums().add(store.getAlbum(xmlr.getElementText()));
          } else {
            throw unexpected();
          }
        }
      } else if ("external-ids".equals(localName)) {
        album.setExternalIds(unmarshallExternalIds());

      } else if (!unmarshalRestrictedMedia(album)) {
        throw unexpected();

      }

    }

    if (fullyLoaded != null) {
      album.setLoaded(fullyLoaded);
      album = (Album) store.persist(album);
    }

    return album;
  }

  /**
   * <xs:complexType name="artistType">
   * <xs:choice maxOccurs="unbounded" minOccurs="0">
   * <xs:element type="hex_32"               name="id"/>
   * <xs:element type="xs:string"            name="name"/>
   * <xs:element type="imageType"            name="portrait" />
   * <xs:element type="comma_seperated"      name="genres"/>
   * <xs:element type="xs:string"            name="years-active"/>
   * <xs:element type="xs:long"              name="version"/>
   * <xs:element type="biosType"             name="bios"/>
   * <xs:element type="similar-artistsType"  name="similar-artists"/>
   * <xs:element type="albumsType"           name="albums"/>
   * </xs:choice>
   * </xs:complexType>
   *
   * @return
   * @throws XMLStreamException
   */
  public Artist unmarshallArtist(Date fullyLoaded) throws XMLStreamException {

    Artist artist = null;
    String artistName = null; // sometimes artist name occurs prior to the artist id.

    int eventType;
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      String localName = xmlr.getLocalName();
      if ("id".equals(localName)) {
        artist = store.getArtist(xmlr.getElementText());
        if (artistName != null) {
          artist.setName(artistName);
          artistName = null;
        }
      } else if ("name".equals(localName)) {
        if (artist == null) {
          artistName = xmlr.getElementText();
        } else {
          artist.setName(xmlr.getElementText());
        }
      } else if ("portrait".equals(localName)) {
        artist.setPortrait(unmarshallImage());
      } else if ("genres".equals(localName)) {
        String string = xmlr.getElementText().trim();
        if (!"".equals(string)) {
          artist.setGenres(new HashSet<String>(Arrays.asList(string.split(","))));
        }
      } else if ("years-active".equals(localName)) {
        artist.setYearsActive(new ArrayList<String>(Arrays.asList(xmlr.getElementText().split(","))));
      } else if ("version".equals(localName)) {
        // ignored, todo log something if not 1?
        eventType = skip(); // end element
      } else if ("bios".equals(localName)) {
        if (artist.getBiographies() == null) {
          artist.setBiographies(new ArrayList<Biography>());
        }
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("bio".equals(localName)) {
            Biography biography = new Biography();
            while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
              localName = xmlr.getLocalName();
              if ("text".equals(localName)) {
                biography.setText(xmlr.getElementText());
              } else if ("portraits".equals(localName)) {
                biography.setPortraits(new ArrayList<Image>());
                while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
                  localName = xmlr.getLocalName();
                  if ("portrait".equals(localName)) {
                    biography.getPortraits().add(unmarshallImage());
                  } else {
                    throw unexpected();
                  }
                }
              } else {
                throw unexpected();
              }
            }
            artist.getBiographies().add(biography);
          } else {
            throw unexpected();
          }
        }
      } else if ("similar-artists".equals(localName)) {
        artist.setSimilarArtists(new ArrayList<Artist>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("artist".equals(localName)) {
            artist.getSimilarArtists().add(unmarshallArtist(null));
          } else {
            throw unexpected();
          }
        }
      } else if ("albums".equals(localName)) {
        artist.setMainArtistAlbums(new ArrayList<Album>());
        artist.setAllAlbumsWithTrackPresent(new ArrayList<Album>());
        artist.setTracks(new ArrayList<Track>());
        while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
          localName = xmlr.getLocalName();
          if ("album".equals(localName)) {
            Album album = unmarshallAlbum(fullyLoaded);
            artist.getAllAlbumsWithTrackPresent().add(album);
            if (artist.equals(album.getMainArtist())) {
              artist.getMainArtistAlbums().add(album);
            }
            for (Track track : album.getTracks()) {
              if (artist.equals(track.getArtist())) {
                artist.getTracks().add(track);
              }
            }
          } else {
            throw unexpected();
          }
        }
      } else if ("external-ids".equals(localName)) {
        artist.setExternalIds(unmarshallExternalIds());
      } else if (!unmarshalRestrictedMedia(artist)) {
        throw unexpected();
      }

    }

    if (fullyLoaded != null) {
      artist.setLoaded(fullyLoaded);
      artist = (Artist) store.persist(artist);
    }

    return artist;
  }

  /**
   * <xs:complexType name="imageType">
   * <xs:sequence>
   * <xs:element type="hex_40" name="id" minOccurs="0"/>
   * <xs:element name="width" minOccurs="0" type="xs:short"/>
   * <xs:element type="xs:short" name="height" minOccurs="0"/>
   * </xs:sequence>
   * </xs:complexType>
   *
   * @return
   * @throws XMLStreamException
   */
  public Image unmarshallImage() throws XMLStreamException {
    Image image = null;

    int eventType;
    while ((eventType = skip()) == XMLStreamConstants.START_ELEMENT) {
      String localName = xmlr.getLocalName();
      if ("id".equals(localName)) {
        image = store.getImage(xmlr.getElementText());
      } else if ("width".equals(localName)) {
        image.setWidth(Integer.valueOf(xmlr.getElementText()));
      } else if ("height".equals(localName)) {
        image.setHeight(Integer.valueOf(xmlr.getElementText()));
      } else {
        throw unexpected();
      }
    }

    return image;
  }


  public XMLStreamException unexpected() {
    String tag = xmlr.getLocalName();
    String value;
    try {
      value = xmlr.getElementText();
    } catch (Exception e) {
      value = "[error]";
    }


    StringBuilder sb = new StringBuilder();
    sb.append("<").append(tag).append(">");
    sb.append(value);
    sb.append("</").append(tag).append(">");


    return new XMLStreamException(sb.toString() + "\n" + xmlr.getLocation().toString());
  }


  public int skip() throws XMLStreamException {
    int eventType = XMLEvent.COMMENT;
    while (eventType != XMLEvent.START_ELEMENT && eventType != XMLEvent.END_ELEMENT) {
      if (eventType == XMLEvent.ATTRIBUTE) {
        System.currentTimeMillis();
      }
      eventType = xmlr.next();
    }
    return eventType;
  }


  public static String getEventTypeString(int eventType) {
    switch (eventType) {
      case XMLEvent.START_ELEMENT:
        return "START_ELEMENT";
      case XMLEvent.END_ELEMENT:
        return "END_ELEMENT";
      case XMLEvent.PROCESSING_INSTRUCTION:
        return "PROCESSING_INSTRUCTION";
      case XMLEvent.CHARACTERS:
        return "CHARACTERS";
      case XMLEvent.COMMENT:
        return "COMMENT";
      case XMLEvent.START_DOCUMENT:
        return "START_DOCUMENT";
      case XMLEvent.END_DOCUMENT:
        return "END_DOCUMENT";
      case XMLEvent.ENTITY_REFERENCE:
        return "ENTITY_REFERENCE";
      case XMLEvent.ATTRIBUTE:
        return "ATTRIBUTE";
      case XMLEvent.DTD:
        return "DTD";
      case XMLEvent.CDATA:
        return "CDATA";
      case XMLEvent.SPACE:
        return "SPACE";
    }
    return "UNKNOWN_EVENT_TYPE ,   " + eventType;
  }

  public String getString() throws XMLStreamException {
    String text = xmlr.getElementText();
    if ("".equals(text)) {
      return null;
    }
    return text;
  }

  public Float getFloat() throws XMLStreamException {
    String text = xmlr.getElementText();
    if ("".equals(text)) {
      return null;
    }
    return Float.valueOf(text);
  }

  public Integer getInteger() throws XMLStreamException {
    String text = xmlr.getElementText();
    if ("".equals(text)) {
      return null;
    }
    return Integer.valueOf(text);
  }

}
