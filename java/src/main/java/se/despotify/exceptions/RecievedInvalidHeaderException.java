package se.despotify.exceptions;

/**
 * @author kalle
 * @since 2009-jun-11 13:07:10
 */
public class RecievedInvalidHeaderException extends DespotifyException {

  public RecievedInvalidHeaderException() {
  }

  public RecievedInvalidHeaderException(String message) {
    super(message);
  }
}
