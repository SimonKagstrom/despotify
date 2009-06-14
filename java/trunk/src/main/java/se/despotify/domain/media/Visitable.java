package se.despotify.domain.media;

import org.domdrides.entity.Entity;

import java.io.Serializable;

/**
 * @since 2009-apr-20 20:01:54
 */
public interface Visitable<IdType extends Serializable> extends Entity<IdType>, Serializable {

  public abstract void accept(Visitor visitor);

}
