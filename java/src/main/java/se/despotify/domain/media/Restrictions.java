package se.despotify.domain.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.util.XMLElement;

import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * @since 2009-apr-30 08:31:26
 */
public class Restrictions {

  protected static Logger log = LoggerFactory.getLogger(Restrictions.class);


  /**
   * AT,BE,CH,CN,CZ,DK,ES,FI,GB,HK,HU,IE,IL,IN,IT,MY,NL,NO,NZ,PL,PT,RU,SE,SG,SK,TR,TW,ZA
   */
  private Set<String> allowed;

  /**
   * AT,BE,CH,CN,CZ,DK,ES,FI,GB,HK,HU,IE,IL,IN,IT,MY,NL,NO,NZ,PL,PT,RU,SE,SG,SK,TR,TW,ZA
   */
  private Set<String> forbidden;

  /**
   * free,daypass,premium
   */
  private Set<String> catalogues;

  public Set<String> getAllowed() {
    return allowed;
  }

  public void setAllowed(Set<String> allowed) {
    this.allowed = allowed;
  }

  public Set<String> getCatalogues() {
    return catalogues;
  }

  public void setCatalogues(Set<String> catalogues) {
    this.catalogues = catalogues;
  }

  public Set<String> getForbidden() {
    return forbidden;
  }

  public void setForbidden(Set<String> forbidden) {
    this.forbidden = forbidden;
  }

  public static Restrictions fromXMLElement(XMLElement restrictionsNode) {
    Restrictions restrictions = new Restrictions();

    for (XMLElement restrictionNode : restrictionsNode.getChildren()) {

      if (!"restriction".equals(restrictionNode.getElement().getNodeName())) {
        log.warn("Unknown restrictions child node " + restrictionNode.getElement().getNodeName());
      } else {

        if (restrictionNode.hasChild("allowed")) {
          restrictions.setAllowed(new LinkedHashSet<String>(Arrays.asList(restrictionNode.getChildText("allowed").split(","))));
        }
        if (restrictionNode.hasChild("forbidden")) {
          restrictions.setForbidden(new LinkedHashSet<String>(Arrays.asList(restrictionNode.getChildText("forbidden").split(","))));
        }
        if (restrictionNode.hasChild("catalouges")) {
          restrictions.setCatalogues(new LinkedHashSet<String>(Arrays.asList(restrictionNode.getChildText("catalouges").split(","))));
        }
      }

    }
    return restrictions;
  }

  @Override
  public String toString() {
    return "Restrictions{" +
        "allowed=" + allowed +
        ", forbidden=" + forbidden +
        ", catalogues=" + catalogues +
        '}';
  }
}
