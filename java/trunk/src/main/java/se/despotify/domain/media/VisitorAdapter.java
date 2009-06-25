package se.despotify.domain.media;

/**
 * @since 2009-apr-20 20:49:52
 */
public class VisitorAdapter implements Visitor {

  @Override
  public void visit(Album album) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void visit(Artist artist) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void visit(Playlist playlist) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void visit(Track track) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void visit(Image image) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void visit(File file) {
    throw new UnsupportedOperationException();
  }
}
