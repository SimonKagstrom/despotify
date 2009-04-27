package se.despotify.domain.media;

import se.despotify.util.ChecksumCalculator;

import java.io.Serializable;
import java.util.*;

/**
 * An ArrayList<Track> decorator with utillity methods.
 */
public class MediaList<M extends Media> implements List<M>, Serializable {

  public static long serialVersionUID = 1l;

  private List<M> decorated;

  public M get(byte[] UUID) {
    for (M m : this) {
      if (Arrays.equals(UUID, m.getUUID())) {
        return m;
      }
    }
    return null;
  }

  private long checksum() {
    ChecksumCalculator calculator = new ChecksumCalculator();
    for (M m : this) {
      m.accept(calculator);
    }
    return calculator.getValue();
  }

  public MediaList() {
    this(100);
  }

  public MediaList(int inialCapacity) {
    decorated = new ArrayList<M>(inialCapacity);
  }

  @Override
  public int size() {
    return decorated.size();
  }

  @Override
  public boolean isEmpty() {
    return decorated.isEmpty();
  }

  @Override
  public boolean contains(Object o) {
    return decorated.contains(o);
  }

  @Override
  public Iterator<M> iterator() {
    return decorated.iterator();
  }

  @Override
  public Object[] toArray() {
    return decorated.toArray();
  }

  @SuppressWarnings({"SuspiciousToArrayCall"})
  @Override
  public <T> T[] toArray(T[] ts) {
    return decorated.toArray(ts);
  }

  @Override
  public boolean add(M m) {
    return decorated.add(m);
  }

  @Override
  public boolean remove(Object o) {
    return decorated.remove(o);
  }

  @Override
  public boolean containsAll(Collection<?> objects) {
    return decorated.containsAll(objects);
  }

  @Override
  public boolean addAll(Collection<? extends M> tracks) {
    return decorated.addAll(tracks);
  }

  @Override
  public boolean addAll(int i, Collection<? extends M> tracks) {
    return decorated.addAll(i, tracks);
  }

  @Override
  public boolean removeAll(Collection<?> objects) {
    return decorated.removeAll(objects);
  }

  @Override
  public boolean retainAll(Collection<?> objects) {
    return decorated.retainAll(objects);
  }

  @Override
  public void clear() {
    decorated.clear();
  }

  @Override
  public M get(int i) {
    return decorated.get(i);
  }

  @Override
  public M set(int i, M m) {
    return decorated.set(i, m);
  }

  @Override
  public void add(int i, M m) {
    decorated.add(i, m);
  }

  @Override
  public M remove(int i) {
    return decorated.remove(i);
  }

  @Override
  public int indexOf(Object o) {
    return decorated.indexOf(o);
  }

  @Override
  public int lastIndexOf(Object o) {
    return decorated.lastIndexOf(o);
  }

  @Override
  public ListIterator<M> listIterator() {
    return decorated.listIterator();
  }

  @Override
  public ListIterator<M> listIterator(int i) {
    return decorated.listIterator(i);
  }

  @Override
  public MediaList<M> subList(int fromIndex, int toIndex) {
    MediaList<M> subList = new MediaList<M>(toIndex - fromIndex);
    for (int index = fromIndex; index <= toIndex; index++) {
      subList.add(get(index));
    }
    return subList;
  }


  @Override
  public String toString() {
    return "TrackList{" +
        "decorated=" + decorated +
        '}';
  }
}